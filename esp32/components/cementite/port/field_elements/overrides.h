
#ifndef __PORT_FIELD_ELEMENTS_OVERRIDES_H
#define __PORT_FIELD_ELEMENTS_OVERRIDES_H

/* Override constants and macros in field_elements.h, field_elements_ed25519.h and field_elements_ed448.h */

/* Use 17 15-bit limbs for fe25519, so allocate 9 words */
#define ED25519_FE_LIMB_COUNT 9

#define FE25519_STR "0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx"
#define FE25519_TO_STR(x) \
    x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7], x[8]

#define ED25519_BASEPOINT_X  { 0x1e4b551a, 0x4ab13582, 0x24b47b2c, 0x16635825, 0x2db85c69, 0x271147f7, 0x4a7f6c0a, 0x1b69735b, 0x2169 }
#define ED25519_BASEPOINT_Y  { 0x4ccc6658, 0x33331999, 0x4ccc6666, 0x33331999, 0x4ccc6666, 0x33331999, 0x4ccc6666, 0x33331999, 0x6666 }
#define ED25519_BASEPOINT_Z  { 1, 0, 0, 0, 0, 0, 0, 0, 0 }
#define ED25519_BASEPOINT_T  { 0x4b6f5da3, 0x6ef42ace, 0x6a2a2f56, 0x784f601d, 0x57c67d20, 0x52743992, 0x6ecc566e, 0x2f8775e2, 0x6787 }

#endif /* __PORT_FIELD_ELEMENTS_OVERRIDES_H */
