#include <field_elements/field_elements_ed25519.h>
#include <utils/utils.h>

#if !FE3C_32BIT
    #error "Build system inconsistency detected! field_elements_ed25519_32.c in use despite FE3C_32BIT not being set"
#endif /* !FE3C_32BIT */

/* To reduce clutter, define a symbol for inline asm line termination */
#define _ "\n\t"

/* Field characteristic */
static const fe25519 fe25519_p = {
    0x7fff7fed, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff
};
/* Twice the field characteristic */
static const fe25519 fe25519_2p = {
    0x7fff7fda, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x17fff
};
/* Elliptic curve constant d = -121665/121666 */
const fe25519 ed25519_d = {
    0x26b278a3, 0x2f5a3728, 0x283b0ab7, 0x38051350, 0x73d11800, 0x3a0365dd, 0x7fce38cc, 0x36770adb, 0x5203
};
/* Additive identity in the field */
const fe25519 fe25519_zero = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
/* Multiplicative identity in the field */
const fe25519 fe25519_one = { 1, 0, 0, 0, 0, 0, 0, 0, 0 };
/* "Positive" (even) square root of -1 in the field */
const fe25519 fe25519_i = {
    0x141d20b0, 0x27706c9d, 0x25fc478c, 0x218c01ab, 0x77af272f, 0x680464f7, 0x3be132b4, 0x124013f0, 0x2b83
};

/**
 * @brief Uncondionally copy a field element
 * @param r Destination field element
 * @param a Source field element
 */
void fe25519_copy(fe25519 r, const fe25519 a) {

    if (a != r) {

        (void) memcpy(r, a, sizeof(fe25519));
    }
}

/**
 * @brief Do a constant-time equality check
 * @param[in] a The first element to check
 * @param[in] b The second element to check
 * @return 1 if a = b, 0 otherwise
 */
int fe25519_equal(const fe25519 a, const fe25519 b) {

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
        _ "movi.n %[ax],    %[limb_count]"
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
        : [limb_count] "i" (ED25519_FE_LIMB_COUNT)
        :
    );

    return equal;
}

/**
 * @brief Check if an elements is in its canonical form, i.e. less than the modulus
 * @param a Field element to check
 * @return 1 if a is in canonical form, 0 otherwise
 */
static int fe25519_is_canonical(const fe25519 a) {

    /* Iterators over the limbs */
    const fe_limb_type * ai = (fe_limb_type *) a;
    const fe_limb_type * pi = (fe_limb_type *) fe25519_p;
    /* Registers to store the values of limbs */
    u32 ax;
    u32 px;
    u32 borrow;

    /* Try subtracting p from a and check if borrow is set afterwards */
    asm volatile(
        /* Initialize the borrow to zero */
        _ "movi.n %[borrow], 0"

        /* Set up a hardware loop */
        _ "movi.n %[ax],     %[limb_count]"
        _ "loop   %[ax],     is_canonical.endloop"
    _ "is_canonical.startloop:"
        /* Load the operands' limbs. By design, loading with or without
         * sign extension should not make a difference.
         * TODO: Check if it has performance implications.*/
        _ "l16si  %[ax],     %[ai], 0"
        _ "l16si  %[px],     %[pi], 0"

        /* Subtract the low halves and the borrow (write the result to
         * ax as we discard it anyways since we only care about the borrow) */
        _ "sub    %[ax],     %[px], %[ax]"
        _ "sub    %[ax],     %[ax], %[borrow]"
        /* Extract the sign bit to identify the borrow */
        _ "extui  %[borrow], %[ax], 31, 1"

        /* Repeat for the high halves */
        _ "l16si  %[ax],     %[ai], 2"
        _ "l16si  %[px],     %[pi], 2"
        _ "sub    %[ax],     %[px], %[ax]"
        _ "sub    %[ax],     %[ax], %[borrow]"
        _ "extui  %[borrow], %[ax], 31, 1"

        /* Advance the iterators */
        _ "addi.n %[ai],     %[ai], 4"
        _ "addi.n %[pi],     %[pi], 4"

    _ "is_canonical.endloop:"

        : [ai] "+r" (ai),
          [pi] "+r" (pi),
          [ax] "=r" (ax),
          [px] "=r" (px),
          [borrow] "=r" (borrow)
        : [limb_count] "i" (ED25519_FE_LIMB_COUNT)
        : "memory"
    );

    /* If the element is exactly equal to the field characteristic,
     * there will be no borrow, but it's still not canonical */
    borrow |= fe25519_equal(a, fe25519_p);
    return 1 - borrow;
}

/**
 * @brief Do a constant-time conditional move
 * @param[out] r The result field element set to a conditionally or left alone
 * @param[in] a The element to conditionally move into r
 * @param[in] move Flag deciding on the branch, if set to 0, r ::= r, and if set to 1, r ::= a
 * @note If move is set to anything other than 0 or 1, the results are undefined
 */
void fe25519_conditional_move(fe25519 r, const fe25519 a, int move) {

    const fe_limb_type * ai = a;
    fe_limb_type * ri = r;
    u32 rx;
    u32 ax;
    asm volatile(
        /* Set up a hardware loop */
        _ "movi.n %[ax], %[limb_count]"
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
        : [limb_count] "i" (ED25519_FE_LIMB_COUNT),
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
void fe25519_weak_reduce(fe25519 r, const fe25519 a) {

    fe25519_copy(r, a);
    fe_limb_type * ri = r;
    u32 rx;
    u32 carry;

    asm volatile(

        /* Note that we use the identity
         *
         *        (x + y 2^255) = x + py + 19 y = x + 19 y (mod p)
         *
         * Note that 255 / 15 = 17, so whatever is in limb 17 now is the overflow
         * beyond the 2^255 boundary (y in the above identity). We shall multiply it
         * by 19 and add it back to result. */
        _ "l16ui  %[carry], %[ri], 4 * %[limb_count] - 2"
        _ "movi.n %[rx],    19"
        _ "mull   %[carry], %[rx], %[carry]"
        /* Clear the overflow limb in memory */
        _ "movi.n %[rx],    0"
        _ "s16i   %[rx],    %[ri], 4 * %[limb_count] - 2"

        _ "movi.n %[rx],    %[limb_count]"
        _ "loop   %[rx],    weak_reduce.endloop"
    _ "weak_reduce.startloop:"
        /* Load a 15-bit limb */
        _ "l16ui  %[rx],    %[ri], 0"
        /* Add the carry to it */
        _ "add.n  %[rx],    %[rx], %[carry]"
        /* Write the low 15 bits back to memory... */
        _ "extui  %[carry], %[rx], 0, 15"
        _ "s16i   %[carry], %[ri], 0"
        /* ...and put anything above 15 bits back into carry */
        _ "srli   %[carry], %[rx], 15"

        /* Repeat for the top half of the word */
        _ "l16ui  %[rx],    %[ri], 2"
        _ "add.n  %[rx],    %[rx], %[carry]"
        _ "extui  %[carry], %[rx], 0, 15"
        _ "s16i   %[carry], %[ri], 2"
        _ "srli   %[carry], %[rx], 15"

        /* Advance the iterator */
        _ "addi.n %[ri],    %[ri], 4"
    _ "weak_reduce.endloop:"

        : [ri] "+r" (ri),
          [rx] "=r" (rx),
          [carry] "=r" (carry)
        : [limb_count] "i" (ED25519_FE_LIMB_COUNT)
        : "memory"
    );
}

static inline void fe25519_sub_internal(fe25519 r, const fe25519 a, const fe25519 b) {

    /* This is an internal function that assumes a >= b and the subtraction
     * can be done directly. */

    /* Registers to store the values of limbs */
    u32 ax;
    u32 bx;
    u32 rx;
    u32 borrow;

    asm volatile(
        /* Initialize the borrow to zero */
        _ "movi.n %[borrow], 0"

        /* Set up a hardware loop */
        _ "movi.n %[ax],     %[limb_count]"
        _ "loop   %[ax],     sub_internal%=.endloop"
    _ "sub_internal%=.startloop:"
        /* Load the operands' limbs. By design, loading with or without
         * sign extension should not make a difference. */
        _ "l16ui  %[ax],     %[ai], 0"
        _ "l16ui  %[bx],     %[bi], 0"

        /* Subtract the low halves and the borrow */
        _ "sub    %[rx],     %[ax], %[bx]"
        _ "sub    %[rx],     %[rx], %[borrow]"
        /* Extract the sign bit to identify the borrow */
        _ "extui  %[borrow], %[rx], 31, 1"
        /* Write the low 15 bits of the result back to memory */
        _ "extui  %[rx],     %[rx], 0, 15"
        _ "s16i   %[rx],     %[ri], 0"

        /* Repeat for the high halves */
        _ "l16si  %[ax],     %[ai], 2"
        _ "l16si  %[bx],     %[bi], 2"
        _ "sub    %[rx],     %[ax], %[bx]"
        _ "sub    %[rx],     %[rx], %[borrow]"
        _ "extui  %[borrow], %[rx], 31, 1"
        _ "extui  %[rx],     %[rx], 0, 15"
        _ "s16i   %[rx],     %[ri], 2"

        /* Advance the iterators */
        _ "addi.n %[ai],     %[ai], 4"
        _ "addi.n %[bi],     %[bi], 4"
        _ "addi.n %[ri],     %[ri], 4"

    _ "sub_internal%=.endloop:"

        : [ai] "+r" (a),
          [bi] "+r" (b),
          [ri] "+r" (r),
          [ax] "=r" (ax),
          [bx] "=r" (bx),
          [rx] "=r" (rx),
          [borrow] "=r" (borrow)
        : [limb_count] "i" (ED25519_FE_LIMB_COUNT)
        : "memory"
    );
}

/**
 * @brief Do a strong modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note The result is guaranteed to be in canonical form, i.e. between 0 and p-1
 */
void fe25519_strong_reduce(fe25519 r, const fe25519 a) {

    /* Start by doing a weak reduction */
    fe25519_weak_reduce(r, a);
    /* Check canonicity of a */
    int canonical = fe25519_is_canonical(a);
    /* Compute a-p and conditionally use it as a result if a is larger than p */
    fe25519 t;
    fe25519_sub_internal(t, a, fe25519_p);
    fe25519_conditional_move(r, t, 1 - canonical);
}

/**
 * @brief Negate an element in the field
 * @param[out] r The result of negation
 * @param[in] a Element to be negated
 */
void fe25519_neg(fe25519 r, const fe25519 a) {

    /* TODO: Assert that the input is weakly reduced, i.e. smaller than 2p */
    fe25519_sub_internal(r, fe25519_2p, a);
}

/**
 * @brief Add two field elements
 * @param[out] r Result of the addition, i.e. the sum r = a + b
 * @param[in] a Operand
 * @param[in] b Operand
 */
void fe25519_add(fe25519 r, const fe25519 a, const fe25519 b) {

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
        _ "movi.n %[ax],    %[limb_count]"
        _ "loop   %[ax],    add.endloop"
    _ "add.startloop:"
        /* Load the operands' limbs. By design, loading with or without
         * sign extension should not make a difference.
         * TODO: Check if it has performance implications.*/
        _ "l16si  %[ax],    %[ai], 0"
        _ "l16si  %[bx],    %[bi], 0"

        /* Add the low halves */
        _ "add.n  %[rx],    %[ax], %[bx]"
        /* Add the carry */
        _ "add.n  %[rx],    %[rx], %[carry]"
        /* ...write back to memory the low 15 bits of the result... */
        _ "extui  %[carry], %[rx], 0, 15"
        _ "s16i   %[carry], %[ri], 0"
        /* ...and put any overflow back in carry */
        _ "srli   %[carry], %[rx], 15"

        /* Repeat for the high halves */
        _ "l16si  %[ax],    %[ai], 2"
        _ "l16si  %[bx],    %[bi], 2"
        _ "add.n  %[rx],    %[ax], %[bx]"
        _ "add.n  %[rx],    %[rx], %[carry]"
        _ "extui  %[carry], %[rx], 0, 15"
        _ "s16i   %[carry], %[ri], 2"
        _ "srli   %[carry], %[rx], 15"

        /* Advance the iterators */
        _ "addi.n %[ai],    %[ai], 4"
        _ "addi.n %[bi],    %[bi], 4"
        _ "addi.n %[ri],    %[ri], 4"
    _ "add.endloop:"

        : [ai] "+r" (ai),
          [bi] "+r" (bi),
          [ri] "+r" (ri),
          [ax] "=r" (ax),
          [bx] "=r" (bx),
          [rx] "=r" (rx),
          [carry] "=r" (carry)
        : [limb_count] "i" (ED25519_FE_LIMB_COUNT)
        : "memory"
    );
}

/**
 * @brief Subtract one field element from another
 * @param[out] r Result of the subtraction, i.e. the difference r = a - b
 * @param[in] a Minuend
 * @param[in] b Subtrahend
 */
void fe25519_sub(fe25519 r, const fe25519 a, const fe25519 b) {

    /* Negate b and then add that to a */
    fe25519 _b;
    /* TODO: Assert that b is weakly reduced, i.e. smaller than 2p */
    fe25519_sub_internal(_b, fe25519_2p, b);

    /* Iterators over the limbs */
    const fe_limb_type * ai = a;
    const fe_limb_type * bi = _b;
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
        _ "movi.n %[ax],    %[limb_count]"
        _ "loop   %[ax],    sub.endloop"
    _ "sub.startloop:"
        /* Load the operands' limbs. By design, loading with or without
         * sign extension should not make a difference.
         * TODO: Check if it has performance implications.*/
        _ "l16si %[ax],     %[ai], 0"
        _ "l16si %[bx],     %[bi], 0"

        /* Add the low halves */
        _ "add.n %[rx],     %[ax], %[bx]"
        /* Add the carry */
        _ "add.n %[rx],     %[rx], %[carry]"
        /* ...write back to memory the low 15 bits of the result... */
        _ "extui %[carry],  %[rx], 0, 15"
        _ "s16i  %[carry],  %[ri], 0"
        /* ...and put any overflow back in carry */
        _ "srli  %[carry],  %[rx], 15"

        /* Repeat for the high halves */
        _ "l16si %[ax],     %[ai], 2"
        _ "l16si %[bx],     %[bi], 2"
        _ "add.n %[rx],     %[ax], %[bx]"
        _ "add.n %[rx],     %[rx], %[carry]"
        _ "extui %[carry],  %[rx], 0, 15"
        _ "s16i  %[carry],  %[ri], 2"
        _ "srli  %[carry],  %[rx], 15"

        /* Advance the iterators */
        _ "addi.n %[ai],    %[ai], 4"
        _ "addi.n %[bi],    %[bi], 4"
        _ "addi.n %[ri],    %[ri], 4"
    _ "sub.endloop:"

        : [ai] "+r" (ai),
          [bi] "+r" (bi),
          [ri] "+r" (ri),
          [ax] "=r" (ax),
          [bx] "=r" (bx),
          [rx] "=r" (rx),
          [carry] "=r" (carry)
        : [limb_count] "i" (ED25519_FE_LIMB_COUNT)
        : "memory"
    );
}

/**
 * @brief Multiply two field elements
 * @param[out] r Result of the multiplication, i.e. the product r = a b
 * @param[in] a Operand
 * @param[in] b Operand
 */
void fe25519_mul(fe25519 r, const fe25519 a, const fe25519 b) {

    fe25519 _r;
    /* Constants */
    u32 nineteen;
    /* Iterators over the inputs */
    const fe_limb_type * ai;
    const fe_limb_type * bi;
    /* Temporary storage */
    u32 carrylo;
    u32 carryhi;
    u32 carry19lo;
    u32 carry19hi;
    u32 acccopylo;
    u32 acccopyhi;
    u32 temp;
    u32 rxeven;
    u32 rxodd;

/**
 * @brief Put the canonical part of the carry in carryhi | carrylo and clear the accumulator
 */
#define put_canonical_carry_aside() \
    _ "movi.n    %[carrylo], 0"     \
    _ "movi.n    %[carryhi], 0"     \
    _ "xsr.acclo %[carrylo]"        \
    _ "xsr.acchi %[carryhi]"
#define put_canonical_carry_aside_small() \
    _ "movi.n    %[carrylo], 0"           \
    _ "movi.n    %[carryhi], 0"           \
    _ "xsr.acclo %[carrylo]"

/**
 * @brief Combine the canonical and overflow carries, extract the low 15 bits, and shift the ACC
 * @param __rx Register where the low 15 bits of the carry should
 *             be stored
 * @note ACCHI is 8-bits wide, so after shifting right by 15 bits (even after multiplication by 19)
 *       we know it is going to be zero. For this reason we do not bother to shift acccopyhi by 15
 *       and do wsr.acchi, instead we do xsr.acchi (exchange special register) with a zero register
 *       (carry19hi) immediately.
 */
#define combine_carries(__rx)                                \
    /* Clear the high part of the accumulator immediately */ \
    _ "movi.n    %[carry19hi], 0"                            \
    _ "rsr.acclo %[carry19lo]"                               \
    _ "xsr.acchi %[carry19hi]"                               \
    /* Combine the "canonical" and "overflow" carries */     \
    _ "mull      %[acccopylo], %[carry19lo], %[nineteen]"    \
    _ "muluh     %[acccopyhi], %[carry19lo], %[nineteen]"    \
    /* Add the canonical carry */                            \
    _ "add.n     %[acccopyhi], %[acccopyhi], %[carryhi]"     \
    _ "add.n     %[acccopylo], %[acccopylo], %[carrylo]"     \
    /* Handle the addition overflow in constant time */      \
    _ "minu      %[temp],      %[acccopylo], %[carrylo]"     \
    _ "xor       %[temp],      %[temp],      %[carrylo]"     \
    _ "movi.n    %[carrylo],   1"                            \
    _ "movnez    %[temp],      %[carrylo],   %[temp]"        \
    _ "add.n     %[acccopyhi], %[acccopyhi], %[temp]"        \
    /* carry19hi is below 256, so ignore the high word */    \
    _ "mull      %[carry19hi], %[carry19hi], %[nineteen]"    \
    _ "add.n     %[acccopyhi], %[acccopyhi], %[carry19hi]"   \
    /* Commit the low 15 bits to a register */               \
    _ "extui     %[" #__rx "], %[acccopylo], 0, 15"          \
    /* Drop the low 15 bits just written from the ACC */     \
    _ "src       %[acccopylo], %[acccopyhi], %[acccopylo]"   \
    /* Write back to the accumulator */                      \
    _ "wsr.acclo %[acccopylo]"

/**
 * @brief Combine the rxodd and rxeven registers and store the result in memory
 * @param __idx Index (not offset in bytes!) into the result word array
 * @note It is faster to do two 16-bit stores than shift and OR the registers
 *       to have a single 32-bit store
 */
#define commit_to_memory(__idx)                      \
    _ "s16i %[rxeven], %[r],    4 * " #__idx         \
    _ "s16i %[rxodd],  %[r],    4 * " #__idx " + 2"

    asm volatile(
        /* Clear the accumulator */
        _ "movi.n    %[temp],     0"
        _ "wsr.acclo %[temp]"
        _ "wsr.acchi %[temp]"
        /* Initialize the shift amount register */
        _ "ssai      15"
        /* Initialize the constant 19 */
        _ "movi.n    %[nineteen], 19"

        /* Below we shall use the following notation: ai refers to the i'th
         * 15-bit limb of a. Note that ai is contained in a[i/2] and is in the
         * high part (high 16 bits) of a[i/2] if i % 2, otherwise it is in the
         * low part. The MAC16 DSP instruction set shall be utilized and to this
         * end the registers m0 and m1 shall be allocated to store limbs of a
         * and registers m2 and m3 shall be used to store limbs of b. */

        /* We shall make use of the following identity:
         *
         *     x + 2^255 y = x + (2^255 - 19) y + 19 y = x + 19 y (mod p)
         *
         * and the regular schoolbook multiplication, but bring all the "columns"
         * (corresponding to 15-bit limbs of the result) above the 2^255 boundary
         * down by multiplying by 19 (since they correspond to y in the above
         * identity). For brevity and efficiency we shall first add up the partial
         * products ai*bj of the "canonical" limbs (below the 2^255 boundary),
         * store that aside (in carryhi | carrylo), clear the accumulator, add up
         * the "overflow" limbs (above the 2^255 boundary), store that as well
         * (in carry19hi | carry19lo), multiply that by 19 and add it to the sum
         * of canonical limbs (carryhi | carrylo). That will correspond to the
         * final sum of a given column - low 15 bits of this sum will be committed
         * to memory and the rest will be put back in the accumulator as the carry.
         */

        /* To limit the number of memory accesses, we shall commit two limbs of the
         * result to memory at a time. To enable this we shall store the 15-bit
         * result of a column (see above) in rxeven or rxodd. Once both contain valid
         * results (rxeven contains the even limb and rxodd - the odd limb), we
         * shall write both to memory in one go. */

        /* Note the indentation style when using the MAC16 instruction set:
         *
         *     <MAC opcode>     <prefetch> <operands>
         */

        /* Also note that the ldinc/lddec operations _first_ increment/decrement
         * the address and then they make the memory access. For this reason we
         * always position the iterators one word ahead or behind the actual data
         * we want to read. */

        /* ------------------ Column i+j = 0 ------------------ */
            /* a0*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 1"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"
            _ "mula.dd.ll                  m0, m2"  /* a0*b0 */
            put_canonical_carry_aside_small()

            /* 19*(a0*b17 + a1*b16 + ... + a16*b1 + a17*b0) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a0*b17 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a1*b16 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a2*b15 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a3*b14 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a4*b13 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a5*b12 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a6*b11 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a7*b10 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a8*b9 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a9*b8 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a10*b7 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a11*b6 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a12*b5 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a13*b4 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a14*b3 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a15*b2 */

            _ "mula.dd.lh                  m0, m2"  /* a16*b1 */
            _ "mula.dd.hl                  m0, m2"  /* a17*b0 */

            combine_carries(rxeven)

        /* ------------------ Column i+j = 1 ------------------ */
            /* a0*b1 + a1*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 1"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"
            _ "mula.dd.lh                  m0, m2"  /* a0*b1 */
            _ "mula.dd.hl                  m0, m2"  /* a1*b0 */
            put_canonical_carry_aside()

            /* 19*(a1*b17 + a2*b16 + ... + a16*b2 + a17*b1) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.hh.ldinc m1, %[ai], m0, m2"  /* a1*b17 */
            _ "mula.dd.ll.lddec m3, %[bi], m1, m2"  /* a2*b16 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m3"  /* a3*b15 */
            _ "mula.dd.ll.lddec m2, %[bi], m0, m3"  /* a4*b14 */

            _ "mula.dd.hh.ldinc m1, %[ai], m0, m2"  /* a5*b13 */
            _ "mula.dd.ll.lddec m3, %[bi], m1, m2"  /* a6*b12 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m3"  /* a7*b11 */
            _ "mula.dd.ll.lddec m2, %[bi], m0, m3"  /* a8*b10 */

            _ "mula.dd.hh.ldinc m1, %[ai], m0, m2"  /* a9*b9 */
            _ "mula.dd.ll.lddec m3, %[bi], m1, m2"  /* a10*b8 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m3"  /* a11*b7 */
            _ "mula.dd.ll.lddec m2, %[bi], m0, m3"  /* a12*b6 */

            _ "mula.dd.hh.ldinc m1, %[ai], m0, m2"  /* a13*b5 */
            _ "mula.dd.ll.lddec m3, %[bi], m1, m2"  /* a14*b4 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m3"  /* a15*b3 */
            _ "mula.dd.ll.lddec m2, %[bi], m0, m3"  /* a16*b2 */

            _ "mula.dd.hh                  m0, m2"  /* a17*b1 */

            combine_carries(rxodd)
            commit_to_memory(0)

        /* ------------------ Column i+j = 2 ------------------ */
            /* a0*b2 + a1*b1 + a2*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 2"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"
            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a0*b2 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a1*b1 */
            _ "mula.dd.ll                  m1, m3"  /* a2*b0 */
            put_canonical_carry_aside()

            /* 19*(a2*b17 + a3*b16 + ... + a16*b3 + a17*b2) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a2*b17 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a3*b16 */
            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a4*b15 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a5*b14 */

            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a6*b13 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a7*b12 */
            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a8*b11 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a9*b10 */

            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a10*b9 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a11*b8 */
            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a12*b7 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a13*b6 */

            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a14*b5 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a15*b4 */
            _ "mula.dd.lh                  m0, m3"  /* a16*b3 */
            _ "mula.dd.hl                  m0, m3"  /* a17*b2 */

            combine_carries(rxeven)

        /* ------------------ Column i+j = 3 ------------------ */
            /* a0*b3 + a1*b2 + a2*b1 + a3*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 2"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a0*b3 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a1*b2 */
            _ "mula.dd.lh                  m1, m3"  /* a2*b1 */
            _ "mula.dd.hl                  m1, m3"  /* a3*b0 */

            put_canonical_carry_aside()

            /* 19*(a3*b17 + a4*b16 + ... + a16*b4 + a17*b3) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a3*b17 */
            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a4*b16 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a5*b15 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a6*b14 */

            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a7*b13 */
            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a8*b12 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a9*b11 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a10*b10 */

            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a11*b9 */
            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a12*b8 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a13*b7 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a14*b6 */

            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a15*b5 */
            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a16*b4 */
            _ "mula.dd.hh                  m0, m3"  /* a17*b3 */

            combine_carries(rxodd)
            commit_to_memory(1)

        /* ------------------ Column i+j = 4 ------------------ */
            /* a0*b4 + a1*b3 + a2*b2 + a3*b1 + a4*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 3"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"
            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a0*b4 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a1*b3 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a2*b2 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a3*b1 */
            _ "mula.dd.ll                  m0, m2"  /* a4*b0 */

            put_canonical_carry_aside()

            /* 19*(a4*b17 + a4*b16 + ... + a16*b4 + a17*b4) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a4*b17 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a5*b16 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a6*b15 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a7*b14 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a8*b13 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a9*b12 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a10*b11 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a11*b10 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a12*b9 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a13*b8 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a14*b7 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a15*b6 */

            _ "mula.dd.lh                  m0, m2"  /* a16*b5 */
            _ "mula.dd.hl                  m0, m2"  /* a17*b4 */

            combine_carries(rxeven)

        /* ------------------ Column i+j = 5 ------------------ */
            /* a0*b5 + a1*b4 + ... + a4*b1 + a5*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 3"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a0*b5 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a1*b4 */
            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a2*b3 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a3*b2 */
            _ "mula.dd.lh                  m0, m2"  /* a4*b1 */
            _ "mula.dd.hl                  m0, m2"  /* a5*b0 */

            put_canonical_carry_aside()

            /* 19*(a5*b17 + a6*b16 + ... + a16*b6 + a17*b5) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.hh.ldinc m1, %[ai], m0, m2"  /* a5*b17 */
            _ "mula.dd.ll.lddec m3, %[bi], m1, m2"  /* a6*b16 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m3"  /* a7*b15 */
            _ "mula.dd.ll.lddec m2, %[bi], m0, m3"  /* a8*b14 */

            _ "mula.dd.hh.ldinc m1, %[ai], m0, m2"  /* a9*b13 */
            _ "mula.dd.ll.lddec m3, %[bi], m1, m2"  /* a10*b12 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m3"  /* a11*b11 */
            _ "mula.dd.ll.lddec m2, %[bi], m0, m3"  /* a12*b10 */

            _ "mula.dd.hh.ldinc m1, %[ai], m0, m2"  /* a13*b9 */
            _ "mula.dd.ll.lddec m3, %[bi], m1, m2"  /* a14*b8 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m3"  /* a15*b7 */
            _ "mula.dd.ll.lddec m2, %[bi], m0, m3"  /* a16*b6 */

            _ "mula.dd.hh                  m0, m2"  /* a17*b5 */

            combine_carries(rxodd)
            commit_to_memory(2)


        /* ------------------ Column i+j = 6 ------------------ */
            /* a0*b6 + a1*b5 + ... + a5*b1 + a6*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 4"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a0*b6 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a1*b5 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a2*b4 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a3*b3 */

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a4*b2 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a5*b1 */
            _ "mula.dd.ll                  m1, m3"  /* a6*b0 */

            put_canonical_carry_aside()

            /* 19*(a6*b17 + a7*b16 + ... + a16*b7 + a17*b6) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a6*b17 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a7*b16 */
            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a8*b15 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a9*b14 */

            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a10*b13 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a11*b12 */
            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a12*b11 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a13*b10 */

            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a14*b9 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a15*b8 */
            _ "mula.dd.lh                  m0, m3"  /* a16*b7 */
            _ "mula.dd.hl                  m0, m3"  /* a17*b6 */

            combine_carries(rxeven)

        /* ------------------ Column i+j = 7 ------------------ */
            /* a0*b7 + a1*b6 + ... + a6*b1 + a7*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 4"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a0*b7 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a1*b6 */
            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a2*b5 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a3*b4 */

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a4*b3 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a5*b2 */
            _ "mula.dd.lh                  m1, m3"  /* a6*b1 */
            _ "mula.dd.hl                  m1, m3"  /* a7*b0 */

            put_canonical_carry_aside()

            /* 19*(a7*b17 + a8*b16 + ... + a16*b8 + a17*b7) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a7*b17 */
            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a8*b16 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a9*b15 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a10*b14 */

            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a11*b13 */
            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a12*b12 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a13*b11 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a14*b10 */

            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a15*b9 */
            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a16*b8 */
            _ "mula.dd.hh                  m0, m3"  /* a17*b7 */

            combine_carries(rxodd)
            commit_to_memory(3)


        /* ------------------ Column i+j = 8 ------------------ */
            /* a0*b8 + a1*b7 + ... + a7*b1 + a8*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 5"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a0*b8 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a1*b7 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a2*b6 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a3*b5 */
            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a4*b4 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a5*b3 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a6*b2 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a7*b1 */
            _ "mula.dd.ll                  m0, m2"  /* a8*b0 */

            put_canonical_carry_aside()

            /* 19*(a8*b17 + a9*b16 + ... + a16*b9 + a17*b8) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a8*b17 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a9*b16 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a10*b15 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a11*b14 */

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a12*b13 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a13*b12 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a14*b11 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a15*b10 */

            _ "mula.dd.lh                  m0, m2"  /* a16*b9 */
            _ "mula.dd.hl                  m0, m2"  /* a17*b8 */

            combine_carries(rxeven)


        /* ------------------ Column i+j = 9 ------------------ */
            /* a0*b9 + a1*b8 + ... + a8*b1 + a9*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 5"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a0*b9 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a1*b8 */
            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a2*b7 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a3*b6 */

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a4*b5 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a5*b4 */
            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a6*b3 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a7*b2 */

            _ "mula.dd.lh                  m0, m2"  /* a8*b1 */
            _ "mula.dd.hl                  m0, m2"  /* a9*b0 */

            put_canonical_carry_aside()

            /* 19*(a9*b17 + a10*b16 + ... + a16*b10 + a17*b9) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.hh.ldinc m1, %[ai], m0, m2"  /* a9*b17 */
            _ "mula.dd.ll.lddec m3, %[bi], m1, m2"  /* a10*b16 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m3"  /* a11*b15 */
            _ "mula.dd.ll.lddec m2, %[bi], m0, m3"  /* a12*b14 */

            _ "mula.dd.hh.ldinc m1, %[ai], m0, m2"  /* a13*b13 */
            _ "mula.dd.ll.lddec m3, %[bi], m1, m2"  /* a14*b12 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m3"  /* a15*b11 */
            _ "mula.dd.ll.lddec m2, %[bi], m0, m3"  /* a16*b10 */

            _ "mula.dd.hh                  m0, m2"  /* a17*b9 */

            combine_carries(rxodd)
            commit_to_memory(4)


        /* ------------------ Column i+j = 10 ------------------ */
            /* a0*b10 + a1*b9 + ... + a9*b1 + a10*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 6"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a0*b10 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a1*b9 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a2*b8 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a3*b7 */

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a4*b6 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a5*b5 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a6*b4 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a7*b3 */

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a8*b2 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a9*b1 */
            _ "mula.dd.ll                  m1, m3"  /* a10*b0 */

            put_canonical_carry_aside()

            /* 19*(a10*b17 + a11*b16 + ... + a16*b11 + a17*b10) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a10*b17 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a11*b16 */
            _ "mula.dd.lh.ldinc m1, %[ai], m0, m3"  /* a12*b15 */
            _ "mula.dd.hl.lddec m2, %[bi], m0, m3"  /* a13*b14 */

            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a14*b13 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a15*b12 */
            _ "mula.dd.lh                  m0, m3"  /* a16*b11 */
            _ "mula.dd.hl                  m0, m3"  /* a17*b10 */

            combine_carries(rxeven)

        /* ------------------ Column i+j = 11 ------------------ */
            /* a0*b11 + a1*b10 + ... + a10*b1 + a11*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 6"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a0*b11 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a1*b10 */
            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a2*b9 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a3*b8 */

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a4*b7 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a5*b6 */
            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a6*b5 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a7*b4 */

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a8*b3 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a9*b2 */
            _ "mula.dd.lh                  m1, m3"  /* a10*b1 */
            _ "mula.dd.hl                  m1, m3"  /* a11*b0 */

            put_canonical_carry_aside()

            /* 19*(a11*b17 + a12*b16 + ... + a16*b12 + a17*b11) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a11*b17 */
            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a12*b16 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a13*b15 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a14*b14 */

            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a15*b13 */
            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a16*b12 */
            _ "mula.dd.hh                  m0, m3"  /* a17*b11 */

            combine_carries(rxodd)
            commit_to_memory(5)


        /* ------------------ Column i+j = 12 ------------------ */
            /* a0*b12 + a1*b11 + ... + a11*b1 + a12*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 7"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a0*b12 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a1*b11 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a2*b10 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a3*b9 */

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a4*b8 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a5*b7 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a6*b6 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a7*b5 */

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a8*b4 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a9*b3 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a10*b2 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a11*b1 */
            _ "mula.dd.ll                  m0, m2"  /* a12*b0 */

            put_canonical_carry_aside()

            /* 19*(a12*b17 + a13*b16 + ... + a16*b13 + a17*b12) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh.ldinc m1, %[ai], m0, m2"  /* a12*b17 */
            _ "mula.dd.hl.lddec m3, %[bi], m0, m2"  /* a13*b16 */
            _ "mula.dd.lh.ldinc m0, %[ai], m1, m3"  /* a14*b15 */
            _ "mula.dd.hl.lddec m2, %[bi], m1, m3"  /* a15*b14 */

            _ "mula.dd.lh                  m0, m2"  /* a16*b13 */
            _ "mula.dd.hl                  m0, m2"  /* a17*b12 */

            combine_carries(rxeven)

        /* ------------------ Column i+j = 13 ------------------ */
            /* a0*b13 + a1*b12 + ... + a12*b1 + a13*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 7"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a0*b13 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a1*b12 */
            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a2*b11 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a3*b10 */

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a4*b9 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a5*b8 */
            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a6*b7 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a7*b6 */

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a8*b5 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a9*b4 */
            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a10*b3 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a11*b2 */

            _ "mula.dd.lh                  m0, m2"  /* a12*b1 */
            _ "mula.dd.hl                  m0, m2"  /* a13*b0 */

            put_canonical_carry_aside()

            /* 19*(a13*b17 + a14*b16 + ... + a16*b14 + a17*b13) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.hh.ldinc m1, %[ai], m0, m2"  /* a13*b17 */
            _ "mula.dd.ll.lddec m3, %[bi], m1, m2"  /* a14*b16 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m3"  /* a15*b15 */
            _ "mula.dd.ll.lddec m2, %[bi], m0, m3"  /* a16*b14 */

            _ "mula.dd.hh                  m0, m2"  /* a17*b13 */

            combine_carries(rxodd)
            commit_to_memory(6)


        /* ------------------ Column i+j = 14 ------------------ */
            /* a0*b14 + a1*b13 + ... + a13*b1 + a14*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 8"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a0*b14 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a1*b13 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a2*b12 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a3*b11 */

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a4*b10 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a5*b9 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a6*b8 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a7*b7 */

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a8*b6 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a9*b5 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a10*b4 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a11*b3 */

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a12*b2 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a13*b1 */
            _ "mula.dd.ll                  m1, m3"  /* a14*b0 */

            put_canonical_carry_aside()

            /* 19*(a14*b17 + a15*b16 + a16*b15 + a17*b14) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh.ldinc m0, %[ai], m1, m2"  /* a14*b17 */
            _ "mula.dd.hl.lddec m3, %[bi], m1, m2"  /* a15*b16 */
            _ "mula.dd.lh                  m0, m3"  /* a16*b15 */
            _ "mula.dd.hl                  m0, m3"  /* a17*b14 */

            combine_carries(rxeven)

        /* ------------------ Column i+j = 15 ------------------ */
            /* a0*b15 + a1*b14 + ... + a14*b1 + a15*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 8"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a0*b15 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a1*b14 */
            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a2*b13 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a3*b12 */

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a4*b11 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a5*b10 */
            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a6*b9 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a7*b8 */

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a8*b7 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a9*b6 */
            _ "mula.dd.lh.lddec m2, %[bi], m1, m3"  /* a10*b5 */
            _ "mula.dd.hl.ldinc m0, %[ai], m1, m3"  /* a11*b4 */

            _ "mula.dd.lh.lddec m3, %[bi], m0, m2"  /* a12*b3 */
            _ "mula.dd.hl.ldinc m1, %[ai], m0, m2"  /* a13*b2 */
            _ "mula.dd.lh                  m1, m3"  /* a14*b1 */
            _ "mula.dd.hl                  m1, m3"  /* a15*b0 */

            put_canonical_carry_aside()

            /* 19*(a15*b17 + a16*b16 + a17*b15) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a15*b17 */
            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a16*b16 */
            _ "mula.dd.hh                  m0, m3"  /* a17*b15 */

            combine_carries(rxodd)
            commit_to_memory(7)

        /* ------------------ Column i+j = 16 ------------------ */
            /* a0*b16 + a1*b15 + ... + a15*b1 + a16*b0 */
            _ "addi.n    %[ai], %[a], -4"
            _ "addi.n    %[bi], %[b],  4 * 9"
            _ "ldinc m0, %[ai]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a0*b16 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a1*b15 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a2*b14 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a3*b13 */

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a4*b12 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a5*b11 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a6*b10 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a7*b9 */

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a8*b8 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a9*b7 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a10*b6 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a11*b5 */

            _ "mula.dd.ll.lddec m3, %[bi], m0, m2"  /* a12*b4 */
            _ "mula.dd.hh.ldinc m1, %[ai], m0, m3"  /* a13*b3 */
            _ "mula.dd.ll.lddec m2, %[bi], m1, m3"  /* a14*b2 */
            _ "mula.dd.hh.ldinc m0, %[ai], m1, m2"  /* a15*b1 */
            _ "mula.dd.ll                  m0, m2"  /* a16*b0 */

            put_canonical_carry_aside()

            /* 19*(a16*b17 + a17*b16) */
            _ "addi.n    %[bi], %[b], 4 * %[limb_count]"
            _ "lddec m2, %[bi]"

            _ "mula.dd.lh                  m0, m2"  /* a16*b17 */
            _ "mula.dd.hl                  m0, m2"  /* a17*b16 */

            combine_carries(rxeven)
            /* Commit limb 16 to memory at the same time clearing
             * limb 17 by making a 32-bit write */
            _ "s32i %[rxeven], %[r], 4 * 8"

        /* ------------------ Column i+j = 17 ------------------ */
            /* Note that limb 17 is already above the 2^255 boundary,
             * and the sum (a0*b17 + a1*b16 +... + a16*b1 + a17*b0)
             * has already been brought down and added to column i+j=0.
             * All that's left to do is to bring a17*b17 down by 255 bits
             * twice, i.e. multiply it by 19 twice. */

            /* Use address registers for this to reduce the number of
             * unproductive read/writes from/to ACC. Put a17 in carrylo
             * and b17 in carryhi since we don't need those anymore. */
            _ "l16ui %[carrylo],   %[a],       4 * %[limb_count] - 4 + 2"
            _ "l16ui %[carryhi],   %[b],       4 * %[limb_count] - 4 + 2"
            _ "mull  %[carrylo],   %[carrylo], %[nineteen]"
            _ "mull  %[carry19lo], %[carrylo], %[carryhi]"
            /* Note that for reasonably reduced inputs (TODO: make this
             * bound tight), the high word must be zero at this point */

            /* Now 19*a17*b17 must be added to the accumulator and the result
             * multiplied by 19. This will be the final overflow above the 2^255
             * boundary that we have brought down and shall distribute among
             * the low 16 limbs. Note that at this point the high part of the
             * accumulator must be zero. */
            _ "rsr.acclo %[acccopylo]"
            /* Add 19*a17*b17 kept in carry19lo to the accumulator */
            _ "add       %[carrylo],   %[acccopylo], %[carry19lo]"
            /* Note that no overflow could have occurred for good inputs */

            /* At this point carrylo contains the total overflow above the
             * 2^255 boundary. Multiply by 19 one last time. */
            _ "mull      %[carry19lo], %[carrylo],   %[nineteen]"

            /* Do one final (weak) reduction in a hardware loop. Note that
             * the carry at all points below will fit in 32 bits for good
             * inputs. */
            /* Use ai to iterate over r */
            _ "mov.n     %[ai],        %[r]"
            _ "movi.n    %[temp],      %[limb_count]"
            _ "loop      %[temp],      mul.endreduce"
        _ "mul.startreduce:"

            /* TODO: Check if it is not faster to load both limbs as u32
             * and then do shifts and masks */
            _ "l16ui     %[rxeven],    %[ai],        0"
            _ "l16ui     %[rxodd],     %[ai],        2"

            /* Add the low limb to the accumulator */
            _ "add.n     %[carry19lo], %[carry19lo], %[rxeven]"
            /* Extract low 15 bits of the accumulator */
            _ "extui     %[rxeven],    %[carry19lo], 0, 15"
            /* Write back to memory */
            _ "s16i      %[rxeven],    %[ai], 0"
            /* Shift the accumulator */
            _ "srli      %[carry19lo], %[carry19lo], 15"

            /* Add the high limb to the accumulator */
            _ "add.n     %[carry19lo], %[carry19lo], %[rxodd]"
            /* Extract low 15 bits of the accumulator */
            _ "extui     %[rxodd],     %[carry19lo], 0, 15"
            /* Write back to memory */
            _ "s16i      %[rxodd],     %[ai],        2"
            /* Shift the accumulator */
            _ "srli      %[carry19lo], %[carry19lo], 15"

            /* Move on to the next pair of limbs */
            _ "addi.n    %[ai],        %[ai],        4"

        _ "mul.endreduce:"

        : [nineteen]   "=&r" (nineteen),
          [ai]         "=&r" (ai),
          [bi]         "=&r" (bi),
          [carrylo]    "=&r" (carrylo),
          [carryhi]    "=&r" (carryhi),
          [carry19lo]  "=&r" (carry19lo),
          [carry19hi]  "=&r" (carry19hi),
          [acccopylo]  "=&r" (acccopylo),
          [acccopyhi]  "=&r" (acccopyhi),
          [temp]       "=&r" (temp),
          [rxeven]     "=&r" (rxeven),
          [rxodd]      "=&r" (rxodd)
        : [limb_count] "i"   (ED25519_FE_LIMB_COUNT),
          [a] "r" (a),
          [b] "r" (b),
          [r] "r" (_r)
        : "memory", "acc"
    );

    fe25519_copy(r, _r);
}

/**
 * @brief Multiply an element by itself
 * @param[out] r Result of the squaring, i.e. the product r = a a
 * @param[in] a Field element to square
 */
void fe25519_square(fe25519 r, const fe25519 a) {

#if !FE3C_FAST_SQUARING
    fe25519_mul(r, a, a);
#else
    /* TODO: Implement optimized squaring */
    fe25519_mul(r, a, a);
#endif /* !FE3C_FAST_SQUARING */
}

/**
 * @brief Encode a field element according to RFC 8032
 * @param[out] buffer Output buffer for the encoded field element
 * @param[in] a Field element to encode
 */
void fe25519_encode(u8 * buffer, fe25519 a) {

    /* Canonicalize the element first */
    fe25519_strong_reduce(a, a);

    /* Use word-based memory accesses since encoding is little-endian
     * and so is the CPU */
    u32 * words = (u32 *) buffer;
    /* Iterate over the 15-bit limbs instead of the 32-bit words */
    u16 * limb15 = (u16 *) a;

    /* In the first word of the output buffer we shall fit the first
     * two 15-bit limbs as well as two bits of the third limb */
    words[0] = (limb15[ 0] >>  0) | (limb15[ 1] << 15) | ( (limb15[ 2] &    0x3) << 30);
    /* 13 bits of limb15[2], 15 bits of limb15[3], 4 bits of limb15[4] */
    words[1] = (limb15[ 2] >>  2) | (limb15[ 3] << 13) | ( (limb15[ 4] &    0xF) << 28);
    /* 11 bits of limb15[4], 15 bits of limb15[5], 6 bits of limb15[6] */
    words[2] = (limb15[ 4] >>  4) | (limb15[ 5] << 11) | ( (limb15[ 6] &   0x3F) << 26);
    /* 9 bits of limb15[6], 15 bits of limb15[7], 8 bits of limb15[8] */
    words[3] = (limb15[ 6] >>  6) | (limb15[ 7] <<  9) | ( (limb15[ 8] &   0xFF) << 24);
    /* 7 bits of limb15[8], 15 bits of limb15[9], 10 bits of limb15[10] */
    words[4] = (limb15[ 8] >>  8) | (limb15[ 9] <<  7) | ( (limb15[10] &  0x3FF) << 22);
    /* 5 bits of limb15[10], 15 bits of limb15[11], 12 bits of limb15[12] */
    words[5] = (limb15[10] >> 10) | (limb15[11] <<  5) | ( (limb15[12] &  0xFFF) << 20);
    /* 3 bits of limb15[12], 15 bits of limb15[13], 14 bits of limb15[14] */
    words[6] = (limb15[12] >> 12) | (limb15[13] <<  3) | ( (limb15[14] & 0x3FFF) << 18);
    /* 1 bit of limb15[14], 15 bits of limb15[15], 15 bits of limb15[16] */
    words[7] = (limb15[14] >> 14) | (limb15[15] <<  1) | ( (limb15[16] & 0xFFFF) << 16);
}

/**
 * @brief Decode a field element according to RFC 8032
 * @param[out] r Decoded field element
 * @param[out] buffer Encoding of the field element
 * @return 1 if decoding succeeded, 0 otherwise
 */
__attribute__((warn_unused_result))
int fe25519_decode(fe25519 r, const u8 * buffer) {

    /* Use word-based memory accesses since encoding is little-endian
     * and so is the CPU */
    const u32 * words = (const u32 *) buffer;
    /* Iterate over the 15-bit limbs instead of the 32-bit words */
    u16 * limb15 = (u16 *) r;

    /* First do the 16-bit reads and then clear the top bits in one go
     * for code clarity (at the cost of worse cache usage, though) */

    /* Read in 15 bits from the first word */
    limb15[ 0] = words[0];
    /* Read in the next 15 bits from the same word */
    limb15[ 1] = words[0] >> 15;
    /* Read two most significant bits from the first word and 13 bits from the next one */
    limb15[ 2] = (words[0] >> 30) | (words[1] <<  2);
    /* Words don't come easy */
    limb15[ 3] = (words[1] >> 13);
    limb15[ 4] = (words[1] >> 28) | (words[2] <<  4);
    limb15[ 5] = (words[2] >> 11);
    limb15[ 6] = (words[2] >> 26) | (words[3] <<  6);
    limb15[ 7] = (words[3] >>  9);
    limb15[ 8] = (words[3] >> 24) | (words[4] <<  8);
    limb15[ 9] = (words[4] >>  7);
    limb15[10] = (words[4] >> 22) | (words[5] << 10);
    limb15[11] = (words[5] >>  5);
    limb15[12] = (words[5] >> 20) | (words[6] << 12);
    limb15[13] = (words[6] >>  3);
    limb15[14] = (words[6] >> 18) | (words[7] << 14);
    limb15[15] = (words[7] >>  1);
    limb15[16] = (words[7] >> 16);
    /* Note that MSB of words[7] has to be ignored as per RFC 8032 (it may encode
     * sign of the y coordinate in EC point encoding). We shall ignore it in the
     * loop below where we mask out bit 15 in each limb. */

    /* The top limb is always set to zero as it is only used internally to store
     * overflow above p */
    limb15[17] = 0;

    u16 mask = 0x7FFF;
    /* Clear the top bit in all limbs */
    for (int i = 0; i < 2 * ED25519_FE_LIMB_COUNT - 1; i++) {

        limb15[i] &= mask;
    }

    /* Check canonicity */
    return fe25519_is_canonical(r);
}
