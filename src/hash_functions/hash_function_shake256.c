#include <hash_functions/hash_functions.h>
#include <hash_functions/hash_function_shake256.h>

/* Note that the SHAKE-256 implementation provided is specifically tailored to EdDSA use and so
 * uses a fixed-length 114-byte output. */

static inline void absorb_block(shake256_state * state, const u8 * block, shake256_working_variables * working_variables);
static inline void keccak_f(u64 A[5][5], u64 B[5][5], u64 C[5], u64 D[5]);
static inline void keccak_theta(u64 A[5][5], u64 C[5], u64 D[5]);
static inline void keccak_chi(u64 A[5][5], const u64 B[5][5]);
static inline u64 load_64(const u8 src[8]);
static inline void store_64(u8 * dst, const u64 * src, int bytecount);

void hash_shake256(u8 * output, const struct iovec * iov, int iovcnt) {

    /* Start with an empty state */
    shake256_state state = {};
    shake256_working_variables working_variables;
    u8 block_buffer[SHAKE256_BLOCK_SIZE_BYTES];

    /* Store how much the block buffer has been filled by a previous iteration */
    size_t where_were_we = 0;

    /* Iterate over all fields in the iovec array and for each do the Absorb stage */
    for (int i = 0; i < iovcnt; i++) {

        size_t input_length = iov[i].iov_len;
        const u8 * input = iov[i].iov_base;

        size_t remaining_space = SHAKE256_BLOCK_SIZE_BYTES - where_were_we;
        if (input_length >= remaining_space) {

            /* Process whatever was left in the buffer by a previous iteration along with
             * any data appended now */
            (void) memcpy(&block_buffer[where_were_we], input, remaining_space);
            input += remaining_space;
            input_length -= remaining_space;

            absorb_block(&state, block_buffer, &working_variables);

            /* Continue processing full blocks of the current input buffer (read it directly from the
             * input buffer not from the intermediate block buffer) */
            while (input_length >= SHAKE256_BLOCK_SIZE_BYTES) {

                absorb_block(&state, input, &working_variables);
                input += SHAKE256_BLOCK_SIZE_BYTES;
                input_length -= SHAKE256_BLOCK_SIZE_BYTES;
            }

            where_were_we = 0;
            /* At this point we are possibly left with some bytes remaining in the input buffer (fewer
             * than SHAKE256_BLOCK_SIZE_BYTES) - put this remainder in the intermediate block buffer
             * to be used by the next iteration */
        }

        (void) memcpy(&block_buffer[where_were_we], input, input_length);
        where_were_we += input_length;
    }

    /* Apply the zero padding before putting the SHAKE256-specific pad byte 0x1F so that
     * we don't have to worry about the case where_were_we == SHAKE256_BLOCK_SIZE_BYTES - 1 */
    (void) memset(&block_buffer[where_were_we], 0, SHAKE256_BLOCK_SIZE_BYTES - where_were_we);
    block_buffer[where_were_we] = 0x1F;
    /* Toggle the last bit of the padding */
    block_buffer[SHAKE256_BLOCK_SIZE_BYTES - 1] |= 0x80;
    /* Absorb the final block */
    absorb_block(&state, block_buffer, &working_variables);

    /* Squeeze 114 bytes of result */
    store_64(output, state.outer_state, SHAKE256_OUTPUT_SIZE_BYTES);

    /* Purge the state and the working variables */
    purge_secrets(&state, sizeof(state));
    purge_secrets(&working_variables, sizeof(working_variables));
}

static inline void absorb_block(shake256_state * state, const u8 * block, shake256_working_variables * working_variables) {

    /* XOR the block buffer with*/
    for (int j = 0; j < sizeof(state->outer_state) / sizeof(state->outer_state[0]); j++) {

        state->outer_state[j] ^= load_64(&block[j << 3]);
    }
    /* Apply the Keccak-f permutation to the state */
    keccak_f(
        state->permutation_state_array,
        working_variables->B,
        working_variables->C,
        working_variables->D
    );
}

static inline void keccak_f(u64 A[5][5], u64 B[5][5], u64 C[5], u64 D[5]) {

    for (int i = 0; i < SHAKE256_ROUNDS_COUNT; i++) {

        /* Do the theta step */
        keccak_theta(A, C, D);

        /* Do the rho and pi steps */
        B[0][0] = ROT(A[0][0], rotation_offsets[0][0]);
        B[3][1] = ROT(A[1][0], rotation_offsets[1][0]);
        B[1][2] = ROT(A[2][0], rotation_offsets[2][0]);
        B[4][3] = ROT(A[3][0], rotation_offsets[3][0]);
        B[2][4] = ROT(A[4][0], rotation_offsets[4][0]);
        B[2][0] = ROT(A[0][1], rotation_offsets[0][1]);
        B[0][1] = ROT(A[1][1], rotation_offsets[1][1]);
        B[3][2] = ROT(A[2][1], rotation_offsets[2][1]);
        B[1][3] = ROT(A[3][1], rotation_offsets[3][1]);
        B[4][4] = ROT(A[4][1], rotation_offsets[4][1]);
        B[4][0] = ROT(A[0][2], rotation_offsets[0][2]);
        B[2][1] = ROT(A[1][2], rotation_offsets[1][2]);
        B[0][2] = ROT(A[2][2], rotation_offsets[2][2]);
        B[3][3] = ROT(A[3][2], rotation_offsets[3][2]);
        B[1][4] = ROT(A[4][2], rotation_offsets[4][2]);
        B[1][0] = ROT(A[0][3], rotation_offsets[0][3]);
        B[4][1] = ROT(A[1][3], rotation_offsets[1][3]);
        B[2][2] = ROT(A[2][3], rotation_offsets[2][3]);
        B[0][3] = ROT(A[3][3], rotation_offsets[3][3]);
        B[3][4] = ROT(A[4][3], rotation_offsets[4][3]);
        B[3][0] = ROT(A[0][4], rotation_offsets[0][4]);
        B[1][1] = ROT(A[1][4], rotation_offsets[1][4]);
        B[4][2] = ROT(A[2][4], rotation_offsets[2][4]);
        B[2][3] = ROT(A[3][4], rotation_offsets[3][4]);
        B[0][4] = ROT(A[4][4], rotation_offsets[4][4]);

        /* Do the chi step */
        keccak_chi(A, B);

        /* Do the iota step */
        A[0][0] ^= round_constants[i];
    }
}

static inline void keccak_theta(u64 A[5][5], u64 C[5], u64 D[5]) {

    C[0] = A[0][0] ^ A[1][0] ^ A[2][0] ^ A[3][0] ^ A[4][0];
    C[1] = A[0][1] ^ A[1][1] ^ A[2][1] ^ A[3][1] ^ A[4][1];
    C[2] = A[0][2] ^ A[1][2] ^ A[2][2] ^ A[3][2] ^ A[4][2];
    C[3] = A[0][3] ^ A[1][3] ^ A[2][3] ^ A[3][3] ^ A[4][3];
    C[4] = A[0][4] ^ A[1][4] ^ A[2][4] ^ A[3][4] ^ A[4][4];

    D[0] = C[4] ^ ROT(C[1], 1);
    D[1] = C[0] ^ ROT(C[2], 1);
    D[2] = C[1] ^ ROT(C[3], 1);
    D[3] = C[2] ^ ROT(C[4], 1);
    D[4] = C[3] ^ ROT(C[0], 1);

    A[0][0] ^= D[0];   A[0][1] ^= D[1];   A[0][2] ^= D[2];   A[0][3] ^= D[3];   A[0][4] ^= D[4];
    A[1][0] ^= D[0];   A[1][1] ^= D[1];   A[1][2] ^= D[2];   A[1][3] ^= D[3];   A[1][4] ^= D[4];
    A[2][0] ^= D[0];   A[2][1] ^= D[1];   A[2][2] ^= D[2];   A[2][3] ^= D[3];   A[2][4] ^= D[4];
    A[3][0] ^= D[0];   A[3][1] ^= D[1];   A[3][2] ^= D[2];   A[3][3] ^= D[3];   A[3][4] ^= D[4];
    A[4][0] ^= D[0];   A[4][1] ^= D[1];   A[4][2] ^= D[2];   A[4][3] ^= D[3];   A[4][4] ^= D[4];
}

static inline void keccak_chi(u64 A[5][5], const u64 B[5][5]) {

    A[0][0] = B[0][0] ^ (~B[0][1] & B[0][2]);
    A[1][0] = B[1][0] ^ (~B[1][1] & B[1][2]);
    A[2][0] = B[2][0] ^ (~B[2][1] & B[2][2]);
    A[3][0] = B[3][0] ^ (~B[3][1] & B[3][2]);
    A[4][0] = B[4][0] ^ (~B[4][1] & B[4][2]);
    A[0][1] = B[0][1] ^ (~B[0][2] & B[0][3]);
    A[1][1] = B[1][1] ^ (~B[1][2] & B[1][3]);
    A[2][1] = B[2][1] ^ (~B[2][2] & B[2][3]);
    A[3][1] = B[3][1] ^ (~B[3][2] & B[3][3]);
    A[4][1] = B[4][1] ^ (~B[4][2] & B[4][3]);
    A[0][2] = B[0][2] ^ (~B[0][3] & B[0][4]);
    A[1][2] = B[1][2] ^ (~B[1][3] & B[1][4]);
    A[2][2] = B[2][2] ^ (~B[2][3] & B[2][4]);
    A[3][2] = B[3][2] ^ (~B[3][3] & B[3][4]);
    A[4][2] = B[4][2] ^ (~B[4][3] & B[4][4]);
    A[0][3] = B[0][3] ^ (~B[0][4] & B[0][0]);
    A[1][3] = B[1][3] ^ (~B[1][4] & B[1][0]);
    A[2][3] = B[2][3] ^ (~B[2][4] & B[2][0]);
    A[3][3] = B[3][3] ^ (~B[3][4] & B[3][0]);
    A[4][3] = B[4][3] ^ (~B[4][4] & B[4][0]);
    A[0][4] = B[0][4] ^ (~B[0][0] & B[0][1]);
    A[1][4] = B[1][4] ^ (~B[1][0] & B[1][1]);
    A[2][4] = B[2][4] ^ (~B[2][0] & B[2][1]);
    A[3][4] = B[3][4] ^ (~B[3][0] & B[3][1]);
    A[4][4] = B[4][4] ^ (~B[4][0] & B[4][1]);
}

static inline u64 load_64(const u8 src[8]) {

    u64 dst;
#if FE3C_LILENDIAN_TARGET
    /* Target already little endian - copy the bytes with no shifts */
    (void) memcpy(&dst, src, 8);
#else
    /* Big-endian target or endianness unknown (take the safe route) */
    dst  = (u64) src[0];
    dst |= (u64) src[1] << 8;
    dst |= (u64) src[2] << 16;
    dst |= (u64) src[3] << 24;
    dst |= (u64) src[4] << 32;
    dst |= (u64) src[5] << 40;
    dst |= (u64) src[6] << 48;
    dst |= (u64) src[7] << 56;
#endif /* FE3C_LILENDIAN_TARGET */
    return dst;
}

static inline void store_64(u8 * dst, const u64 * src, int bytecount) {

    int wordcount = bytecount >> 3;
    int trailer = bytecount & 0x7;
#if FE3C_LILENDIAN_TARGET
    /* Target already little endian - copy the full words with no shifts */
    (void) memcpy(dst, src, wordcount << 3);
#else
    /* Big-endian target or endianness unknown (take the safe route) */
    for (size_t i = 0; i < wordcount; i++) {

        dst[8 * i + 0] = (u8) (src[i] >> 0 * 8);
        dst[8 * i + 1] = (u8) (src[i] >> 1 * 8);
        dst[8 * i + 2] = (u8) (src[i] >> 2 * 8);
        dst[8 * i + 3] = (u8) (src[i] >> 3 * 8);
        dst[8 * i + 4] = (u8) (src[i] >> 4 * 8);
        dst[8 * i + 5] = (u8) (src[i] >> 5 * 8);
        dst[8 * i + 6] = (u8) (src[i] >> 6 * 8);
        dst[8 * i + 7] = (u8) (src[i] >> 7 * 8);
    }
#endif /* FE3C_LILENDIAN_TARGET */
    /* Set the trailer manually for either endianness to not risk reading
     * uninitialized stack later */
    for (size_t j = 0; j < trailer; j++) {

        dst[wordcount * 8 + j] = (u8) (src[wordcount] >> j * 8);
    }
}
