#include <field_elements/field_elements_ed448.h>
#include <utils/utils.h>

#if !FE3C_32BIT
    #error "Build system inconsistency detected! field_elements_ed448_32.c in use despite FE3C_32BIT not being set"
#endif /* !FE3C_32BIT */

/* To reduce clutter, define a symbol for inline asm line termination */
#define _ "\n\t"

/* Field characteristic */
static const fe448 fe448_p = {
    0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
    0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
    0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
    0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
};
/* -S-1 where S = 2^224 */
static const fe448 fe448_mSm1 = {
    0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
    0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
    0x3fff3ffd, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
    0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
};
/* Elliptic curve constant d = -39081 */
const fe448 ed448_d = {
    0x3ffd2756, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
    0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
    0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
    0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
};
#if FE3C_ED448_ISOGENY
/* Twisted elliptic curve constant d' = d-1 */
const fe448 ed448twist_d = {
    0x3ffd2755, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
    0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
    0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
    0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
};
#endif /* FE3C_ED448_ISOGENY */
/* Additive identity in the field */
const fe448 fe448_zero = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
/* Multiplicative identity in the field */
const fe448 fe448_one = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/**
 * @brief Uncondionally copy a field element
 * @param r Destination field element
 * @param a Source field element
 */
void fe448_copy(fe448 r, const fe448 a) {

    if (a != r) {

        (void) memcpy(r, a, sizeof(fe448));
    }
}

/**
 * @brief Do a constant-time equality check
 * @param[in] a The first element to check
 * @param[in] b The second element to check
 * @return 1 if a = b, 0 otherwise
 * @note The elements should be reduced by the caller first
 */
int fe448_equal(const fe448 a, const fe448 b) {

    /* Assume equality at first */
    int equal = 1;
    u32 ax;
    u32 bx;
    u32 zero;
    /* This is essentially memcmp, but without early breaks and with a hardware loop */
    asm volatile(
        /* Initialize the zero register */
        _ "movi.n %[zero], 0"

        /* Set up a hardware loop */
        _ "movi.n %[ax],    %[word_count]"
        _ "loop   %[ax],    equal.endloop"
    _ "equal.startloop:"
        /* Load the limbs */
        _ "l32i.n %[ax],    %[a],    0"
        _ "l32i.n %[bx],    %[b],    0"

        /* XOR the limbs and set equal to zero if the result is non-zero */
        _ "xor    %[ax],    %[ax],   %[bx]"
        _ "movnez %[equal], %[zero], %[ax]"

        /* Advance the iterators */
        _ "addi.n %[a],     %[a],    4"
        _ "addi.n %[b],     %[b],    4"

    _ "equal.endloop:"

        : [equal] "+r" (equal),
          [zero] "=r" (zero),
          [a] "+r" (a),
          [b] "+r" (b),
          [ax] "=r" (ax),
          [bx] "=r" (bx)
        : [word_count] "i" (ED448_FE_LIMB_COUNT)
        :
    );

    return equal;
}

/**
 * @brief Check if an elements is in its canonical form, i.e. less than the modulus
 * @param a Field element to check
 * @return 1 if a is in canonical form, 0 otherwise
 */
static inline int fe448_is_canonical(const fe448 a) {

    /* Note that this function is only used internally and we can safely assume that
     * the input is smaller than 2p. For field elements generated internally, we
     * have a guarantee that they are weakly reduced, i.e. smaller than 2p, and as for
     * the (potentially adversarial) inputs: encodings of integers above 2p have the
     * highest byte not equal to zero - a condition checked for explicitly in
     * fe448_decode. */

    /* Iterators over the limbs */
    const fe_limb_type * ai = a;
    const fe_limb_type * pi = fe448_p;
    /* Registers to store the values of limbs, any borrow and an
     * indicator whether or not the input is equal to the modulus
     * (in which case it is also not canonical despite the borrow
     * being zero). */
    u32 ax;
    u32 px;
    u32 borrow;
    u32 notp;

    /* Try subtracting a from p and check if borrow is set afterwards */
    asm volatile(
        /* Initialize the borrow to zero */
        _ "movi.n %[borrow], 0"
        /* Initialize the equality indicator */
        _ "movi.n %[notp],   0"

        /* Set up a hardware loop */
        _ "movi.n %[ax],     %[word_count]"
        _ "loop   %[ax],     is_canonical%=.endloop"
    _ "is_canonical%=.startloop:"
        /* Load the operands' limbs */
        _ "l16ui  %[ax],     %[ai],     0"
        _ "l16ui  %[px],     %[pi],     0"

        /* Subtract the low halves and the borrow (write the result to
         * ax as we discard it anyways since we only care about the borrow) */
        _ "sub    %[ax],     %[px],     %[ax]"
        /* Update the equality indicator */
        _ "movnez %[notp],   %[ax],     %[ax]"
        _ "sub    %[ax],     %[ax],     %[borrow]"
        /* Extract the sign bit to identify the borrow */
        _ "extui  %[borrow], %[ax],     31, 1"

        /* Repeat for the high halves */
        _ "l16ui  %[ax],     %[ai],     2"
        _ "l16ui  %[px],     %[pi],     2"
        _ "sub    %[ax],     %[px],     %[ax]"
        _ "movnez %[notp],   %[ax],     %[ax]"
        _ "sub    %[ax],     %[ax],     %[borrow]"
        _ "extui  %[borrow], %[ax],     31, 1"

        /* Advance the iterators */
        _ "addi.n %[ai],     %[ai],     4"
        _ "addi.n %[pi],     %[pi],     4"

    _ "is_canonical%=.endloop:"

        /* Normalize the equality indicator */
        _ "movi.n %[ax],     1"
        _ "movnez %[notp],   %[ax],     %[notp]"

        : [ai]         "+r" (ai),
          [pi]         "+r" (pi),
          [ax]         "=r" (ax),
          [px]         "=r" (px),
          [borrow]     "=r" (borrow),
          [notp]       "=r" (notp)
        : [word_count] "i"  (ED448_FE_LIMB_COUNT)
        : "memory"
    );

    /* A field element is canonical if and only if there was
     * no borrow at the end of the loop, i.e. a <= p, and
     * notp is set, i.e. a != p */
    return ~borrow & notp;
}

/**
 * @brief Do a constant-time conditional move
 * @param[out] r The result field element set to a conditionally or left alone
 * @param[in] a The element to conditionally move into r
 * @param[in] move Flag deciding on the branch, if set to 0, r ::= r, and if set to 1, r ::= a
 * @note If move is set to anything other than 0 or 1, the results are undefined
 */
void fe448_conditional_move(fe448 r, const fe448 a, int move) {

    const fe_limb_type * ai = a;
    fe_limb_type * ri = r;
    u32 rx;
    u32 ax;
    asm volatile(
        /* Set up a hardware loop */
        _ "movi.n %[ax], %[word_count]"
        _ "loop   %[ax], conditional_move.endloop"
    _ "conditional_move.startloop:"
        /* Load the limbs */
        _ "l32i.n %[rx], %[ri], 0"
        _ "l32i.n %[ax], %[ai], 0"
        /* Conditionally assign rx := ax if move is non-zero */
        _ "movnez %[rx], %[ax], %[move]"
        _ "s32i.n %[rx], %[ri], 0"

        /* Advance the iterators */
        _ "addi.n %[ai], %[ai], 4"
        _ "addi.n %[ri], %[ri], 4"
    _ "conditional_move.endloop:"

        : [ai]   "+&r" (ai),
          [ri]   "+&r" (ri),
          [rx]   "=&r" (rx),
          [ax]   "=&r" (ax)
        : [word_count] "i" (ED448_FE_LIMB_COUNT),
          [move] "r"  (move)
        : "memory"
    );
}

/**
 * @brief Do a weak modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note Note that the result need to be in canonical form, i.e. between 0 and p-1, it need only be less than 2p
 */
void fe448_weak_reduce(fe448 r, const fe448 a) {

    /* On Ed448 we always keep the field elements weakly reduced */
    fe448_copy(r, a);
}

/**
 * @brief Subtract one field element from another
 * @param[out] r Result of the (possibly mock) subtraction, i.e. the difference r = a - b * mock, where mock is 0 or 1
 * @param[in] a Minuend
 * @param[in] b Subtrahend
 * @param mock Flag controlling whether subtraction is actually performed or result is set to a
 * @return Borrow if b > a and mock is zero
 */
static inline u32 fe448_sub_internal(fe448 r, const fe448 a, const fe448 b, int mock) {

    /* This is an internal function that performs the subtraction a - b directly and
     * returns any borrow that may occur. */

    /* Registers to store the values of limbs */
    u32 ax;
    u32 bx;
    u32 rx;
    u32 borrow;

    asm volatile(
        /* Initialize the borrow to zero */
        _ "movi.n %[borrow], 0"

        /* Set up a hardware loop */
        _ "movi.n %[ax],     %[word_count]"
        _ "loop   %[ax],     sub_internal%=.endloop"
    _ "sub_internal%=.startloop:"
        /* Load the operands' limbs. */
        _ "l16ui  %[ax],     %[ai],     0"
        _ "l16ui  %[bx],     %[bi],     0"

        /* If mock=1, set the subtrahend to zero (note that borrow
         * will remain zero for the whole loop in that case) */
        _ "movnez %[bx],     %[borrow], %[mock]"

        /* Subtract the low halves and the borrow */
        _ "sub    %[rx],     %[ax],     %[bx]"
        _ "sub    %[rx],     %[rx],     %[borrow]"
        /* Extract the sign bit to identify the borrow */
        _ "extui  %[borrow], %[rx],     31, 1"
        /* Write the low 14 bits of the result back to memory */
        _ "extui  %[rx],     %[rx],     0, 14"
        _ "s16i   %[rx],     %[ri],     0"

        /* Repeat for the high halves */
        _ "l16ui  %[ax],     %[ai],     2"
        _ "l16ui  %[bx],     %[bi],     2"
        _ "movnez %[bx],     %[borrow], %[mock]"
        _ "sub    %[rx],     %[ax],     %[bx]"
        _ "sub    %[rx],     %[rx],     %[borrow]"
        _ "extui  %[borrow], %[rx],     31, 1"
        _ "extui  %[rx],     %[rx],     0, 14"
        _ "s16i   %[rx],     %[ri],     2"

        /* Advance the iterators */
        _ "addi.n %[ai],     %[ai],     4"
        _ "addi.n %[bi],     %[bi],     4"
        _ "addi.n %[ri],     %[ri],     4"

    _ "sub_internal%=.endloop:"

        : [ai]         "+&r" (a),
          [bi]         "+&r" (b),
          [ri]         "+&r" (r),
          [ax]         "=&r" (ax),
          [bx]         "=&r" (bx),
          [rx]         "=&r" (rx),
          [borrow]     "=&r" (borrow)
        : [word_count] "i"   (ED448_FE_LIMB_COUNT),
          [mock]       "r"   (mock)
        : "memory"
    );

    return borrow;
}

/**
 * @brief Do a strong modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note The result is guaranteed to be in canonical form, i.e. between 0 and p-1
 */
void fe448_strong_reduce(fe448 r, const fe448 a) {

    /* Note that the field elements on Ed448 are always weakly reduced */

    /* Check canonicity of a */
    int canonical = fe448_is_canonical(a);
    /* Compute a-p and conditionally use it as a result if a is larger than p */
    (void) fe448_sub_internal(r, a, fe448_p, canonical);
}

/**
 * @brief Negate an element in the field
 * @param[out] r The result of negation
 * @param[in] a Element to be negated
 */
void fe448_neg(fe448 r, const fe448 a) {

    fe448_sub(r, fe448_p, a);
}

/**
 * @brief Add two field elements
 * @param[out] r Result of the (possibly mock) addition, i.e. the sum r = a + b * mock, where mock is 0 or 1
 * @param[in] a Operand
 * @param[in] b Operand
 * @param mock Flag controlling whether addition is actually performed or result is set to a
 */
static inline void fe448_add_internal(fe448 r, const fe448 a, const fe448 b, int mock) {

    /* Iterators over the limbs */
    const fe_limb_type * ai = a;
    const fe_limb_type * bi = b;
    fe_limb_type * ri = r;
    /* Registers to store the values of limbs */
    u32 ax;
    u32 bx;
    u32 rx;
    u32 carry;

    asm volatile(
        /* Initialize the carry to zero */
        _ "movi.n %[carry], 0"

        /* Set up a hardware loop */
        _ "movi.n %[ax],    %[word_count]"
        _ "loop   %[ax],    add_internal%=.endloop"
    _ "add_internal%=.startloop:"
        /* Load the operands' limbs. */
        _ "l16ui  %[ax],    %[ai],    0"
        _ "l16ui  %[bx],    %[bi],    0"
        /* If mock=1, set the second operand to zero (note that carry
         * will remain zero for the whole loop in that case) */
        _ "movnez %[bx],    %[carry], %[mock]"

        /* Add the low halves */
        _ "add.n  %[rx],    %[ax],    %[bx]"
        /* Add the carry */
        _ "add.n  %[rx],    %[rx],    %[carry]"
        /* Write back to memory the low 14 bits of the result... */
        _ "extui  %[carry], %[rx],    0, 14"
        _ "s16i   %[carry], %[ri],    0"
        /* ...and put any overflow back in carry */
        _ "srli   %[carry], %[rx],    14"

        /* Repeat for the high halves */
        _ "l16ui  %[ax],    %[ai],    2"
        _ "l16ui  %[bx],    %[bi],    2"
        _ "movnez %[bx],    %[carry], %[mock]"
        _ "add.n  %[rx],    %[ax],    %[bx]"
        _ "add.n  %[rx],    %[rx],    %[carry]"
        _ "extui  %[carry], %[rx],    0, 14"
        _ "s16i   %[carry], %[ri],    2"
        _ "srli   %[carry], %[rx],    14"

        /* Advance the iterators */
        _ "addi.n %[ai],    %[ai],    4"
        _ "addi.n %[bi],    %[bi],    4"
        _ "addi.n %[ri],    %[ri],    4"
    _ "add_internal%=.endloop:"

        /* Do a weak reduction. Note that we use the identity
         *
         *   (S^2 z + S y + x) = x + (S^2 - S - 1) z + S z + z + S y =
         *              = S(z + y) + (x + z) (mod p)
         *
         * where S = 2^224 and p = S^2 - S - 1.
         *
         * Note that 448 / 14 = 32, so whatever is in the carry register at this
         * point is the overflow beyond the 2^448 boundary (z in the above identity).
         * We shall distribute it at the weight-S and weight-1 level. */

        /* Make a copy of z for future use */
        _ "mov.n  %[ax],    %[carry]"

        /* Reinitialize the result iterator */
        _ "mov.n  %[ri],    %[r]"

        /* Loop over the first 16 limbs */
        _ "movi.n %[rx],    %[word_count] / 2"
        _ "loop   %[rx],    add_internal%=.endreduceloop"
    _ "add_internal%=.startreduceloop:"
        /* Load a 14-bit limb */
        _ "l16ui  %[rx],    %[ri],    0"
        /* Add the carry to it */
        _ "add.n  %[rx],    %[rx],    %[carry]"
        /* Write the low 14 bits back to memory... */
        _ "extui  %[carry], %[rx],    0, 14"
        _ "s16i   %[carry], %[ri],    0"
        /* ...and put anything above 14 bits back into carry */
        _ "srli   %[carry], %[rx],    14"

        /* Repeat for the top half of the word */
        _ "l16ui  %[rx],    %[ri],    2"
        _ "add.n  %[rx],    %[rx],    %[carry]"
        _ "extui  %[carry], %[rx],    0, 14"
        _ "s16i   %[carry], %[ri],    2"
        _ "srli   %[carry], %[rx],    14"

        /* Advance the iterator */
        _ "addi.n %[ri],    %[ri],    4"
    _ "add_internal%=.endreduceloop:"

        /* At this point add the previously stored z back
         * to the carry before continuing */
        _ "add.n  %[carry], %[carry], %[ax]"

        _ "movi.n %[rx],    %[word_count] / 2"
        _ "loop   %[rx],    add_internal%=.endreduceloop2"
    _ "add_internal%=.startreduceloop2:"
        _ "l16ui  %[rx],    %[ri],    0"
        _ "add.n  %[rx],    %[rx],    %[carry]"
        _ "extui  %[carry], %[rx],    0, 14"
        _ "s16i   %[carry], %[ri],    0"
        _ "srli   %[carry], %[rx],    14"

        _ "l16ui  %[rx],    %[ri],    2"
        _ "add.n  %[rx],    %[rx],    %[carry]"
        _ "extui  %[carry], %[rx],    0, 14"
        _ "s16i   %[carry], %[ri],    2"
        _ "srli   %[carry], %[rx],    14"

        _ "addi.n %[ri],    %[ri],    4"
    _ "add_internal%=.endreduceloop2:"

        : [ai]         "+&r" (ai),
          [bi]         "+&r" (bi),
          [ri]         "+&r" (ri),
          [ax]         "=&r" (ax),
          [bx]         "=&r" (bx),
          [rx]         "=&r" (rx),
          [carry]      "=&r" (carry)
        : [r]          "r"   (r),
          [mock]       "r"   (mock),
          [word_count] "i"   (ED448_FE_LIMB_COUNT)
        : "memory"
    );

    /* Do quasi-strong reduction if carry is set, i.e. make
     * the representation use only 32 14-bit limbs, without
     * any overflow bits. Note that such representation may
     * still not be canonical. */
    (void) fe448_sub_internal(r, r, fe448_p, 1 - carry);
}

/**
 * @brief Add two field elements
 * @param[out] r Result of the addition, i.e. the sum r = a + b
 * @param[in] a Operand
 * @param[in] b Operand
 */
void fe448_add(fe448 r, const fe448 a, const fe448 b) {

    fe448_add_internal(r, a, b, 0);
}

/**
 * @brief Subtract one field element from another
 * @param[out] r Result of the subtraction, i.e. the difference r = a - b
 * @param[in] a Minuend
 * @param[in] b Subtrahend
 */
void fe448_sub(fe448 r, const fe448 a, const fe448 b) {

    u32 borrow = fe448_sub_internal(r, a, b, 0);
    fe448_add_internal(r, r, fe448_mSm1, 1 - borrow);
}

/**
 * @brief Multiply two 16-limb integers without any reductions to produce a 32-limb integer
 * @param[out] r Resulting 32-limb integer
 * @param[in] a 16-limb operand
 * @param[in] b 16-limb operand
 * @return Carry
 * @note No result aliasing is supported
 */
static u32 multiply_distinct_16_limbs(fe_limb_type * restrict r, const fe_limb_type * a, const fe_limb_type * b) {

    const fe_limb_type * ai;
    const fe_limb_type * bi;
    u32 temp;
    u32 carrylo;
    u32 carryhi;

#define commit_to_memory(__dst, __word)                \
    /* Read the accumulator */                         \
    _ "movi.n    %[carryhi], 0"                        \
    _ "rsr.acclo %[carrylo]"                           \
    _ "xsr.acchi %[carryhi]"                           \
    /* Commit low 14 bits to memory */                 \
    _ "extui     %[temp], %[carrylo],    0, 14"        \
    _ "s16i      %[temp], %[" #__dst "], 2 * " #__word \
    /* Shift the accumulator */                        \
    _ "src       %[carrylo], %[carryhi], %[carrylo]"   \
    _ "wsr.acclo %[carrylo]"

    asm volatile(
            /* Clear the accumulator */
            _ "movi.n    %[temp], 0"
            _ "wsr.acclo %[temp]"
            _ "wsr.acchi %[temp]"
            /* Initialize the shift amount register */
            _ "ssai      14"

            _ "addi.n    %[ai], %[a], -4"
            _ "ldinc m0, %[ai]"
            _ "addi.n    %[bi], %[b],  4"
            _ "lddec m2, %[bi]"
            _ "mula.dd.ll                  m0, m2"  /* a0*b0 */

            commit_to_memory(r, 0)


            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a0*b1 */
            _ "mula.dd.hl                  m0, m2"  /* a1*b0 */

            commit_to_memory(r, 1)


            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a2*b0 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a1*b1 */
            _ "mula.dd.ll                  m0, m3"  /* a0*b2 */

            commit_to_memory(r, 2)


            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a0*b3 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a1*b2 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a2*b1 */
            _ "mula.dd.hl                  m1, m2"  /* a3*b0 */


            commit_to_memory(r, 3)


            _ "mula.dd.ll.lddec m1, %[ai], m0, m2"  /* a4*b0 */
            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a3*b1 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a2*b2 */
            _ "mula.dd.hh.ldinc m2, %[bi], m0, m3"  /* a1*b3 */

            _ "mula.dd.ll                  m0, m2"  /* a0*b4 */

            commit_to_memory(r, 4)


            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a0*b5 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a1*b4 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a2*b3 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a3*b2 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a4*b1 */
            _ "mula.dd.hl                  m0, m2"  /* a5*b0 */

            commit_to_memory(r, 5)


            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a6*b0 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a5*b1 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m3"  /* a4*b2 */
            _ "mula.dd.hh.ldinc m2, %[bi], m1, m3"  /* a3*b3 */

            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a2*b4 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a1*b5 */
            _ "mula.dd.ll                  m0, m3"  /* a0*b6 */

            commit_to_memory(r, 6)


            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a0*b7 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a1*b6 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a2*b5 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a3*b4 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a4*b3 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a5*b2 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a6*b1 */
            _ "mula.dd.hl                  m1, m2"  /* a7*b0 */


            commit_to_memory(r, 7)


            _ "mula.dd.ll.lddec m1, %[ai], m0, m2"  /* a8*b0 */
            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a7*b1 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a6*b2 */
            _ "mula.dd.hh.ldinc m2, %[bi], m0, m3"  /* a5*b3 */

            _ "mula.dd.ll.lddec m1, %[ai], m0, m2"  /* a4*b4 */
            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a3*b5 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a2*b6 */
            _ "mula.dd.hh.ldinc m2, %[bi], m0, m3"  /* a1*b7 */

            _ "mula.dd.ll                  m0, m2"  /* a0*b8 */

            commit_to_memory(r, 8)


            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a0*b9 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a1*b8 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a2*b7 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a3*b6 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a4*b5 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a5*b4 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a6*b3 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a7*b2 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a8*b1 */
            _ "mula.dd.hl                  m0, m2"  /* a9*b0 */

            commit_to_memory(r, 9)


            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a10*b0 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a9*b1 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m3"  /* a8*b2 */
            _ "mula.dd.hh.ldinc m2, %[bi], m1, m3"  /* a7*b3 */

            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a6*b4 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a5*b5 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m3"  /* a4*b6 */
            _ "mula.dd.hh.ldinc m2, %[bi], m1, m3"  /* a3*b7 */

            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a2*b8 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a1*b9 */
            _ "mula.dd.ll                  m0, m3"  /* a0*b10 */

            commit_to_memory(r, 10)


            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a0*b11 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a1*b10 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a2*b9 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a3*b8 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a4*b7 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a5*b6 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a6*b5 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a7*b4 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a8*b3 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a9*b2 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a10*b1 */
            _ "mula.dd.hl                  m1, m2"  /* a11*b0 */


            commit_to_memory(r, 11)


            _ "mula.dd.ll.lddec m1, %[ai], m0, m2"  /* a12*b0 */
            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a11*b1 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a10*b2 */
            _ "mula.dd.hh.ldinc m2, %[bi], m0, m3"  /* a9*b3 */

            _ "mula.dd.ll.lddec m1, %[ai], m0, m2"  /* a8*b4 */
            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a7*b5 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a6*b6 */
            _ "mula.dd.hh.ldinc m2, %[bi], m0, m3"  /* a5*b7 */

            _ "mula.dd.ll.lddec m1, %[ai], m0, m2"  /* a4*b8 */
            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a3*b9 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a2*b10 */
            _ "mula.dd.hh.ldinc m2, %[bi], m0, m3"  /* a1*b11 */

            _ "mula.dd.ll                  m0, m2"  /* a0*b12 */

            commit_to_memory(r, 12)


            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a0*b13 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a1*b12 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a2*b11 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a3*b10 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a4*b9 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a5*b8 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a6*b7 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a7*b6 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a8*b5 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a9*b4 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a10*b3 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a11*b2 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a12*b1 */
            _ "mula.dd.hl                  m0, m2"  /* a13*b0 */

            commit_to_memory(r, 13)


            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a14*b0 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a13*b1 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m3"  /* a12*b2 */
            _ "mula.dd.hh.ldinc m2, %[bi], m1, m3"  /* a11*b3 */

            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a10*b4 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a9*b5 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m3"  /* a8*b6 */
            _ "mula.dd.hh.ldinc m2, %[bi], m1, m3"  /* a7*b7 */

            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a6*b8 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a5*b9 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m3"  /* a4*b10 */
            _ "mula.dd.hh.ldinc m2, %[bi], m1, m3"  /* a3*b11 */

            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a2*b12 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a1*b13 */
            _ "mula.dd.ll                  m0, m3"  /* a0*b14 */

            commit_to_memory(r, 14)


            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a0*b15 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a1*b14 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a2*b13 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a3*b12 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a4*b11 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a5*b10 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a6*b9 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a7*b8 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a8*b7 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a9*b6 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a10*b5 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a11*b4 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a12*b3 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a13*b2 */
            _ "mula.dd.lh                  m1, m2"  /* a14*b1 */
            _ "mula.dd.hl                  m1, m2"  /* a15*b0 */


            commit_to_memory(r, 15)


            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a15*b1 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a14*b2 */
            _ "mula.dd.hh.ldinc m2, %[bi], m0, m3"  /* a13*b3 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m2"  /* a12*b4 */

            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a11*b5 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a10*b6 */
            _ "mula.dd.hh.ldinc m2, %[bi], m0, m3"  /* a9*b7 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m2"  /* a8*b8 */

            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a7*b9 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a6*b10 */
            _ "mula.dd.hh.ldinc m2, %[bi], m0, m3"  /* a5*b11 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m2"  /* a4*b12 */

            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a3*b13 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a2*b14 */
            _ "mula.dd.hh                  m0, m3"  /* a1*b15 */

            commit_to_memory(r, 16)


            _ "ldinc m1, %[ai]"
            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a2*b15 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a3*b14 */
            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a4*b13 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a5*b12 */

            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a6*b11 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a7*b10 */
            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a8*b9 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a9*b8 */

            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a10*b7 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a11*b6 */
            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a12*b5 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a13*b4 */

            _ "mula.dd.lh                  m1, m3"  /* a14*b3 */
            _ "mula.dd.hl                  m1, m3"  /* a15*b2 */

            commit_to_memory(r, 17)


            _ "mula.dd.hh.ldinc m2, %[bi], m1, m3"  /* a15*b3 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a14*b4 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a13*b5 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m3"  /* a12*b6 */

            _ "mula.dd.hh.ldinc m2, %[bi], m1, m3"  /* a11*b7 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a10*b8 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a9*b9 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m3"  /* a8*b10 */

            _ "mula.dd.hh.ldinc m2, %[bi], m1, m3"  /* a7*b11 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a6*b12 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a5*b13 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m3"  /* a4*b14 */

            _ "mula.dd.hh                  m1, m3"  /* a3*b15 */

            commit_to_memory(r, 18)


            _ "ldinc m0, %[ai]"
            _ "mula.dd.lh.lddec m2, %[bi], m0, m3"  /* a4*b15 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m3"  /* a5*b14 */
            _ "mula.dd.lh.lddec m3, %[bi], m1, m2"  /* a6*b13 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m2"  /* a7*b12 */

            _ "mula.dd.lh.lddec m2, %[bi], m0, m3"  /* a8*b11 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m3"  /* a9*b10 */
            _ "mula.dd.lh.lddec m3, %[bi], m1, m2"  /* a10*b9 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m2"  /* a11*b8 */

            _ "mula.dd.lh.lddec m2, %[bi], m0, m3"  /* a12*b7 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m3"  /* a13*b6 */
            _ "mula.dd.lh                  m1, m2"  /* a14*b5 */
            _ "mula.dd.hl                  m1, m2"  /* a15*b4 */


            commit_to_memory(r, 19)


            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a15*b5 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a14*b6 */
            _ "mula.dd.hh.ldinc m2, %[bi], m0, m3"  /* a13*b7 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m2"  /* a12*b8 */

            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a11*b9 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a10*b10 */
            _ "mula.dd.hh.ldinc m2, %[bi], m0, m3"  /* a9*b11 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m2"  /* a8*b12 */

            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a7*b13 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a6*b14 */
            _ "mula.dd.hh                  m0, m3"  /* a5*b15 */

            commit_to_memory(r, 20)


            _ "ldinc m1, %[ai]"
            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a6*b15 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a7*b14 */
            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a8*b13 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a9*b12 */

            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a10*b11 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a11*b10 */
            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a12*b9 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a13*b8 */

            _ "mula.dd.lh                  m1, m3"  /* a14*b7 */
            _ "mula.dd.hl                  m1, m3"  /* a15*b6 */

            commit_to_memory(r, 21)


            _ "mula.dd.hh.ldinc m2, %[bi], m1, m3"  /* a15*b7 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a14*b8 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a13*b9 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m3"  /* a12*b10 */

            _ "mula.dd.hh.ldinc m2, %[bi], m1, m3"  /* a11*b11 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a10*b12 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a9*b13 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m3"  /* a8*b14 */

            _ "mula.dd.hh                  m1, m3"  /* a7*b15 */

            commit_to_memory(r, 22)


            _ "ldinc m0, %[ai]"
            _ "mula.dd.lh.lddec m2, %[bi], m0, m3"  /* a8*b15 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m3"  /* a9*b14 */
            _ "mula.dd.lh.lddec m3, %[bi], m1, m2"  /* a10*b13 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m2"  /* a11*b12 */

            _ "mula.dd.lh.lddec m2, %[bi], m0, m3"  /* a12*b11 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m3"  /* a13*b10 */
            _ "mula.dd.lh                  m1, m2"  /* a14*b9 */
            _ "mula.dd.hl                  m1, m2"  /* a15*b8 */


            commit_to_memory(r, 23)


            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a15*b9 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a14*b10 */
            _ "mula.dd.hh.ldinc m2, %[bi], m0, m3"  /* a13*b11 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m2"  /* a12*b12 */

            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a11*b13 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a10*b14 */
            _ "mula.dd.hh                  m0, m3"  /* a9*b15 */

            commit_to_memory(r, 24)


            _ "ldinc m1, %[ai]"
            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a10*b15 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a11*b14 */
            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a12*b13 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a13*b12 */

            _ "mula.dd.lh                  m1, m3"  /* a14*b11 */
            _ "mula.dd.hl                  m1, m3"  /* a15*b10 */

            commit_to_memory(r, 25)


            _ "mula.dd.hh.ldinc m2, %[bi], m1, m3"  /* a15*b11 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m2"  /* a14*b12 */
            _ "mula.dd.hh.ldinc m3, %[bi], m0, m2"  /* a13*b13 */
            _ "mula.dd.ll.lddec m1, %[ai], m0, m3"  /* a12*b14 */

            _ "mula.dd.hh                  m1, m3"  /* a11*b15 */

            commit_to_memory(r, 26)


            _ "ldinc m0, %[ai]"
            _ "mula.dd.lh.lddec m2, %[bi], m0, m3"  /* a12*b15 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m3"  /* a13*b14 */
            _ "mula.dd.lh                  m1, m2"  /* a14*b13 */
            _ "mula.dd.hl                  m1, m2"  /* a15*b12 */


            commit_to_memory(r, 27)


            _ "mula.dd.hh.ldinc m3, %[bi], m1, m2"  /* a15*b13 */
            _ "mula.dd.ll.lddec m0, %[ai], m1, m3"  /* a14*b14 */
            _ "mula.dd.hh                  m0, m3"  /* a13*b15 */

            commit_to_memory(r, 28)


            _ "ldinc m1, %[ai]"
            _ "mula.dd.lh                  m1, m3"  /* a14*b15 */
            _ "mula.dd.hl                  m1, m3"  /* a15*b14 */

            commit_to_memory(r, 29)


            _ "mula.dd.hh                  m1, m3"  /* a15*b15 */

            commit_to_memory(r, 30)


            commit_to_memory(r, 31)


        : [ai] "=&r" (ai),
          [bi] "=&r" (bi),
          [carrylo] "=&r" (carrylo),
          [carryhi] "=&r" (carryhi),
          [temp] "=&r" (temp)
        : [word_count] "i" (ED448_FE_LIMB_COUNT),
          [a] "r" (a),
          [b] "r" (b),
          [r] "r" (r)
        : "memory", "acc"
    );

    return carrylo;
}

/**
 * @brief Multiply two field elements
 * @param[out] r Result of the multiplication, i.e. the product r = a b
 * @param[in] a Operand
 * @param[in] b Operand
 */
void fe448_mul(fe448 r, const fe448 a, const fe448 b) {

    /* Let a = u + vS and b = x + yS, where S = 2^224. Since we
     * are working modulo a "golden-ratio prime" we can leverage
     * fast Karatsuba multiplication by employing the identity:
     *
     *    (u + vS)(x + yS) = ux + (uy + vx)S + vyS^2
     *                     = (ux + vy) + (uy + vx + vy)S
     *                     = (ux + vy) + ((u+v)(x+y) - ux)S (mod p)
     *
     * where p = S^2 - S - 1. The additions (u+v) and (x+y) can be
     * performed extremely efficiently since we are using 14-bit
     * limbs, so by adding two 32-bit words we add two 14-bit limbs
     * at a time.
     *
     * Let A = (x+y)(u+v), B = vy, and C = ux. We shall denote by
     * AL = A (mod S) and AH = [A/S]. Similarly, we define BL, BH,
     * CL, and CH.
     *
     * The result of a*b (denoted RH|RL) shall have the following
     * layout:
     *
     *            2^224 boundary_________
     *            2^448 boundary____     |
     *                              | RH | RL |
     * -----------------------------|----|----|
     *                           AH | AL |    |
     *                              | BH | BL |
     *                              | CH | CL |
     *                          -CH |-CL |    |
     *
     * After reducing modulo p using the identity:
     *
     *                     2^448 = 2^224 + 1 (mod p)
     *
     * we obtain the following layout:
     *
     *            2^224 boundary_________
     *            2^448 boundary____     |
     *                              | RH | RL |
     * -----------------------------|----|----|
     *                              | AL |    |
     *                              | BH | BL |
     *                              | CH | CL |
     *                              |-CL |    |
     *                              | AH | AH |
     *                              |-CH |-CH |
     *
     * or after cancelling common terms:
     *
     *            2^224 boundary_________
     *            2^448 boundary____     |
     *                              | RH | RL |
     * -----------------------------|----|----|
     *                              | AL |-CH |
     *                              | BH | BL |
     *                              |-CL | CL |
     *                              | AH | AH |
     *
     * We shall start by computing A and C, and storing them
     * aside on the stack. B shall be put directly into the
     * result buffer. Then parts of A and C shall be added or
     * subtracted according to the diagram above.
     */

    /* Use an intermediate result buffer to support aliasing */
    fe448 _r;
    fe_limb_type _A[ED448_FE_LIMB_COUNT];
    fe_limb_type _C[ED448_FE_LIMB_COUNT];
    fe_limb_type * A = _A;
    fe_limb_type * C = _C;
    /* Reuse the C buffer for (u+v) and (x+y) as these are only needed temporarily */
    fe_limb_type * uplusv = C;
    fe_limb_type * xplusy = C + 8;

    u32 AE;
    u32 aux1;
    u32 aux2;
    u32 aux3;
    u32 aux4;
    u32 carry;

#define add_karatsuba(__src, __dst, __word)                \
    _ "l32i  %[aux1], %[" #__src "], 4 * " #__word         \
    _ "l32i  %[aux2], %[" #__src "], 4 * 8 + 4 * " #__word \
    _ "add.n %[aux1], %[aux1],       %[aux2]"              \
    _ "s32i  %[aux1], %[" #__dst "], 4 * " #__word

    asm volatile(

        add_karatsuba(a, uplusv, 0)
        add_karatsuba(a, uplusv, 1)
        add_karatsuba(a, uplusv, 2)
        add_karatsuba(a, uplusv, 3)
        add_karatsuba(a, uplusv, 4)
        add_karatsuba(a, uplusv, 5)
        add_karatsuba(a, uplusv, 6)
        add_karatsuba(a, uplusv, 7)

        add_karatsuba(b, xplusy, 0)
        add_karatsuba(b, xplusy, 1)
        add_karatsuba(b, xplusy, 2)
        add_karatsuba(b, xplusy, 3)
        add_karatsuba(b, xplusy, 4)
        add_karatsuba(b, xplusy, 5)
        add_karatsuba(b, xplusy, 6)
        add_karatsuba(b, xplusy, 7)

        : [uplusv]  "+&r" (uplusv),
          [xplusy]  "+&r" (xplusy),
          [aux1]    "=&r" (aux1),
          [aux2]    "=&r" (aux2)

        : [word_count] "i" (ED448_FE_LIMB_COUNT),
          [a] "r" (a),
          [b] "r" (b)

        : "memory", "acc"
    );

    /* Compute A = (u+v)(x+y) */
    AE = multiply_distinct_16_limbs(A, uplusv, xplusy);

    /* Note that AH can be more than 16-bits long, i.e. we can get up to two bits
     * of extra carry which exceeds the S^2 boundary. This overflow will have to
     * be reduced one more time at the weight-1 and weight-S level. Let AE denote
     * this "extra" overflow of A. Then our diagram has to be updated and now reads:
     *
     *            2^224 boundary_________
     *            2^448 boundary____     |
     *                              | RH | RL |
     * -----------------------------|----|----|
     *                              | AL |-CH |
     *                              | BH | BL |
     *                              |-CL | CL |
     *                              | AH | AH |
     *                           AE | AE |    |
     *
     * or after this extra reduction:
     *
     *            2^224 boundary_________
     *            2^448 boundary____     |
     *                              | RH | RL |
     * -----------------------------|----|----|
     *                              | AL |-CH |
     *                              | BH | BL |
     *                              |-CL | CL |
     *                              | AH | AH |
     *                              |2AE | AE |
     */

    /* Compute C = ux. Note that since u < S and x < S, there can be no overflow
     * above S^2 after multiplying them */
    aux1 = multiply_distinct_16_limbs(C, a, b);
    FE3C_SANITY_CHECK(aux1 == 0, NULL);

    /* Compute B = vy and put the result directly in the result buffer. Again,
     * since we assume all inputs are below S^2, i.e. v < S and y < S, there
     * can be no overflow above S^2. */
    aux1 = multiply_distinct_16_limbs(_r, a + 8, b + 8);
    FE3C_SANITY_CHECK(aux1 == 0, NULL);

    asm volatile(

        /* We already have BH | BL in place. Iterate over the limbs of the result
         * and add/subtract the missing pieces */

        /* Initialize the carry to the overflow of (u+v)(x+y) */
        _ "mov.n  %[carry], %[AE]"
        /* Set up the iterators */
        _ "mov.n  %[riter], %[r]"
        _ "mov.n  %[Aiter], %[A]"
        _ "mov.n  %[Citer], %[C]"

        /* Set up a hardware loop over half the limbs,
         * which coincidentally is equal to the number
         * of words in a field element */
        _ "movi.n %[temp],  %[word_count]"
        _ "loop   %[temp],  mul.endlloop"
    _ "mul.startlloop:"

        /* Add a limb of BL */
        _ "l16ui  %[temp],  %[riter], 0"
        _ "add.n  %[carry], %[carry], %[temp]"
        /* Add a limb of CL */
        _ "l16ui  %[temp],  %[Citer], 0"
        _ "add.n  %[carry], %[carry], %[temp]"
        /* Add a limb of AH */
        _ "l16ui  %[temp],  %[Aiter], 8 * 4"
        _ "add.n  %[carry], %[carry], %[temp]"
        /* Subtract a limb of CH */
        _ "l16ui  %[temp],  %[Citer], 8 * 4"
        _ "sub    %[carry], %[carry], %[temp]"

        /* Write back to memory and shift the carry
         * (note that carry may be negative at this point
         * so we must use an arithmetic shift instruction
         * - srai - and not a logical shift - srli) */
        _ "extui  %[temp],  %[carry], 0, 14"
        _ "s16i   %[temp],  %[riter], 0"
        _ "srai   %[carry], %[carry], 14"

        /* Advance the iterators */
        _ "addi.n %[riter], %[riter], 2"
        _ "addi.n %[Aiter], %[Aiter], 2"
        _ "addi.n %[Citer], %[Citer], 2"
        /* Note that we could leverage the fact that the distance
         * between the buffers (and the respective iterators) is
         * constant throughout the loop and known at compile-time.
         * Benchmarking shows, however, that the performance
         * improvement from doing this is negligible. */

    _ "mul.endlloop:"

        /* Add AE again (and twice) to the carry */
        _ "add.n  %[carry], %[carry], %[AE]"
        _ "add.n  %[carry], %[carry], %[AE]"

        /* Reset the A and C iterators, the iterator over the result (or B)
         * is set up correctly after the previous loop */
        _ "mov.n  %[Aiter], %[A]"
        _ "mov.n  %[Citer], %[C]"

        /* Set up a hardware loop over half the limbs */
        _ "movi.n %[temp],  %[word_count]"
        _ "loop   %[temp],  mul.endhloop"
    _ "mul.starthloop:"

        /* Add a limb of BH */
        _ "l16ui  %[temp],  %[riter], 0"
        _ "add.n  %[carry], %[carry], %[temp]"
        /* Add a limb of AL */
        _ "l16ui  %[temp],  %[Aiter], 0"
        _ "add.n  %[carry], %[carry], %[temp]"
        /* Add a limb of AH */
        _ "l16ui  %[temp],  %[Aiter], 8 * 4"
        _ "add.n  %[carry], %[carry], %[temp]"
        /* Subtract a limb of CL */
        _ "l16ui  %[temp],  %[Citer], 0"
        _ "sub    %[carry], %[carry], %[temp]"

        /* Write back to memory and shift the carry */
        _ "extui  %[temp],  %[carry], 0, 14"
        _ "s16i   %[temp],  %[riter], 0"
        _ "srai   %[carry], %[carry], 14"

        /* Advance the iterators */
        _ "addi.n %[riter], %[riter], 2"
        _ "addi.n %[Aiter], %[Aiter], 2"
        _ "addi.n %[Citer], %[Citer], 2"

    _ "mul.endhloop:"

        /* At this point we have must do a weak reduction (and a strong reduction later as well),
         * but note that carry may be negative at this point, which means we must do arithmetic
         * shifts and not logical ones */

        /* Reuse Aiter to store the carry which must be added at both weight-1 and weight-S levels */
        _ "mov.n  %[Aiter], %[carry]"
        /* Reset the result iterator*/
        _ "mov.n  %[riter], %[r]"

        /* Set up a loop over the low 16 limbs - unroll it partially */
        _ "movi.n %[temp],  %[word_count] / 2"
        _ "loop   %[temp],  mul.endloredloop"
    _ "mul.startloredloop:"
        /* Load a 14-bit limb */
        _ "l16ui  %[temp],  %[riter], 0"
        /* Add the carry to it */
        _ "add.n  %[carry], %[temp],  %[carry]"
        /* Write the low 14 bits back to memory */
        _ "extui  %[temp],  %[carry], 0, 14"
        _ "s16i   %[temp],  %[riter], 0"
        _ "srai   %[carry], %[carry], 14"

        /* Repeat for the top half of the word */
        _ "l16ui  %[temp],  %[riter], 2"
        /* Add the carry to it */
        _ "add.n  %[carry], %[temp],  %[carry]"
        /* Write the low 14 bits back to memory */
        _ "extui  %[temp],  %[carry], 0, 14"
        _ "s16i   %[temp],  %[riter], 2"
        _ "srai   %[carry], %[carry], 14"

        /* Advance the iterator */
        _ "addi.n %[riter], %[riter], 4"
    _ "mul.endloredloop:"

        /* Add the original overflow again at the weight-S level */
        _ "add.n %[carry],  %[carry], %[Aiter]"

        /* Set up a loop over the top 16 limbs */
        _ "movi.n %[temp],  %[word_count] / 2"
        _ "loop   %[temp],  mul.endhiredloop"
    _ "mul.starthiredloop:"
        /* Load a 14-bit limb */
        _ "l16ui  %[temp],  %[riter], 0"
        /* Add the carry to it */
        _ "add.n  %[carry], %[temp],  %[carry]"
        /* Write the low 14 bits back to memory */
        _ "extui  %[temp],  %[carry], 0, 14"
        _ "s16i   %[temp],  %[riter], 0"
        _ "srai   %[carry], %[carry], 14"

        /* Repeat for the top half of the word */
        _ "l16ui  %[temp],  %[riter], 2"
        /* Add the carry to it */
        _ "add.n  %[carry], %[temp],  %[carry]"
        /* Write the low 14 bits back to memory */
        _ "extui  %[temp],  %[carry], 0, 14"
        _ "s16i   %[temp],  %[riter], 2"
        _ "srai   %[carry], %[carry], 14"

        /* Advance the iterator */
        _ "addi.n %[riter], %[riter], 4"
    _ "mul.endhiredloop:"

        : [A]          "+&r" (A),
          [C]          "+&r" (C),
          [Aiter]      "=&r" (aux1),
          [Citer]      "=&r" (aux2),
          [riter]      "=&r" (aux3),
          [temp]       "=&r" (aux4),
          [carry]      "=&r" (carry)

        : [word_count] "i"   (ED448_FE_LIMB_COUNT),
          [r]          "r"   (_r),
          [AE]         "r"   (AE)

        : "memory", "acc"
    );

    /* If we have nonzero carry at this point, we must do a
     * reduction to keep the integer in 32 14-bit limbs. Note,
     * however, that such representation is not necessarily
     * canonical. */
    FE3C_SANITY_CHECK(carry < 2, NULL);
    (void) fe448_sub_internal(r, _r, fe448_p, 1 - carry);
}

/**
 * @brief Multiply an element by itself
 * @param[out] r Result of the squaring, i.e. the product r = a a
 * @param[in] a Field element to square
 */
void fe448_square(fe448 r, const fe448 a) {

    fe448_mul(r, a, a);
}

/**
 * @brief Encode a field element according to RFC 8032
 * @param[out] buffer Output buffer for the encoded field element
 * @param[in] a Field element to encode
 */
void fe448_encode(u8 * buffer, fe448 a) {

    /* Use word-based memory accesses since encoding is little-endian
     * and so is the CPU */
    u32 * words = (u32 *) buffer;
    /* Iterate over the 14-bit limbs instead of the 32-bit words */
    u16 * limb14 = (u16 *) a;

    /* In the first word of the output buffer we shall fit the first
     * two 14-bit limbs as well as four bits of the third limb */
    words[ 0] = (limb14[ 0] >>  0) | (limb14[ 1] << 14) | ( (limb14[ 2] &    0xF) << 28 );
    /* 10 bits of limb14[2], 14 bits of limb14[3], 8 bits of limb14[4] */
    words[ 1] = (limb14[ 2] >>  4) | (limb14[ 3] << 10) | ( (limb14[ 4] &   0xFF) << 24 );
    /* 6 bits of limb14[4], 14 bits of limb14[5], 12 bits of limb14[6] */
    words[ 2] = (limb14[ 4] >>  8) | (limb14[ 5] <<  6) | ( (limb14[ 6] &  0xFFF) << 20 );
    /* 2 bits of limb14[6], 14 bits of limb14[7], 14 bits of limb14[8], 2 bits of limb14[9] */
    words[ 3] = (limb14[ 6] >> 12) | (limb14[ 7] <<  2) | ( (limb14[ 8]         ) << 16 ) | ( (limb14[ 9] & 0xF) << 30 );
    /* 12 bits of limb14[9], 14 bits of limb14[10], 6 bits of limb14[11] */
    words[ 4] = (limb14[ 9] >>  2) | (limb14[10] << 12) | ( (limb14[11] &   0x3F) << 26 );
    /* 8 bits of limb14[11], 14 bits of limb14[12], 10 bits of limb14[13] */
    words[ 5] = (limb14[11] >>  6) | (limb14[12] <<  8) | ( (limb14[13] &  0x3FF) << 22 );
    /* 4 bits of limb14[13], 14 bits of limb14[14], 14 bits of limb14[15] */
    words[ 6] = (limb14[13] >> 10) | (limb14[14] <<  4) | ( (limb14[15]         ) << 18 );

    /* Rinse and repeat for the next 7 words */
    words[ 7] = (limb14[16] >>  0) | (limb14[17] << 14) | ( (limb14[18] &    0xF) << 28 );
    words[ 8] = (limb14[18] >>  4) | (limb14[19] << 10) | ( (limb14[20] &   0xFF) << 24 );
    words[ 9] = (limb14[20] >>  8) | (limb14[21] <<  6) | ( (limb14[22] &  0xFFF) << 20 );
    words[10] = (limb14[22] >> 12) | (limb14[23] <<  2) | ( (limb14[24]         ) << 16 ) | ( (limb14[25] & 0xF) << 30 );
    words[11] = (limb14[25] >>  2) | (limb14[26] << 12) | ( (limb14[27] &   0x3F) << 26 );
    words[12] = (limb14[27] >>  6) | (limb14[28] <<  8) | ( (limb14[29] &  0x3FF) << 22 );
    words[13] = (limb14[29] >> 10) | (limb14[30] <<  4) | ( (limb14[31]         ) << 18 );

    /* Clear the highest byte */
    buffer[56] = 0;
}

/**
 * @brief Decode a field element according to RFC 8032
 * @param[out] r Decoded field element
 * @param[out] buffer Encoding of the field element
 * @return 1 if decoding succeeded, 0 otherwise
 */
__attribute__((warn_unused_result))
int fe448_decode(fe448 r, const u8 * buffer) {

    /* Use word-based memory accesses since encoding is little-endian
     * and so is the CPU */
    const u32 * words = (const u32 *) buffer;
    /* Iterate over the 14-bit limbs instead of the 32-bit words */
    u16 * limb14 = (u16 *) r;

    /* First do the 16-bit reads and then clear the top bits in one go
     * for code clarity (at the cost of worse cache usage, though) */

    /* Read in 14 bits from the first word */
    limb14[ 0] = (words[ 0] >>  0);
    /* Read in the next 14 bits from the same word */
    limb14[ 1] = (words[ 0] >> 14);
    /* Read in the remaining 4 bits from the first word, and the rest from
     * the next one */
    limb14[ 2] = (words[ 0] >> 28) | (words[ 1] <<  4);
    limb14[ 3] = (words[ 1] >> 10);
    limb14[ 4] = (words[ 1] >> 24) | (words[ 2] <<  8);
    limb14[ 5] = (words[ 2] >>  6);
    limb14[ 6] = (words[ 2] >> 20) | (words[ 3] << 12);
    limb14[ 7] = (words[ 3] >>  2);
    limb14[ 8] = (words[ 3] >> 16);
    limb14[ 9] = (words[ 3] >> 30) | (words[ 4] <<  2);
    limb14[10] = (words[ 4] >> 12);
    limb14[11] = (words[ 4] >> 26) | (words[ 5] <<  6);
    limb14[12] = (words[ 5] >>  8);
    limb14[13] = (words[ 5] >> 22) | (words[ 6] << 10);
    limb14[14] = (words[ 6] >>  4);
    limb14[15] = (words[ 6] >> 18);

    limb14[16] = (words[ 7] >>  0);
    limb14[17] = (words[ 7] >> 14);
    limb14[18] = (words[ 7] >> 28) | (words[ 8] <<  4);
    limb14[19] = (words[ 8] >> 10);
    limb14[20] = (words[ 8] >> 24) | (words[ 9] <<  8);
    limb14[21] = (words[ 9] >>  6);
    limb14[22] = (words[ 9] >> 20) | (words[10] << 12);
    limb14[23] = (words[10] >>  2);
    limb14[24] = (words[10] >> 16);
    limb14[25] = (words[10] >> 30) | (words[11] <<  2);
    limb14[26] = (words[11] >> 12);
    limb14[27] = (words[11] >> 26) | (words[12] <<  6);
    limb14[28] = (words[12] >>  8);
    limb14[29] = (words[12] >> 22) | (words[13] << 10);
    limb14[30] = (words[13] >>  4);
    limb14[31] = (words[13] >> 18);

    u16 mask = 0x3FFF;
    /* Clear the two top bits in all limbs */
    for (int i = 0; i < 2 * ED448_FE_LIMB_COUNT - 1; i++) {

        limb14[i] &= mask;
    }

    /* Check that the last byte is cleared (except for possibly the highest bit)
     * and that the rest of the bytes (which we have just parsed into limbs)
     * encode a canonical integer (i.e. smaller than p) */
    return fe448_is_canonical(r) & ( (buffer[56] & 0x7F) == 0 );
}
