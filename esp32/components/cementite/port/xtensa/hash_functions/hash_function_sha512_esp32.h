
#ifndef __FE3C_PORT_XTENSA_HASH_FUNCTIONS_HASH_FUNCTION_SHA512_ESP32_H
#define __FE3C_PORT_XTENSA_HASH_FUNCTIONS_HASH_FUNCTION_SHA512_ESP32_H

#include <hash_functions/hash_functions.h>

#if FE3C_USE_SHA512_HARDWARE
/* Note that sha512_impl_hw subverts the convention used in our other APIs where 1 indicates success, and 0 indicates failure.
 * Here we use the good old POSIX convention, where 0 stands for no errors, and anything else is an error code. */
int sha512_impl_hw(u8 * output, const struct iovec * iov, int iovcnt);

typedef enum sha512_impl {
    sha512_hw_acceleration = 0,
    sha512_sw_fallback
} sha512_impl;

sha512_impl sha512_try_lock_hw(void);
void sha512_release_hw(void);
#endif /* FE3C_USE_SHA512_HARDWARE */

#endif /* __FE3C_PORT_XTENSA_HASH_FUNCTIONS_HASH_FUNCTION_SHA512_ESP32_H */
