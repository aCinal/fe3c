#include <hash_functions/hash_functions.h>
#include <port/hash_functions/hash_function_sha512_esp32.h>

void hash_sha512(u8 * output, const struct iovec * iov, int iovcnt) {

#if FE3C_USE_SHA512_HARDWARE
    sha512_impl impl_select = sha512_try_lock_hw();
    if (impl_select == sha512_hw_acceleration) {

        sha512_impl_hw(output, iov, iovcnt);
        sha512_release_hw();

    } else {

        sha512_impl_sw(output, iov, iovcnt);
    }
#else
    sha512_impl_sw(output, iov, iovcnt);
#endif
}
