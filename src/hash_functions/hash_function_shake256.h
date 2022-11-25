
#ifndef __FE3C_HASH_FUNCTIONS_HASH_FUNCTION_SHAKE256_H
#define __FE3C_HASH_FUNCTIONS_HASH_FUNCTION_SHAKE256_H

#include <global_defs.h>
#include <utils/utils.h>

#define SHAKE256_CAPACITY_BITS      512
#define SHAKE256_BITRATE_BITS       1088
#define SHAKE256_CAPACITY_BYTES     (SHAKE256_CAPACITY_BITS / 8)
#define SHAKE256_BITRATE_BYTES      (SHAKE256_BITRATE_BITS / 8)
#define SHAKE256_STATE_SIZE_BYTES   (SHAKE256_CAPACITY_BYTES + SHAKE256_BITRATE_BYTES)
#define SHAKE256_BLOCK_SIZE_BYTES   SHAKE256_BITRATE_BYTES
#define SHAKE256_OUTPUT_SIZE_BYTES  114
#define SHAKE256_ROUNDS_COUNT       24

/**
 * @brief Rotate a 64-bit word left by a given number of bits
 * @warning The parameter order is different than in SHA512 where the number of bits comes first, and
 *          the word to rotate comes second
 */
#define ROT(word, bits)  ( ( (word) << (bits) ) | ( (word) >> (64 - (bits))) )

/**
 * @brief Round constants of the Keccak-f transformation
 */
static const u64 round_constants[] = {
    0x0000000000000001ULL,
    0x0000000000008082ULL,
    0x800000000000808AULL,
    0x8000000080008000ULL,
    0x000000000000808BULL,
    0x0000000080000001ULL,
    0x8000000080008081ULL,
    0x8000000000008009ULL,
    0x000000000000008AULL,
    0x0000000000000088ULL,
    0x0000000080008009ULL,
    0x000000008000000AULL,
    0x000000008000808BULL,
    0x800000000000008BULL,
    0x8000000000008089ULL,
    0x8000000000008003ULL,
    0x8000000000008002ULL,
    0x8000000000000080ULL,
    0x000000000000800AULL,
    0x800000008000000AULL,
    0x8000000080008081ULL,
    0x8000000000008080ULL,
    0x0000000080000001ULL,
    0x8000000080008008ULL
};

/**
 * @brief Rotation offsets in the rho phase of the Keccak-f transformation
 * @note The indexing is inverted relative to the notation used by FIPS PUB 202
 */
static const int rotation_offsets[5][5] = {
    {  0,  1, 62, 28, 27 },
    { 36, 44,  6, 55, 20 },
    {  3, 10, 43, 25, 39 },
    { 41, 45, 15, 21,  8 },
    { 18,  2, 61, 56, 14 }
};

/**
 * @brief SHAKE256 state
 */
typedef union shake256_state {
    struct {
        /**
         * @brief Outer state used for absorbing the input buffer
         */
        u64 outer_state[SHAKE256_BITRATE_BYTES / sizeof(u64)];
        /**
         * @brief Inner state not accessed directly
         */
        u64 inner_state[SHAKE256_CAPACITY_BYTES / sizeof(u64)];
    };
    /**
     * @brief The permutation state array used by the Keccak-f transformation
     * @note The indexing is inverted relative to the notation used by FIPS PUB 202:
     *       "SHA-3 Standard: Permutation-Based Hash and Extendable-Output Functions".
     *       This is because FIPS PUB 202 treats the x-coordinate as changing more
     *       often than the y-coordinate which is not the case for C-style 2D arrays.
     */
    u64 permutation_state_array[5][5];
} shake256_state;

/* Let the compiler allocate the array(s) according to their initializer(s) and later validate the size(s) */
FE3C_COMPILE_TIME_SANITY_CHECK(sizeof(shake256_state) == SHAKE256_STATE_SIZE_BYTES);
FE3C_COMPILE_TIME_SANITY_CHECK(sizeof(round_constants) / sizeof(round_constants[0]) == SHAKE256_ROUNDS_COUNT);
/* The implementation assumes all needed bytes can be squeezed out in a single go, which requires they are
 * fewer than the size of the state */
FE3C_COMPILE_TIME_SANITY_CHECK(SHAKE256_OUTPUT_SIZE_BYTES < SHAKE256_STATE_SIZE_BYTES);

#endif /* __FE3C_HASH_FUNCTIONS_HASH_FUNCTION_SHAKE256_H */
