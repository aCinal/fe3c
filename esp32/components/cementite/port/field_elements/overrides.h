
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

/* ------------------------------------------------------------------------------------------------------------------------ */

/* Use 32 14-bit limbs for fe448, so allocate 16 words (coincidentally the same number as for refcode) */
#define ED448_FE_LIMB_COUNT 16

#define FE448_STR "0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx"
#define FE448_TO_STR(x) \
    x[ 0], x[ 1], x[ 2], x[ 3], \
    x[ 4], x[ 5], x[ 6], x[ 7], \
    x[ 8], x[ 9], x[10], x[11], \
    x[12], x[13], x[14], x[15]

#define ED448_BASEPOINT_X  { 0x1c33005e, 0x9aa02bc, 0x24e0e26, 0x203818b0, 0x1445033b, 0x3dca2b66, 0x104a2e1a, 0x28f43a46, 0x29b72324, 0x3c5367e, 0x11953047, 0xdb629e1, 0x169822bf, 0x3b4821d1, 0x19af2d0d, 0x13c6170c }
#define ED448_BASEPOINT_Y  { 0x8c33a14, 0x21e15bf, 0x1f222d98, 0x4cb04ed, 0x27133dbd, 0x73927c3, 0x1ceb13ff, 0x1680c2d, 0x1de21c1e, 0x28e60408, 0x29cd336c, 0x31d824be, 0xdd16c9, 0x9220762, 0x5ba36bc, 0x1a4f3467 }
#define ED448_BASEPOINT_Z  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

#define ED448_ISOGENY_BASEPOINT_X { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x20000000, 0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x1fff3fff }
#define ED448_ISOGENY_BASEPOINT_Y { 0x3f750a64, 0x181e1b4d, 0x11c221c8, 0x30723ab, 0x2394199b, 0x113533f4, 0x10501818, 0x14111271, 0xf54242c, 0x131d0c39, 0x190d3050, 0x9020052, 0x4c138ca, 0x3523106c, 0x13c10286, 0x21420de1 }
#define ED448_ISOGENY_BASEPOINT_Z { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
#define ED448_ISOGENY_BASEPOINT_T { 0x30660105, 0x38f2016d, 0xfd32093, 0x18813183, 0xd081403, 0x1377298e, 0x14670985, 0x28c61b57, 0x18e02384, 0x1e523e95, 0x21b6260f, 0x38771c2b, 0x1a0a29ff, 0x143823d5, 0x5cf06a4, 0x1b4d269e }

#endif /* __PORT_FIELD_ELEMENTS_OVERRIDES_H */
