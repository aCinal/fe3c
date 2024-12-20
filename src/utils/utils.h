
#ifndef __FE3C_UTILS_UTILS_H
#define __FE3C_UTILS_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <global_defs.h>
#include <string.h>  /* Include memcpy() from libc via this header to allow overriding it if need be */

/** Access a specific bit in an array (assume little-endian byte order) */
#define array_bit(arr, b)      ( (arr[(b) >> 3] >> ((b) & 0x7)) & 1 )
/** Get array length at compile time */
#define static_array_len(arr)  ( sizeof(arr) / sizeof((arr)[0]) )
/** Swap two pointers  */
#define swap_pointers(x, y)  { void *__aux; __aux = x; x = y; y = __aux; }

/**
 * @brief Test two bytes for equality in a constant-time manner
 * @param x First input byte
 * @param y Second input byte
 * @return 1 if x == y, 0 otherwise
 */
static inline int byte_equal(u8 x, u8 y)
{
    u8 ret = x ^ y;
    ret |= (ret >> 4);
    ret |= (ret >> 2);
    ret |= (ret >> 1);
    return (ret ^ 1) & 1;
}

/**
 * @brief Count leading zeroes in a non-zero 32-bit word
 * @param x A non-zero 32-bit word
 * @return Number of leading zeroes in x
 * @warning The function assumes x != 0x00000000
 * @note From "Hacker's Delight" by Henry S. Warren
 */
static inline int u32_nlz(u32 x)
{
    /* Seems more portable than __builtin_clz which promotes the input to an integer */
    int nlz = 1;
    /* Binary-search through the byte */
    if ((x >> 16) == 0) { nlz += 16; x <<= 16; }
    if ((x >> 24) == 0) { nlz += 8;  x <<= 8;  }
    if ((x >> 28) == 0) { nlz += 4;  x <<= 4;  }
    if ((x >> 30) == 0) { nlz += 2;  x <<= 2;  }
    nlz -= (x >> 31);
    return nlz;
}

#if FE3C_SKIP_ZEROIZATION
    #define purge_secrets(...)
#else
    /**
     * @brief Clear secret data in memory so that erroneous accesses to the same stack space later do not reveal anything
     * @param secrets Buffer to be cleared
     * @param size Size of the buffer
     */
    static inline void purge_secrets(void *secrets, size_t size)
    {

        volatile u8 *p = (volatile u8 *) secrets;
        while (size -- > 0)
            *p++ = 0;
    }
#endif /* FE3C_SKIP_ZEROIZATION */

#if FE3C_ENABLE_SANITY_CHECKS
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    __attribute__((format(printf, 4, 5)))
    static inline void __sanity_check_failed(const char *check, const char *file, int line, const char *format, ...)
    {
        fprintf(stderr, "%s in %s at line %d: [ %s ]\n", \
            __FUNCTION__, file, line, check);
        if (format) {
            va_list ap;
            va_start(ap, format);
            vfprintf(stderr, format, ap);
            va_end(ap);
            fprintf(stderr, "\n");
        }
        abort();
    }
    #define FE3C_SANITY_CHECK(check, message, ...)  if (!(check)) { __sanity_check_failed(#check, __FILE__, __LINE__, message, ##__VA_ARGS__); }
    #define FE3C_COMPILE_TIME_SANITY_CHECK(check)   _Static_assert(check)
#else
    #define FE3C_SANITY_CHECK(check, message, ...)
    #define FE3C_COMPILE_TIME_SANITY_CHECK(check)
#endif /* FE3C_ENABLE_SANITY_CHECKS */

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_UTILS_UTILS_H */
