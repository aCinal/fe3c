#include <hash_functions/hash_function_sha512.h>
#include <port/hash_functions/hash_function_sha512_esp32.h>
#include <utils/utils.h>

/* To reduce clutter, define a symbol for inline asm line termination */
#define _ "\n\t"

/* Redefine the SHA512 functions in assembly */
#undef Ch
#undef Maj
#undef ROTR64
#undef SHR
#undef BSIG0
#undef BSIG1
#undef SSIG0
#undef SSIG1

/**
 * @brief Compute Ch(x, y, z) defined as (x AND y) XOR (NOT(x) AND z)
 * @param[in,out] xlo Register holding the low word of x
 * @param[in,out] xhi Register holding the high word of x
 * @param[in] ylo Register holding the low word of y
 * @param[in] yhi Register holding the high word of y
 * @param[in] zlo Register holding the low word of z
 * @param[in] zhi Register holding the high word of z
 * @note Output is returned via the xlo and xhi registers
 */
#define Ch(xlo, xhi, ylo, yhi, zlo, zhi)             \
    /* Set y to x AND y */                           \
    _ "and    %[" #ylo "], %[" #xlo "], %[" #ylo "]" \
    _ "and    %[" #yhi "], %[" #xhi "], %[" #yhi "]" \
    /* Set x to NOT(x) AND z */                      \
    _ "movi.n %[aux1],     -1"                       \
    _ "xor    %[" #xlo "], %[" #xlo "], %[aux1]"     \
    _ "xor    %[" #xhi "], %[" #xhi "], %[aux1]"     \
    _ "and    %[" #xlo "], %[" #xlo "], %[" #zlo "]" \
    _ "and    %[" #xhi "], %[" #xhi "], %[" #zhi "]" \
    /* Set x to Ch(x, y, z) */                       \
    _ "xor    %[" #xlo "], %[" #xlo "], %[" #ylo "]" \
    _ "xor    %[" #xhi "], %[" #xhi "], %[" #yhi "]"

/**
 * @brief Compute Maj(x, y, z) defined as (x AND y) XOR (x AND z) XOR (y AND z)
 * @param[in,out] xlo Register holding the low word of x
 * @param[in,out] xhi Register holding the high word of x
 * @param[in] ylo Register holding the low word of y
 * @param[in] yhi Register holding the high word of y
 * @param[in] zlo Register holding the low word of z
 * @param[in] zhi Register holding the high word of z
 * @note Output is returned via the xlo and xhi registers
 */
#define Maj(xlo, xhi, ylo, yhi, zlo, zhi)         \
    /* Set aux to x AND y */                      \
    _ "and %[aux1],     %[" #xlo "], %[" #ylo "]" \
    _ "and %[aux2],     %[" #xhi "], %[" #yhi "]" \
    /* Set x to x AND z */                        \
    _ "and %[" #xlo "], %[" #xlo "], %[" #zlo "]" \
    _ "and %[" #xhi "], %[" #xhi "], %[" #zhi "]" \
    /* Set y to y AND z */                        \
    _ "and %[" #ylo "], %[" #ylo "], %[" #zlo "]" \
    _ "and %[" #yhi "], %[" #yhi "], %[" #zhi "]" \
    /* Set x to Maj(x, y, z) */                   \
    _ "xor %[" #xlo "], %[" #xlo "], %[aux1]"     \
    _ "xor %[" #xhi "], %[" #xhi "], %[aux2]"     \
    _ "xor %[" #xlo "], %[" #xlo "], %[" #ylo "]" \
    _ "xor %[" #xhi "], %[" #xhi "], %[" #yhi "]"

/**
 * @brief Rotate a 64-bit word right by a given number of bits
 * @param[in] bits Number of bits to rotate
 * @param[out] outlo Register for the low word of the result
 * @param[out] outhi Register for the high word of the result
 * @param[in] inlo Register with the low word of the input
 * @param[in] inhi Register with the high word of the input
 * @note The number of bits to rotate can be 31 at most
 * @warning No aliasing is supported
 */
#define ROTR64(bits, outlo, outhi, inlo, inhi)                            \
    /* Initialize the shift amount register */                            \
    _ "ssai " #bits                                                       \
    /* Do shift right combined, i.e. concatenate inhi | inlo and shift */ \
    _ "src  %[" #outlo "], %[" #inhi "], %[" #inlo "]"                    \
    _ "src  %[" #outhi "], %[" #inlo "], %[" #inhi "]"

/**
 * @brief Shift a 64-bit word right by a given number of bits
 * @param[in] bits Number of bits to shift
 * @param[out] outlo Register for the low word of the result
 * @param[out] outhi Register for the high word of the result
 * @param[in] inlo Register with the low word of the input
 * @param[in] inhi Register with the high word of the input
 * @note The number of bits to shift can be 31 at most
 * @warning No aliasing is supported
 */
#define SHR(bits, outlo, outhi, inlo, inhi)                               \
    /* Initialize the shift amount register */                            \
    _ "ssai " #bits                                                       \
    /* Do shift right combined, i.e. concatenate inhi | inlo and shift */ \
    _ "src  %[" #outlo "], %[" #inhi "], %[" #inlo "]"                    \
    /* Do shift right logical of the high word */                         \
    _ "srl  %[" #outhi "], %[" #inhi "]"

/**
 * @brief Compute BSIG0(x) defined as ROTR64(28, x) XOR ROTR64(34, x) XOR ROTR64(39, x)
 * @param[out] outlo Register for the low word of the result
 * @param[out] outhi Register for the high word of the result
 * @param[in] inlo Register with the low word of the input
 * @param[in] inhi Register with the high word of the input
 * @note All inputs are preserved
 * @warning No aliasing is supported
 */
#define BSIG0(outlo, outhi, inlo, inhi)           \
    /* Compute ROTR64(39, x) */                   \
    ROTR64(31, aux1, aux2, inlo, inhi)            \
    ROTR64(8, aux3, aux4, aux1, aux2)             \
    /* Compute ROTR64(34, x) */                   \
    ROTR64(20, aux1, aux2, inlo, inhi)            \
    ROTR64(14, outlo, outhi, aux1, aux2)          \
    /* Compute ROTR64(28, x) */                   \
    ROTR64(28, aux1, aux2, inlo, inhi)            \
    /* Compute BSIG0(x) */                        \
    _ "xor %[" #outlo "], %[" #outlo "], %[aux1]" \
    _ "xor %[" #outhi "], %[" #outhi "], %[aux2]" \
    _ "xor %[" #outlo "], %[" #outlo "], %[aux3]" \
    _ "xor %[" #outhi "], %[" #outhi "], %[aux4]"

/**
 * @brief Compute BSIG1(x) defined as ROTR64(14, x) XOR ROTR64(18, x) XOR ROTR64(41, x)
 * @param[out] outlo Register for the low word of the result
 * @param[out] outhi Register for the high word of the result
 * @param[in] inlo Register with the low word of the input
 * @param[in] inhi Register with the high word of the input
 * @note All inputs are preserved
 * @warning No aliasing is supported
 */
#define BSIG1(outlo, outhi, inlo, inhi)           \
    /* Compute ROTR64(41, x) */                   \
    ROTR64(31, aux1, aux2, inlo, inhi)            \
    ROTR64(10, aux3, aux4, aux1, aux2)            \
    /* Compute ROTR64(14, x) */                   \
    ROTR64(14, aux1, aux2, inlo, inhi)            \
    /* Compute ROTR64(18, x) */                   \
    ROTR64(18, outlo, outhi, inlo, inhi)          \
    /* Compute BSIG1(x) */                        \
    _ "xor %[" #outlo "], %[" #outlo "], %[aux1]" \
    _ "xor %[" #outhi "], %[" #outhi "], %[aux2]" \
    _ "xor %[" #outlo "], %[" #outlo "], %[aux3]" \
    _ "xor %[" #outhi "], %[" #outhi "], %[aux4]"

/**
 * @brief Compute SSIG0(x) defined as ROTR64(1, x) XOR ROTR64(8, x) XOR SHR(7, x)
 * @param[out] outlo Register for the low word of the result
 * @param[out] outhi Register for the high word of the result
 * @param[in] inlo Register with the low word of the input
 * @param[in] inhi Register with the high word of the input
 * @note All inputs are preserved
 * @warning No aliasing is supported
 */
#define SSIG0(outlo, outhi, inlo, inhi)           \
    /* Compute ROTR64(1, x) */                    \
    ROTR64(1, aux1, aux2, inlo, inhi)             \
    /* Compute ROTR64(8, x) */                    \
    ROTR64(8, aux3, aux4, inlo, inhi)             \
    /* Compute SHR(7, x) */                       \
    SHR(7, outlo, outhi, inlo, inhi)              \
    /* Compute SSIG0(x) */                        \
    _ "xor %[" #outlo "], %[" #outlo "], %[aux1]" \
    _ "xor %[" #outhi "], %[" #outhi "], %[aux2]" \
    _ "xor %[" #outlo "], %[" #outlo "], %[aux3]" \
    _ "xor %[" #outhi "], %[" #outhi "], %[aux4]"

/**
 * @brief Compute SSIG1(x) defined as ROTR64(19, x) XOR ROTR64(61, x) XOR SHR(6, x)
 * @param[out] outlo Register for the low word of the result
 * @param[out] outhi Register for the high word of the result
 * @param[in] inlo Register with the low word of the input
 * @param[in] inhi Register with the high word of the input
 * @note All inputs are preserved
 * @warning No aliasing is supported
 */
#define SSIG1(outlo, outhi, inlo, inhi)           \
    /* Compute ROTR64(61, x) */                   \
    ROTR64(31, aux3, aux4, inlo, inhi)            \
    ROTR64(30, aux1, aux2, aux3, aux4)            \
    /* Compute ROTR64(19, x) */                   \
    ROTR64(19, aux3, aux4, inlo, inhi)            \
    /* Compute SHR(6, x) */                       \
    SHR(6, outlo, outhi, inlo, inhi)              \
    /* Compute SSIG0(x) */                        \
    _ "xor %[" #outlo "], %[" #outlo "], %[aux1]" \
    _ "xor %[" #outhi "], %[" #outhi "], %[aux2]" \
    _ "xor %[" #outlo "], %[" #outlo "], %[aux3]" \
    _ "xor %[" #outhi "], %[" #outhi "], %[aux4]"

/**
 * @brief Add two 64-bit integers in constant-time
 * @param[out] rlo Register for the low word of the result
 * @param[out] rhi Register for the high word of the result
 * @param[in] alo Register with the low word of the first operand
 * @param[in] ahi Register with the high word of the first operand
 * @param[in] blo Register with the low word of the second operand
 * @param[in] bhi Register with the high word of the second operand
 * @note There is limited support for aliasing, only ahi and alo may alias rhi and rlo,
 *       specifically rlo must be distinct from blo for the carry handling to work
 */
#define CONSTANT_TIME_U64_ADD(rlo, rhi, alo, ahi, blo, bhi)  \
    /* Add the high parts */                                 \
    _ "add.n  %[" #rhi "],  %[" #ahi "],  %[" #bhi "]"       \
    /* Add the low parts */                                  \
    _ "add.n  %[" #rlo "],  %[" #alo "],  %[" #blo "]"       \
    /* Handle the overflow */                                \
    _ "minu   %[aux1],      %[" #rlo "],  %[" #blo "]"       \
    _ "xor    %[aux1],      %[aux1],      %[" #blo "]"       \
    _ "movi.n %[aux2],      1"                               \
    _ "movnez %[aux1],      %[aux2],      %[aux1]"           \
    _ "add.n  %[" #rhi "],  %[" #rhi "],  %[aux1]"

static inline void sha512_compress(u64 * state, const u8 * input_block, u64 * schedule, sha512_working_variables * work);
static inline void sha512_prepare_message_schedule(u64 * schedule, const u8 * input);
static inline void store_64(u8 * dst, const u64 * src, size_t wordcount);
static inline void load_64(u64 * dst, const u8 * src, size_t wordcount);

void sha512_impl_sw(u8 * output, const struct iovec * iov, int iovcnt) {

    FE3C_SANITY_CHECK(output, NULL);
    FE3C_SANITY_CHECK(iov || iovcnt == 0, NULL);

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
    u8 block_buffer[SHA512_BLOCK_SIZE_BYTES];
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
        /* Add the high part of input_length into the aggregate message length. Note that
         * no secret data is used here, so we allow C implementation of 64-bit addition
         * (which compiles to branching code). */
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
            sha512_compress(state, block_buffer, message_schedule, working_variables);

            /* Continue processing full 1024-bit blocks of the current input buffer (read it directly from the
             * input buffer not from the intermediate block buffer) */
            while (input_length >= SHA512_BLOCK_SIZE_BYTES) {

                sha512_compress(state, input, message_schedule, working_variables);
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
        sha512_compress(state, block_buffer, message_schedule, working_variables);
        /* Pad the next block with all zeroes up to the last 16 octets */
        (void) memset(block_buffer, 0, SHA512_BLOCK_SIZE_BYTES - 16);
    }

    /* Do the Merkle-Damgard strengthening and copy the message length into the last 16 octets of the padding */
    store_64(&block_buffer[SHA512_BLOCK_SIZE_BYTES - 16], &message_length_high, 1);
    store_64(&block_buffer[SHA512_BLOCK_SIZE_BYTES - 8], &message_length_low, 1);
    /* Do one final block processing */
    sha512_compress(state, block_buffer, message_schedule, working_variables);
    /* Copy the final state to the output buffer */
    store_64(output, state, SHA512_STATE_WORD_COUNT);

    /* Purge the on-stack state */
    purge_secrets(state, sizeof(state));
    /* Zeroize the scratchpad */
    purge_secrets(message_schedule, sizeof(message_schedule));
    purge_secrets(working_variables, sizeof(working_variables));
    purge_secrets(block_buffer, sizeof(block_buffer));
}

static inline void sha512_compress(u64 * state, const u8 * input_block, u64 * schedule, sha512_working_variables * work) {

    sha512_prepare_message_schedule(schedule, input_block);

    /* Initialize the working variables - see RFC 6234 for the naming convention */
    work->a = state[0];
    work->b = state[1];
    work->c = state[2];
    work->d = state[3];
    work->e = state[4];
    work->f = state[5];
    work->g = state[6];
    work->h = state[7];

    /* Let the compiler allocate registers - we use a convention where
     * aux1-aux4 can be clobbered by the macros, whereas the other
     * registers (which are not specified as inputs to the macros)
     * are preserved */
    u32 T1lo;
    u32 T1hi;
    u32 T2lo;
    u32 T2hi;
    u32 aux1;
    u32 aux2;
    u32 aux3;
    u32 aux4;
    u32 aux5;
    u32 aux6;
    const u64 * rciter = round_constants;

#define update_state(i)                                       \
    _ "l32i %[aux3],       %[state],    " #i " * 8"           \
    _ "l32i %[aux4],       %[state],    " #i " * 8 + 4"       \
    _ "l32i %[aux5],       %[work],     " #i " * 8"           \
    _ "l32i %[aux6],       %[work],     " #i " * 8 + 4"       \
    CONSTANT_TIME_U64_ADD(aux3, aux4, aux3, aux4, aux5, aux6) \
    _ "s32i %[aux3],       %[state],    " #i " * 8"           \
    _ "s32i %[aux4],       %[state],    " #i " * 8 + 4"

    asm volatile(
        /* Set up a hardware loop - note that the loop body is larger than 256
         * bytes so the loop end offset cannot fit into the loop instruction,
         * but gcc works around it, by manually fixing the LBEG, LEND and LCOUNT
         * registers using the wsr (write special register) and rsr (read special
         * register) instructions at a small cost before the first iteration */
        _ "movi.n %[aux1],     %[rounds]"
        _ "loop   %[aux1],     sha512_compress.endloop"
    _ "sha512_compress.startloop:"

        /* Load the working variable e - note that the working variables are
         * stored in native endianness, i.e. little-endian */
        _ "l32i   %[T1lo],     %[work],     4 * 8"
        _ "l32i   %[T1hi],     %[work],     4 * 8 + 4"
        /* Compute BSIG1(e) */
        BSIG1(T2lo, T2hi, T1lo, T1hi)
        /* Load f */
        _ "l32i   %[aux3],     %[work],     5 * 8"
        _ "l32i   %[aux4],     %[work],     5 * 8 + 4"
        /* Load g */
        _ "l32i   %[aux5],     %[work],     6 * 8"
        _ "l32i   %[aux6],     %[work],     6 * 8 + 4"
        /* Compute Ch(e, f, g) */
        Ch(T1lo, T1hi, aux3, aux4, aux5, aux6)

        /* Add BSIG1(e) to T1 */
        CONSTANT_TIME_U64_ADD(T1lo, T1hi, T1lo, T1hi, T2lo, T2hi)

        /* Add h to T1 */
        _ "l32i   %[aux3],     %[work],     7 * 8"
        _ "l32i   %[aux4],     %[work],     7 * 8 + 4"
        CONSTANT_TIME_U64_ADD(T1lo, T1hi, T1lo, T1hi, aux3, aux4)

        /* Add the round constant to T1 */
        _ "l32i   %[aux3],     %[rc],       0"
        _ "l32i   %[aux4],     %[rc],       4"
        CONSTANT_TIME_U64_ADD(T1lo, T1hi, T1lo, T1hi, aux3, aux4)

        /* Add the schedule word to T1 */
        _ "l32i   %[aux3],     %[schedule], 0"
        _ "l32i   %[aux4],     %[schedule], 4"
        CONSTANT_TIME_U64_ADD(T1lo, T1hi, T1lo, T1hi, aux3, aux4)

        /* Load a */
        _ "l32i   %[T2lo],     %[work],     0 * 8"
        _ "l32i   %[T2hi],     %[work],     0 * 8 + 4"
        /* Compute BSIG0(a) */
        BSIG0(aux5, aux6, T2lo, T2hi)
        /* The h working variable will get overwritten anyway,
         * so reuse it for temporary storage (TODO: try avoiding
         * this memory access) */
        _ "s32i   %[aux5],     %[work],     7 * 8"
        _ "s32i   %[aux6],     %[work],     7 * 8 + 4"
        /* Load b */
        _ "l32i   %[aux3],     %[work],     1 * 8"
        _ "l32i   %[aux4],     %[work],     1 * 8 + 4"
        /* Load c */
        _ "l32i   %[aux5],     %[work],     2 * 8"
        _ "l32i   %[aux6],     %[work],     2 * 8 + 4"

        /* Compute Maj(a, b, c) */
        Maj(T2lo, T2hi, aux3, aux4, aux5, aux6)

        /* Add BSIG0(a) to T2 */
        _ "l32i   %[aux3],     %[work],     7 * 8"
        _ "l32i   %[aux4],     %[work],     7 * 8 + 4"
        CONSTANT_TIME_U64_ADD(T2lo, T2hi, T2lo, T2hi, aux3, aux4)

        /* Set h to g */
        _ "l32i   %[aux1],     %[work],     6 * 8"
        _ "l32i   %[aux2],     %[work],     6 * 8 + 4"
        _ "s32i   %[aux1],     %[work],     7 * 8"
        _ "s32i   %[aux2],     %[work],     7 * 8 + 4"

        /* Set g to f */
        _ "l32i   %[aux1],     %[work],     5 * 8"
        _ "l32i   %[aux2],     %[work],     5 * 8 + 4"
        _ "s32i   %[aux1],     %[work],     6 * 8"
        _ "s32i   %[aux2],     %[work],     6 * 8 + 4"

        /* Set f to e */
        _ "l32i   %[aux1],     %[work],     4 * 8"
        _ "l32i   %[aux2],     %[work],     4 * 8 + 4"
        _ "s32i   %[aux1],     %[work],     5 * 8"
        _ "s32i   %[aux2],     %[work],     5 * 8 + 4"

        /* Set e to d + T1 */
        _ "l32i   %[aux1],     %[work],     3 * 8"
        _ "l32i   %[aux2],     %[work],     3 * 8 + 4"
        CONSTANT_TIME_U64_ADD(aux3, aux4, aux1, aux2, T1lo, T1hi)
        _ "s32i   %[aux3],     %[work],     4 * 8"
        _ "s32i   %[aux4],     %[work],     4 * 8 + 4"

        /* Set d to c */
        _ "l32i   %[aux1],     %[work],     2 * 8"
        _ "l32i   %[aux2],     %[work],     2 * 8 + 4"
        _ "s32i   %[aux1],     %[work],     3 * 8"
        _ "s32i   %[aux2],     %[work],     3 * 8 + 4"

        /* Set c to b */
        _ "l32i   %[aux1],     %[work],     1 * 8"
        _ "l32i   %[aux2],     %[work],     1 * 8 + 4"
        _ "s32i   %[aux1],     %[work],     2 * 8"
        _ "s32i   %[aux2],     %[work],     2 * 8 + 4"

        /* Set b to a */
        _ "l32i   %[aux1],     %[work],     0 * 8"
        _ "l32i   %[aux2],     %[work],     0 * 8 + 4"
        _ "s32i   %[aux1],     %[work],     1 * 8"
        _ "s32i   %[aux2],     %[work],     1 * 8 + 4"

        /* Set a to T1 + T2 */
        CONSTANT_TIME_U64_ADD(aux3, aux4, T1lo, T1hi, T2lo, T2hi)
        _ "s32i   %[aux3],     %[work],     0 * 8"
        _ "s32i   %[aux4],     %[work],     0 * 8 + 4"

        /* Move on to the next round constant */
        _ "addi.n %[rc],       %[rc],       8"
        /* Move on to the next schedule word */
        _ "addi.n %[schedule], %[schedule], 8"

    _ "sha512_compress.endloop:"

        /* Update the state */
        update_state(0)
        update_state(1)
        update_state(2)
        update_state(3)
        update_state(4)
        update_state(5)
        update_state(6)
        update_state(7)

        : [rc]              "+&r" (rciter),
          [schedule]        "+&r" (schedule),
          [T1lo]            "=&r" (T1lo),
          [T1hi]            "=&r" (T1hi),
          [T2lo]            "=&r" (T2lo),
          [T2hi]            "=&r" (T2hi),
          [aux1]            "=&r" (aux1),
          [aux2]            "=&r" (aux2),
          [aux3]            "=&r" (aux3),
          [aux4]            "=&r" (aux4),
          [aux5]            "=&r" (aux5),
          [aux6]            "=&r" (aux6)
        : [state]           "r"   (state),
          [work]            "r"   (work),
          [rounds]          "i"   (SHA512_ROUNDS_COUNT)
        : "memory"
    );
}

static inline void sha512_prepare_message_schedule(u64 * schedule, const u8 * input) {

    /* Write the current input block into the first 16 words of the message schedule */
    load_64(schedule, input, SHA512_BLOCK_SIZE_BYTES / 8);

    /* Keep the schedule iterator 16 words (64-bit words) behind the current "loop index"
     * since we cannot do negative offset addressing */
    u64 * schediter = &schedule[SHA512_BLOCK_SIZE_BYTES / 8 - 16];

    /* Let the compiler allocate registers - we use a convention where
     * aux1-aux4 can be clobbered by the macros, whereas the other
     * registers (which are not specified as inputs to the macros)
     * are preserved */
    u32 aux1;
    u32 aux2;
    u32 aux3;
    u32 aux4;
    u32 aux5;
    u32 aux6;
    u32 outlo;
    u32 outhi;
    u32 inlo;
    u32 inhi;

    /* Fill in the rest of the message schedule */
    asm volatile(

        /* Set up a hardware loop */
        _ "movi.n %[aux1],      %[iterations]"
        _ "loop   %[aux1],      sha512_prepare_message_schedule.endloop"
    _ "sha512_prepare_message_schedule.startloop:"

        /* Load schedule word two words behind the current target */
        _ "l32i   %[inlo],      %[schediter], 14 * 8"
        _ "l32i   %[inhi],      %[schediter], 14 * 8 + 4"
        /* Compute SSIG1(schedule[t - 2]) */
        SSIG1(outlo, outhi, inlo, inhi)

        /* Load schedule word seven words behind the current target */
        _ "l32i   %[inlo],      %[schediter], 9 * 8"
        _ "l32i   %[inhi],      %[schediter], 9 * 8 + 4"
        /* Add schedule[t - 7] to the current output */
        CONSTANT_TIME_U64_ADD(outlo, outhi, outlo, outhi, inlo, inhi)

        /* Load schedule word fifteen words behind the current target */
        _ "l32i   %[inlo],      %[schediter], 1 * 8"
        _ "l32i   %[inhi],      %[schediter], 1 * 8 + 4"
        /* Compute SSIG0(schedule[t - 15]) */
        SSIG0(aux5, aux6, inlo, inhi)
        /* Add SSIG0(schedule[t - 15]) to the current output */
        CONSTANT_TIME_U64_ADD(outlo, outhi, outlo, outhi, aux5, aux6)

        /* Load schedule word sixteen words behind the current target */
        _ "l32i   %[inlo],      %[schediter], 0 * 8"
        _ "l32i   %[inhi],      %[schediter], 0 * 8 + 4"
        /* Add schedule[t - 16] to the current output */
        CONSTANT_TIME_U64_ADD(outlo, outhi, outlo, outhi, inlo, inhi)

        /* Write back to the target schedule word */
        _ "s32i   %[outlo],     %[schediter], 16 * 8"
        _ "s32i   %[outhi],     %[schediter], 16 * 8 + 4"

        /* Advance the schedule iterator */
        _ "addi.n %[schediter], %[schediter], 8"

    _ "sha512_prepare_message_schedule.endloop:"

        : [schediter]  "+r" (schediter),
          [aux1]       "=r" (aux1),
          [aux2]       "=r" (aux2),
          [aux3]       "=r" (aux3),
          [aux4]       "=r" (aux4),
          [aux5]       "=r" (aux5),
          [aux6]       "=r" (aux6),
          [outlo]      "=r" (outlo),
          [outhi]      "=r" (outhi),
          [inlo]       "=r" (inlo),
          [inhi]       "=r" (inhi)
        : [iterations] "i"  (SHA512_MESSAGE_SCHEDULE_WORD_COUNT - SHA512_BLOCK_SIZE_BYTES / 8)
        : "memory"
    );
}

static inline void store_64(u8 * dst, const u64 * src, size_t wordcount) {

    for (size_t i = 0; i < wordcount; i++) {

        *(u64 *) &dst[8 * i] = __builtin_bswap64(src[i]);
    }
}

static inline void load_64(u64 * dst, const u8 * src, size_t wordcount) {

    for (size_t i = 0; i < wordcount; i++) {

        dst[i] = __builtin_bswap64(*(u64 *) &src[8 * i]);
    }
}
