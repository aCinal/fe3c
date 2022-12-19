
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
    __attribute__((warn_unused_result))
    int   (* points_equal)        (const point * p, const point * q);             /**< Check if two points are equal */
    void  (* encode)              (u8 * buf, const point * p);                    /**< Encode the point */
    __attribute__((warn_unused_result))
    int   (* decode)              (point * p, const u8 * buf);                    /**< Try decoding the point, set to the neutral element (0, 1) if decoding fails */
    void  (* points_add)          (point * r, const point * p, const point * q);  /**< Add point p to the point q */
    void  (* scalar_multiply)     (point * r, const point * p, const u8 * s);     /**< Multiply point p by the scalar s */
    void  (* multiply_basepoint)  (point * r, const u8 * s);                      /**< Multiply the group generator by the scalar s */
} group_ops;

#if FE3C_SUPPORT_CURVE_ED25519
extern group_ops ed25519_group_ops;
#endif /* FE3C_SUPPORT_CURVE_ED25519 */

#if FE3C_SUPPORT_CURVE_ED448
extern group_ops ed448_group_ops;
#endif /* FE3C_SUPPORT_CURVE_ED448 */

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_POINTS_POINTS_H */
