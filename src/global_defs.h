
#ifndef __FE3C_GLOBAL_DEFS_H
#define __FE3C_GLOBAL_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

#if defined(FE3C_64BIT)
    #if defined(__SIZEOF_INT128__)
        typedef unsigned __int128 u128;
        typedef __int128 i128;
    #else
        typedef int i128 __attribute__((mode(TI)));
        typedef unsigned u128 __attribute__((mode(TI)));
    #endif
#endif /* defined(FE3C_64BIT) */

/* Define POSIX-like struct iovec, but use const void pointer as we
 * only want to support gathering and not scattering */
struct iovec {
    const void * iov_base;
    size_t iov_len;
};

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_GLOBAL_DEFS_H */
