
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

#define ED25519_BASEPOINT_TIMES_2_128_X  { 0x416f6824, 0x64021eb9, 0x5ce43e5f, 0x73427270, 0x1c532098, 0x69cc1385, 0x3e654952, 0x57ff4f11, 0x4c27 }
#define ED25519_BASEPOINT_TIMES_2_128_Y  { 0x17eb266b, 0x5dd57445, 0x349849e5, 0x685f0794, 0x5383a90, 0x8f5589b, 0x390f595f, 0x4cf3149b, 0x5f2c }
#define ED25519_BASEPOINT_TIMES_2_128_Z  { 1, 0, 0, 0, 0, 0, 0, 0, 0 }
#define ED25519_BASEPOINT_TIMES_2_128_T  { 0xde2338a, 0x4f394725, 0x4c755a9e, 0x3c322099, 0x24966a14, 0xab0001e, 0xc057d4e, 0x327e42fd, 0x5a1 }

#define ED25519_BASEPOINT_TIMES_2_128_PLUS_1_X  { 0x5019169a, 0x5f1774a5, 0x3d974e6f, 0x7376020c, 0x6c053b0, 0x1e0d7a33, 0x1c9178ab, 0x72aa4ae2, 0x6e64 }
#define ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Y  { 0x775f509b, 0x29a92991, 0x58aa2102, 0x1c885627, 0x70a24536, 0x6fe44ce6, 0x6b1766a9, 0x28876932, 0x383c }
#define ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Z  { 1, 0, 0, 0, 0, 0, 0, 0, 0 }
#define ED25519_BASEPOINT_TIMES_2_128_PLUS_1_T  { 0x28d03a9, 0x9c542f2, 0x771560a, 0x7f291e1e, 0x726b1143, 0x797562a6, 0x1b1f157b, 0x30fc1441, 0x475c }

/* ------------------------------------------------------------------------------------------------------------------------ */

/* Use 32 14-bit limbs for fe448, so allocate 16 words (coincidentally the same number as for refcode) */
#define ED448_FE_LIMB_COUNT 16

#define FE448_STR "0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx"
#define FE448_TO_STR(x) \
    x[ 0], x[ 1], x[ 2], x[ 3], \
    x[ 4], x[ 5], x[ 6], x[ 7], \
    x[ 8], x[ 9], x[10], x[11], \
    x[12], x[13], x[14], x[15]

#define ED448_BASEPOINT_X { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x20000000, 0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x1fff3fff }
#define ED448_BASEPOINT_Y { 0x3f750a64, 0x181e1b4d, 0x11c221c8, 0x30723ab, 0x2394199b, 0x113533f4, 0x10501818, 0x14111271, 0xf54242c, 0x131d0c39, 0x190d3050, 0x9020052, 0x4c138ca, 0x3523106c, 0x13c10286, 0x21420de1 }
#define ED448_BASEPOINT_Z { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
#define ED448_BASEPOINT_T { 0x30660105, 0x38f2016d, 0xfd32093, 0x18813183, 0xd081403, 0x1377298e, 0x14670985, 0x28c61b57, 0x18e02384, 0x1e523e95, 0x21b6260f, 0x38771c2b, 0x1a0a29ff, 0x143823d5, 0x5cf06a4, 0x1b4d269e }

#define ED448_BASEPOINT_TIMES_2_225_X  { 0x3ed932fb, 0x32ff3257, 0x12dd2d0f, 0x1e9f3a27, 0x371038c3, 0x2a390afe, 0x25ac29fa, 0xa853d02, 0x8a20bca, 0x1bac28cf, 0x22310255, 0x186a1f62, 0xaa50bcf, 0xa95059d, 0x226a3e2d, 0x29d379d }
#define ED448_BASEPOINT_TIMES_2_225_Y  { 0x271517f9, 0x27231696, 0x12573462, 0x3210ccc, 0x23082423, 0xc5b2b91, 0x17af22d9, 0xf9f0d2e, 0x12ab3a9f, 0x3293243f, 0x3e091196, 0x1ff4244c, 0x36043995, 0x231b35c9, 0x1e0e3008, 0x3137109e }
#define ED448_BASEPOINT_TIMES_2_225_Z  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
#define ED448_BASEPOINT_TIMES_2_225_T  { 0x2c62280e, 0x235d0c90, 0x53f06a9, 0x8fe2368, 0x5462e30, 0xde42843, 0xb236a9, 0x1aa52147, 0x8482f3a, 0x17404db, 0x2c6f0c5d, 0xc1d09a1, 0x1e392b56, 0xacd3d5c, 0x3b47311f, 0x3f5a32bd }

#define ED448_BASEPOINT_TIMES_2_225_PLUS_1_X  { 0x230a2be3, 0x1ea001f5, 0x11151c86, 0x24712a6f, 0x142d1e31, 0x34ff32c2, 0xdde3768, 0x1c21f1c, 0x19930158, 0x1b0d202a, 0x299d1ef8, 0x2df20592, 0x21de1893, 0x216a2129, 0x164d2077, 0x36eb0a71 }
#define ED448_BASEPOINT_TIMES_2_225_PLUS_1_Y  { 0x33450278, 0x3ee2233e, 0x1ef5197e, 0x10c81353, 0x3e4225f9, 0x26d2328d, 0x19771355, 0x2e1e0171, 0x30f075f, 0xb37160b, 0x3f210b1a, 0x3d520750, 0x3f202e0d, 0x16c235a6, 0x12213031, 0x3a7e0be3 }
#define ED448_BASEPOINT_TIMES_2_225_PLUS_1_Z  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
#define ED448_BASEPOINT_TIMES_2_225_PLUS_1_T  { 0x27dc38df, 0xebf2b47, 0x159c3590, 0x3b23077c, 0x137120e3, 0x1b8414bc, 0x269e3037, 0x1934084f, 0x24e5137f, 0x1b8505db, 0x36bf0480, 0xef90c02, 0x4ee2cc2, 0x23912e01, 0x7022bc, 0xddf10c1 }

#endif /* __PORT_FIELD_ELEMENTS_OVERRIDES_H */