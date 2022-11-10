
#ifndef __FE3C_CURVES_CURVES_H
#define __FE3C_CURVES_CURVES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fe3c/eddsa.h>
#include <hash_functions/hash_functions.h>
#include <scalars/scalars.h>
#include <points/points.h>

typedef struct curve {
    hash hash_function;
    group_ops * gops;
    scalar_ops * sops;
    void (* prune_buffer)(u8 * buffer);
    const char * dom_string;
    size_t dom_string_length;
    int b_in_bytes;
} curve;

extern const curve * curves[EDDSA_NUMBER_OF_SUPPORTED_CURVES];

#if FE3C_SUPPORT_CURVE_ED25519
void ed25519_init_curve(void);
#endif /* FE3C_SUPPORT_CURVE_ED25519 */

#if FE3C_SUPPORT_CURVE_ED448
void ed448_init_curve(void);
#endif /* FE3C_SUPPORT_CURVE_ED448 */

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_CURVES_CURVES_H */
