
#ifndef __FE3C_UTILS_UTILS_H
#define __FE3C_UTILS_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fe3c/global_defs.h>
#include <string.h>  /* Include memcpy() from libc via this header to allow overriding it if need be */

/* Provide constant-time and otherwise secure implementations of common libc functions */

/**
 * @brief Do constant-time check if two memory buffers contain the same data
 * @param b1 First buffer
 * @param b2 Second buffer
 * @param n Extent of the check (typically size of the buffers)
 * @return Nonzero value if the buffers are identical, zero otherwise
 */
int memidentical(const void * b1, const void * b2, size_t n);

/**
 * @brief Clear secret data in memory so that erroneous accesses to the same stack space later do not reveal anything
 * @param secrets Buffer to be cleared
 * @param size Size of the buffer
 */
void purge_secrets(void * secrets, size_t size);

#if FE3C_ENABLE_SANITY_CHECKS
    #include <stdio.h>
    #include <stdlib.h>
    static inline void __sanity_check_failed(const char * check, const char * file, int line) {

        fprintf(stderr, "%s in %s at line %d: [ %s ]\n", \
            __FUNCTION__, file, line, check);
        abort();
    }
    #define FE3C_SANITY_CHECK(x)               if (!(x)) { __sanity_check_failed(#x, __FILE__, __LINE__); }
    #define FE3C_COMPILE_TIME_SANITY_CHECK(x)  _Static_assert(x)
#else
    #define FE3C_SANITY_CHECK(x)
    #define FE3C_COMPILE_TIME_SANITY_CHECK(x)
#endif /* FE3C_ENABLE_SANITY_CHECKS */

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_UTILS_UTILS_H */
