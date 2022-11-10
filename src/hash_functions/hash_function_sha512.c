#include <hash_functions/hash_functions.h>
#include <hash_functions/hash_function_sha512.h>

/* TODO: Provide an optimized implementation for 32-bit targets (and even more so
 * for targets with hardware support for SHA, e.g. ESP32) */

static inline void sha512_compress(u64 * state, const u8 * input_block, u64 * schedule, sha512_working_variables * work);
static inline void sha512_prepare_message_schedule(u64 * schedule, const u8 * input);
static inline void store_bigendian_64(u8 * dst, const u64 * src, size_t wordcount);
static inline void load_bigendian_64(u64 * dst, const u8 * src, size_t wordcount);

void hash_sha512(u8 * output, const struct iovec * iov, int iovcnt) {

    FE3C_SANITY_CHECK(output);
    FE3C_SANITY_CHECK(iov || iovcnt == 0);

    /* For big endian targets we could use the output buffer directly for the state
     * and avoid a copy at the end, but we go for a more readable code by always
     * allocating the state on stack */
    u64 state[SHA512_STATE_WORD_COUNT];
    /* Set the state to the initialization vector */
    (void) memcpy(state, state_initialization_vector, sizeof(state_initialization_vector));

    /* Allocate a scratchpad for a message schedule (80 words) + eight working variables.
     * Note that the scratchpad does not need to be shared between calls to sha512_compress
     * and so could be allocated in sha512_compress' frame. We would rather have it here
     * so that we can purge/burn it once we're done with compressing altogether */
    u64 message_schedule[SHA512_MESSAGE_SCHEDULE_WORD_COUNT];
    sha512_working_variables working_variables[1];

    /* Allocate an input block buffer of 1024 bits. SHA-512 processes the messages in
     * blocks of 1024 bits. The input message will be written into this buffer */
    u8 block_buffer[SHA512_INPUT_BLOCK_SIZE_BYTES];
    /* Note that RFC 6234 defines the message length to be the number of bits and
     * we require two 64-bit words to store it */
    u64 message_length_high = 0;
    u64 message_length_low = 0;
    /* Store how much the block buffer has been filled by a previous iteration */
    size_t where_were_we = 0;


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
        size_t remaining_space = SHA512_INPUT_BLOCK_SIZE_BYTES - where_were_we;
        if (input_length >= remaining_space) {

            /* Process whatever was left in the buffer by a previous iteration along with
             * any data appended now */
            (void) memcpy(&block_buffer[where_were_we], input, remaining_space);
            input += remaining_space;
            input_length -= remaining_space;
            sha512_compress(state, block_buffer, message_schedule, working_variables);

            /* Continue processing full 1024-bit blocks of the current input buffer (read it directly from the
             * input buffer not from the intermediate block buffer) */
            while (input_length >= SHA512_INPUT_BLOCK_SIZE_BYTES) {

                sha512_compress(state, input, message_schedule, working_variables);
                input += SHA512_INPUT_BLOCK_SIZE_BYTES;
                input_length -= SHA512_INPUT_BLOCK_SIZE_BYTES;
            }

            where_were_we = 0;
            /* At this point we are possibly left with some bytes remaining in the input buffer (less than 1024 bits)
             * - put this remainder in the intermediate block buffer to be used by the next iteration */
        }

        (void) memcpy(&block_buffer[where_were_we], input, input_length);
        where_were_we += input_length;
    }

    /* Finalize the hash */

    /* Note that the last 16 octets are used to store the message length (Merkle-Damgard strengthening)
     * - check if we have some more free space in the current block */
    if (where_were_we < SHA512_INPUT_BLOCK_SIZE_BYTES - 16) {

        /* Pad the additional free space with a one followed by zeroes */
        block_buffer[where_were_we] = 0x80;
        where_were_we++;
        /* Use a plain memset here, a compliant compiler cannot optimize it away (which is not the case
         * in places where we use purge_secrets instead) */
        (void) memset(&block_buffer[where_were_we], 0, SHA512_INPUT_BLOCK_SIZE_BYTES - 16 - where_were_we);

    } else {

        /* No room for the message length (and the mandatory padding bit) - pad the current block to the end
         * and process it */
        block_buffer[where_were_we] = 0x80;
        where_were_we++;
        (void) memset(&block_buffer[where_were_we], 0, SHA512_INPUT_BLOCK_SIZE_BYTES - where_were_we);
        sha512_compress(state, block_buffer, message_schedule, working_variables);
        /* Pad the next block with all zeroes up to the last 16 octets */
        (void) memset(block_buffer, 0, SHA512_INPUT_BLOCK_SIZE_BYTES - 16);
    }

    /* Do the Merkle-Damgard strengthening and copy the message length into the last 16 octets of the padding */
    store_bigendian_64(&block_buffer[SHA512_INPUT_BLOCK_SIZE_BYTES - 16], &message_length_high, 1);
    store_bigendian_64(&block_buffer[SHA512_INPUT_BLOCK_SIZE_BYTES - 8], &message_length_low, 1);
    /* Do one final block processing */
    sha512_compress(state, block_buffer, message_schedule, working_variables);
    /* Copy the final state to the output buffer */
    store_bigendian_64(output, state, SHA512_STATE_WORD_COUNT);

    /* Purge the on-stack state */
    purge_secrets(state, sizeof(state));
    /* Zeroize the scratchpad */
    purge_secrets(message_schedule, sizeof(message_schedule));
    purge_secrets(working_variables, sizeof(working_variables));
}

static inline void sha512_compress(u64 * state, const u8 * input_block, u64 * schedule, sha512_working_variables * work) {

    /* TODO: Study libsodium/OpenSSL's implementation(s) and do some weak unrolling in the loop */

    sha512_prepare_message_schedule(schedule, input_block);
    /* Initialize the working variables */
    work->a = state[0];
    work->b = state[1];
    work->c = state[2];
    work->d = state[3];
    work->e = state[4];
    work->f = state[5];
    work->g = state[6];
    work->h = state[7];

    for (int t = 0; t < SHA512_ROUNDS_COUNT; t++) {

        /* See RFC 6234 for the variable naming convention */
        u64 T1 = work->h + \
                 BSIG1(work->e) + \
                 Ch(work->e, work->f, work->g) + \
                 magic_constants[t] + \
                 schedule[t];
        u64 T2 = BSIG0(work->a) + Maj(work->a, work->b, work->c);

        /* Shift the working variables */
        work->h = work->g;
        work->g = work->f;
        work->f = work->e;
        work->e = work->d + T1;
        work->d = work->c;
        work->c = work->b;
        work->b = work->a;
        work->a = T1 + T2;
    }

    /* Update the state */
    state[0] += work->a;
    state[1] += work->b;
    state[2] += work->c;
    state[3] += work->d;
    state[4] += work->e;
    state[5] += work->f;
    state[6] += work->g;
    state[7] += work->h;
}

static inline void sha512_prepare_message_schedule(u64 * schedule, const u8 * input) {

    /* Write the current input block into the first 16 words of the message schedule */
    load_bigendian_64(schedule, input, SHA512_INPUT_BLOCK_SIZE_BYTES / 8);

    /* Fill in the rest of the message schedule */
    for (int t = SHA512_INPUT_BLOCK_SIZE_BYTES / 8; t < SHA512_MESSAGE_SCHEDULE_WORD_COUNT; t++) {

        /* TODO: Optimize initializing the message schedule - this can be done in parallel to actually
         * computing the hash */
        schedule[t] = \
            SSIG1(schedule[t - 2]) + \
            schedule[t - 7] + \
            SSIG0(schedule[t - 15]) + \
            schedule[t - 16];
    }
}

static inline void store_bigendian_64(u8 * dst, const u64 * src, size_t wordcount) {
#if FE3C_BIGENDIAN_TARGET
    (void) memcpy(src, dst, wordcount * 8);
#else
    /* Little-endian target or endianness unknown (take the safe route) - store the words
     * into bytes manually */
    for (size_t i = 0; i < wordcount; i++) {

        dst[8 * i + 0] = (u8) (src[i] >> 56);
        dst[8 * i + 1] = (u8) (src[i] >> 48);
        dst[8 * i + 2] = (u8) (src[i] >> 40);
        dst[8 * i + 3] = (u8) (src[i] >> 32);
        dst[8 * i + 4] = (u8) (src[i] >> 24);
        dst[8 * i + 5] = (u8) (src[i] >> 16);
        dst[8 * i + 6] = (u8) (src[i] >> 8);
        dst[8 * i + 7] = (u8) (src[i]);
    }
#endif
}

static inline void load_bigendian_64(u64 * dst, const u8 * src, size_t wordcount) {
#if FE3C_BIGENDIAN_TARGET
    (void) memcpy(dst, src, wordcount * 8);
#else
    /* Little-endian target or endianness unknown (take the safe route) - load the bytes
     * into words manually */
    for (size_t i = 0; i < wordcount; i++) {

        dst[i]  = (u64) src[8 * i + 7];
        dst[i] |= (u64) src[8 * i + 6] << 8;
        dst[i] |= (u64) src[8 * i + 5] << 16;
        dst[i] |= (u64) src[8 * i + 4] << 24;
        dst[i] |= (u64) src[8 * i + 3] << 32;
        dst[i] |= (u64) src[8 * i + 2] << 40;
        dst[i] |= (u64) src[8 * i + 1] << 48;
        dst[i] |= (u64) src[8 * i + 0] << 56;
    }
#endif
}
