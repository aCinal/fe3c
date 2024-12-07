
#ifndef __FE3C_POINTS_POINTS_H
#define __FE3C_POINTS_POINTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <field_elements/field_elements.h>

#if FE3C_SUPPORT_CURVE_ED25519
typedef struct point_ed25519 {
    fe25519 X;
    fe25519 Y;
    fe25519 Z;
    fe25519 T;
} point_ed25519;
#endif /* FE3C_SUPPORT_CURVE_ED25519 */

#if FE3C_SUPPORT_CURVE_ED448
typedef struct point_ed448 {
    fe448 X;
    fe448 Y;
    fe448 Z;
    fe448 T;
} point_ed448;
#endif /* FE3C_SUPPORT_CURVE_ED448 */

typedef union point {

#if FE3C_SUPPORT_CURVE_ED25519
    point_ed25519 ed25519;
#endif /* FE3C_SUPPORT_CURVE_ED25519 */

#if FE3C_SUPPORT_CURVE_ED448
    point_ed448 ed448;
#endif /* FE3C_SUPPORT_CURVE_ED448 */

} point;

typedef struct group_ops {

    /**
     * @brief Check if the (cofactored) Schnorr group equation is satisfied, i.e., if 2^c*(S*B - h*A) == 2^C*R
     * @param public_key Public key point (A in the Schnorr equation)
     * @param commitment Commitment (R)
     * @param challenge Challenge scalar (h)
     * @param response Response scalar (S)
     * @warning The function does not run in constant-time and can freely clobber the elliptic curve points passed as parameters
     */
    __attribute__((warn_unused_result))
    int (*check_group_equation)(point *public_key, point *commitment, const u8 *challenge, const u8 *response);

    /**
     * @brief Encode an Edwards curve point
     * @param[out] buf Resulting point encoding
     * @param[in] p Edwards curve point
     */
    void (*encode)(u8 *buf, const point *p);

    /**
     * @brief Decode an Edwards curve point
     * @param[out] p Decoded Edwards curve point
     * @param[in] buf Point encoding
     * @return 1 if decoding was successful, 0 otherwise
     */
    __attribute__((warn_unused_result))
    int (*decode)(point *p, const u8 *buf);

    /**
     * @brief Multiply the group generator by a scalar
     * @param[out] r Resulting Edwards curve point
     * @param[in] s Secret scalar
     */
    void (*multiply_basepoint)(point *r, const u8 *s);

    /**
     * @brief Negate in place a point on the curve
     * @param[in, out] p Edwards curve point
     */
    void (*point_negate)(point *p);

} group_ops;

#if FE3C_SUPPORT_CURVE_ED25519
extern const group_ops ed25519_group_ops;
#endif /* FE3C_SUPPORT_CURVE_ED25519 */

#if FE3C_SUPPORT_CURVE_ED448
extern const group_ops ed448_group_ops;
#endif /* FE3C_SUPPORT_CURVE_ED448 */

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_POINTS_POINTS_H */
