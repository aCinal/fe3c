
#ifndef __FE3C_HASH_FUNCTIONS_HASH_FUNCTIONS_H
#define __FE3C_HASH_FUNCTIONS_HASH_FUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <global_defs.h>

#if FE3C_SUPPORT_CURVE_ED25519
void hash_sha512(u8 *output, const struct iovec *iov, int iovcnt);
#endif /* FE3C_SUPPORT_CURVE_ED25519 */

#if FE3C_SUPPORT_CURVE_ED448
void hash_shake256(u8 *output, const struct iovec *iov, int iovcnt);
#endif /* FE3C_SUPPORT_CURVE_ED448 */

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_HASH_FUNCTIONS_HASH_FUNCTIONS_H */
