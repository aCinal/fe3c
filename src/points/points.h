
#ifndef __FE3C_POINTS_POINTS_H
#define __FE3C_POINTS_POINTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <field_elements/field_elements.h>

typedef struct point {
    fe X;
    fe Y;
    fe Z;
#if FE3C_SUPPORT_CURVE_ED25519
    fe T;
#endif /* FE3C_SUPPORT_CURVE_ED25519 */
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
