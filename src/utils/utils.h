
#ifndef __FE3C_UTILS_UTILS_H
#define __FE3C_UTILS_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <global_defs.h>
#include <string.h>  /* Include memcpy() from libc via this header to allow overriding it if need be */

/* Provide constant-time and otherwise secure implementations of common libc functions */

/**
 * @brief Clear secret data in memory so that erroneous accesses to the same stack space later do not reveal anything
 * @param secrets Buffer to be cleared
 * @param size Size of the buffer
 */
static inline void purge_secrets(void * secrets, size_t size) {

    /* TODO: Use memset_s when available (when using C11) */
    volatile u8 * p = (volatile u8 *) secrets;
    while (size -- > 0) {
        *p++ = 0;
    }
}

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
