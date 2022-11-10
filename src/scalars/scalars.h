
#ifndef __FE3C_SCALARS_SCALARS_H
#define __FE3C_SCALARS_SCALARS_H

#include <fe3c/global_defs.h>

typedef struct scalar_ops {

    /**
     * @brief Reduce in place a scalar modulo the main subgroup order L (notation from RFC 8032)
     * @param[in,out] s Scalar to be reduced
     */
    void  (* reduce)        (u8 * s);

    /**
     * @brief Check if a response scalar provided as part of the signature is in canonical form (i.e. less than the subgroup order)
     * @param s Scalar to be checked
     * @return 1 if s is in canonical form, 0 otherwise
     */
    __attribute__((warn_unused_result))
    int   (* is_canonical)  (const u8 * s);

    /**
     * @brief Multiply two scalars a and b and add a third one
     * @param[in] a First factor
     * @param[in] b Second factor
     * @param[in] c Offset
     * @param[out] r Buffer for the result, i.e. a*b + c
     */
    void  (* muladd)        (u8 * r, const u8 * a, const u8 * b, const u8 * c);

} scalar_ops;

#if FE3C_SUPPORT_CURVE_ED25519
extern scalar_ops ed25519_scalar_ops;
#endif /* FE3C_SUPPORT_CURVE_ED25519 */

#if FE3C_SUPPORT_CURVE_ED448
extern scalar_ops ed448_scalar_ops;
#endif /* FE3C_SUPPORT_CURVE_ED448 */

#endif /* __FE3C_SCALARS_SCALARS_H */
