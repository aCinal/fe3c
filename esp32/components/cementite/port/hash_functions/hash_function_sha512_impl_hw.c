#include <port/hash_functions/hash_function_sha512_esp32.h>
#include <utils/utils.h>

/* Redefine some constants instead of including hash_functions/hash_function_sha512.h, since the
 * header actually defines the round constants array among other things (TODO: maybe it's worth
 * moving that to a separate translation unit to keep the header light) */
#define SHA512_BLOCK_SIZE_BYTES             ( 1024 / 8 )     /**< The size of the input block in bytes (128) */
#define SHA512_STATE_WORD_COUNT             ( 8 )            /**< The size of the state */

/* Play nice with mbedtls which is the default crypto library for esp-idf
 * - use their locking primitives */
#include <soc/soc_caps.h>
#if SOC_SHA_SUPPORT_PARALLEL_ENG
    #include <sha/sha_parallel_engine.h>
#elif SOC_SHA_SUPPORT_DMA
    #include <sha/sha_dma.h>
#else
    #include <sha/sha_block.h>
#endif /* SOC_SHA_SUPPORT_PARALLEL_ENG */

static inline int sha512_compress(const u8 * input_block, u32 * first_block);
static inline void sha512_read_final(u8 * output_buffer);
static inline void sha512_purge_accelerator(void);
static inline void store_64(u8 * dst, const u64 * src, size_t wordcount);

int sha512_impl_hw(u8 * output, const struct iovec * iov, int iovcnt) {

    FE3C_SANITY_CHECK(output, NULL);
    FE3C_SANITY_CHECK(iov || iovcnt == 0, NULL);

    int ret = 0;
    /* Allocate an input block buffer of 1024 bits. SHA-512 processes the messages in
     * blocks of 1024 bits. The input message will be written into this buffer */
    u8 block_buffer[SHA512_BLOCK_SIZE_BYTES];
    /* Note that RFC 6234 defines the message length to be the number of bits and
     * we require two 64-bit words to store it */
    u64 message_length_high = 0;
    u64 message_length_low = 0;
    /* Store how much the block buffer has been filled by a previous iteration */
    size_t where_were_we = 0;
    u32 is_first_block = 1;

    /* Iterate over all fields in the iovec array and for each do the Update stage */
    for (int i = 0; i < iovcnt; i++) {

        /* Update the state for each field */
        size_t input_length = iov[i].iov_len;
        const u8 * input = iov[i].iov_base;

        /* Update the length */

        /* Compute the length in bits, i.e. input_length times 8 (or left-shifted by 3) */
        u64 temp_length_low = ( (u64) input_length ) << 3;
        /* The three top bits of input_length cannot be multiplied by 8, shift them down
         * and save in the high part of the bitlength */
        u64 temp_length_high = ( (u64) input_length ) >> 61;
        /* Check for overflow of the low word */
        if ((message_length_low += temp_length_low) < temp_length_low) {
            /* Overflow into the high word occurred */
            message_length_high++;
        }
        /* Add the high part of input_length into the aggregate message length */
        message_length_high += temp_length_high;

        /* Write the message into the temporary buffer of size 1024 bits - once this
         * buffer is filled, process it and update the state */
        size_t remaining_space = SHA512_BLOCK_SIZE_BYTES - where_were_we;
        if (input_length >= remaining_space) {

            /* Process whatever was left in the buffer by a previous iteration along with
             * any data appended now */
            (void) memcpy(&block_buffer[where_were_we], input, remaining_space);
            input += remaining_space;
            input_length -= remaining_space;
            /* TODO: Note that we are being wasteful with our calls to sha512_compress in the case
             * when the SoC supports DMA-SHA (SOC_SHA_SUPPORT_DMA). We could offload partitioning
             * each iovec into blocks and manually copying it into the accelerator to the DMA engine.
             * This may be beneficial for large messages. */
            if (sha512_compress(block_buffer, &is_first_block)) {
                ret = -1;
                goto out;
            }

            /* Continue processing full 1024-bit blocks of the current input buffer (read it directly from the
             * input buffer not from the intermediate block buffer) */
            while (input_length >= SHA512_BLOCK_SIZE_BYTES) {

                if (sha512_compress(input, &is_first_block)) {
                    ret = -1;
                    goto out;
                }
                input += SHA512_BLOCK_SIZE_BYTES;
                input_length -= SHA512_BLOCK_SIZE_BYTES;
            }

            where_were_we = 0;
            /* At this point we are possibly left with some bytes remaining in the input buffer (fewer
             * than SHA512_BLOCK_SIZE_BYTES) - put this remainder in the intermediate block buffer
             * to be used by the next iteration */
        }

        (void) memcpy(&block_buffer[where_were_we], input, input_length);
        where_were_we += input_length;
    }

    /* Finalize the hash */

    /* Note that the last 16 octets are used to store the message length (Merkle-Damgard strengthening)
     * - check if we have some more free space in the current block */
    if (where_were_we < SHA512_BLOCK_SIZE_BYTES - 16) {

        /* Pad the additional free space with a one followed by zeroes */
        block_buffer[where_were_we] = 0x80;
        where_were_we++;
        /* Use a plain memset here, a compliant compiler cannot optimize it away (which is not the case
         * in places where we use purge_secrets instead) */
        (void) memset(&block_buffer[where_were_we], 0, SHA512_BLOCK_SIZE_BYTES - 16 - where_were_we);

    } else {

        /* No room for the message length (and the mandatory padding bit) - pad the current block to the end
         * and process it */
        block_buffer[where_were_we] = 0x80;
        where_were_we++;
        (void) memset(&block_buffer[where_were_we], 0, SHA512_BLOCK_SIZE_BYTES - where_were_we);
        if (sha512_compress(block_buffer, &is_first_block)) {
            ret = -1;
            goto out;
        }
        /* Pad the next block with all zeroes up to the last 16 octets */
        (void) memset(block_buffer, 0, SHA512_BLOCK_SIZE_BYTES - 16);
    }

    /* Do the Merkle-Damgard strengthening and copy the message length into the last 16 octets of the padding */
    store_64(&block_buffer[SHA512_BLOCK_SIZE_BYTES - 16], &message_length_high, 1);
    store_64(&block_buffer[SHA512_BLOCK_SIZE_BYTES - 8], &message_length_low, 1);
    /* Do one final block processing */
    if (sha512_compress(block_buffer, &is_first_block)) {
        ret = -1;
        goto out;
    }
    /* Copy the final state to the output buffer */
    sha512_read_final(output);
#if SOC_SHA_SUPPORT_PARALLEL_ENG
    /* The parallel engine implementation is the only one true to its word (API) that actually returns
     * the _state_, i.e., a sequence of big-endian 64-bit integers. DMA and block seem to operate
     * internally in native endianness (little-endian) and calls to esp_sha_read_digest_state()
     * return the state in native endianness as well. For parallel engine, swap the order manually */
    store_64(output, (u64 *) output, SHA512_STATE_WORD_COUNT);
#endif /* SOC_SHA_SUPPORT_PARALLEL_ENG */

out:
    /* Purge the block buffer */
    purge_secrets(block_buffer, sizeof(block_buffer));
    /* Erase the state of the accelerator */
    sha512_purge_accelerator();

    return ret;
}

sha512_impl sha512_try_lock_hw(void) {

#if SOC_SHA_SUPPORT_PARALLEL_ENG
    if (esp_sha_try_lock_engine(SHA2_512)) {

        /* Engine acquired, proceed with hardware-accelerated hashing */
        return sha512_hw_acceleration;

    } else {

        /* Engine busy, fall back to software implementation */
        return sha512_sw_fallback;
    }
#else
    /* When DMA or blocking variant is used, no API is exposed by mbedtls
     * to try-lock the accelerator. */
    esp_sha_acquire_hardware();
    return sha512_hw_acceleration;
#endif
}

void sha512_release_hw(void) {

#if SOC_SHA_SUPPORT_PARALLEL_ENG
    esp_sha_unlock_engine(SHA2_512);
#else
    esp_sha_release_hardware();
#endif
}

static inline int sha512_compress(const u8 * input_block, u32 * first_block) {

    u32 is_first_block = *first_block;
#if SOC_SHA_SUPPORT_DMA
    if (esp_sha_dma(SHA2_512, input_block, SHA512_BLOCK_SIZE_BYTES, NULL, 0, (bool) is_first_block)) {
        return -1;
    }
#else
    esp_sha_block(SHA2_512, input_block, (bool) is_first_block);
#endif
    *first_block = 0;
    return 0;
}

static inline void sha512_read_final(u8 * output_buffer) {

    esp_sha_read_digest_state(SHA2_512, output_buffer);
}

static inline void sha512_purge_accelerator(void) {

#if !FE3C_SKIP_ZEROIZATION
    /* Erase the state of the accelerator */
#if SOC_SHA_SUPPORT_DMA
    u64 poison[SHA512_STATE_WORD_COUNT] = {};
    esp_sha_write_digest_state(SHA2_512, poison);
#else
    u8 poison[SHA512_BLOCK_SIZE_BYTES] = {};
    esp_sha_block(SHA2_512, poison, true);
#endif /* SOC_SHA_SUPPORT_DMA */
#endif /* !FE3C_SKIP_ZEROIZATION */
}

static inline void store_64(u8 * dst, const u64 * src, size_t wordcount) {

    for (size_t i = 0; i < wordcount; i++) {

        *(u64 *) &dst[8 * i] = __builtin_bswap64(src[i]);
    }
}
