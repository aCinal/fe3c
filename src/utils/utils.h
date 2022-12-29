
#ifndef __FE3C_UTILS_UTILS_H
#define __FE3C_UTILS_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <global_defs.h>
#include <string.h>  /* Include memcpy() from libc via this header to allow overriding it if need be */

/* Provide constant-time and otherwise secure implementations of common libc functions */

#if FE3C_OPTIMIZATION_SKIP_ZEROIZATION
#define purge_secrets(...)
#else
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
#endif /* FE3C_OPTIMIZATION_SKIP_ZEROIZATION */

#if FE3C_ENABLE_SANITY_CHECKS
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    __attribute__((format(printf, 4, 5)))
    static inline void __sanity_check_failed(const char * check, const char * file, int line, const char * format, ...) {

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
