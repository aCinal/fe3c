#include <scalars/scalars.h>
#include <utils/utils.h>

#if !FE3C_32BIT
    #error "Build system inconsistency detected! scalars_ed25519_32.c in use despite FE3C_32BIT not being set"
#endif /* !FE3C_32BIT */

/* To reduce clutter, define a symbol for inline asm line termination */
#define _ "\n\t"

static const u8 group_order[] = {
    0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58,
    0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10
};

static int ed25519_scalar_is_canonical(const u8 * s) {

    /* The scalar is encoded on 32 bytes so we should
     * be able to iterate over it using a u16 iterator. */
    const u16 * si = (const u16 *) s;
    const u16 * qi = (const u16 *) group_order;
    u32 borrow = 0;
    /* Store a flag in notq indicating whether the scalar is equal
     * to the group order. If this is the case, borrow will be zero
     * after the loop, despite the scalar not being canonical. */
    u32 notq = 0;

    /* Try subtracting s from the group order q and check if borrow is set afterwards */
    for (int i = 0; i < 16; i++) {

        u32 diff = qi[i] - si[i];
        /* If the subtraction was non-zero, then s != q,
         * let the notq variable reflect that */
        asm volatile("movnez %[notq], %[diff], %[diff]" : [notq] "+&r" (notq) : [diff] "r" (diff) : );
        diff -= borrow;
        /* Extract the sign bit to identify the borrow */
        asm volatile("extui %[borrow], %[diff], 31, 1" : [borrow] "+&r" (borrow) : [diff] "r" (diff) : );
    }

    /* Normalize the equality indicator */
    asm volatile(
        _ "movi.n %[si],   1"
        _ "movnez %[notq], %[si], %[notq]"

        : [notq] "+r" (notq),
          [si]   "=r" (si)
        :
        :
    );

    /* A scalar is canonical if and only if there was
     * no borrow at the end of the loop, i.e. s <= q, and
     * notq is set, i.e. s != q */
    return ~borrow & notq;
}

static void ed25519_scalar_add_internal(u8 * r, const u8 * a, const u8 * b, int mock) {

    /* Note that we could do subtraction with u16 granularity since we use packed-radix
     * representation, thus better utilizing the hardware adder and halving the number
     * of iterations. It does not seem to have any bearing on performance, however, so
     * for code clarity we make byte-based accesses and additions. */
    u32 carry = 0;
    for (int i = 0; i < sizeof(group_order); i++) {

        u32 first = a[i];
        u32 second = b[i];
        /* If mocked addition, set the the second operand to zero */
        asm volatile("movnez %0, %1, %2" : "+&r" (second) : "r" (carry), "r" (mock) : );
        carry += first + second;
        r[i] = (u8) carry;
        carry >>= 8;
    }
}

static void ed25519_scalar_reduce(u8 * s) {

    /* Let L be the group order and c = L - 2^252. Then we have:
     *
     *                     2^256 = -2^4 c (mod L)
     *
     * and so bytes 32 and beyond of the input scalar can be brought down
     * below the 2^256 boundary after multiplying by -16c. Note that c
     * corresponds to the low 16 bytes of group_order.
     */

    /* Since we are subtracting 16c we may get negative digits in the intermediate
     * representations. Use signed 16-bit values to facilitate this. */
    i16 t[49];
    i32 carry = 0;
    for (int k = 0; k < 16; k++) {

        carry += s[k];
        for (int i = 0; i <= k; i++) {

            int j = k - i;
            carry -= s[32 + i] * ( (i32) group_order[j] << 4 );
        }
        t[k] = carry & 0xFF;
        carry >>= 8;
    }

    for (int k = 16; k < 32; k++) {

        carry += s[k];
        for (int i = k + 1 - 16; i <= k; i++) {

            int j = k - i;
            carry -= s[32 + i] * ( (i32) group_order[j] << 4 );
        }
        t[k] = carry & 0xFF;
        carry >>= 8;
    }

    for (int k = 32; k < 48; k++) {

        for (int i = k + 1 - 16; i < 32; i++) {

            int j = k - i;
            carry -= s[32 + i] * ( (i32) group_order[j] << 4 );
        }
        t[k] = carry & 0xFF;
        carry >>= 8;
    }
    t[48] = carry;
    carry = 0;

    /* We have successfully gone down to 49 signed bytes. Do another reduction,
     * bringing us down to 16 + (49 - 32) = 33 (signed) bytes. */
    for (int k = 0; k < 16; k++) {

        carry += t[k];
        for (int i = 0; i <= k; i++) {

            int j = k - i;
            carry -= t[32 + i] * ( (i32) group_order[j] << 4 );
        }
        t[k] = carry & 0xFF;
        carry >>= 8;
    }

    for (int k = 16; k < 17; k++) {

        carry += t[k];
        for (int i = k + 1 - 16; i <= k; i++) {

            int j = k - i;
            carry -= t[32 + i] * ( (i32) group_order[j] << 4 );
        }
        t[k] = carry & 0xFF;
        carry >>= 8;
    }

    for (int k = 17; k < 32; k++) {

        carry += t[k];
        for (int i = k + 1 - 16; i < 17; i++) {

            int j = k - i;
            carry -= t[32 + i] * ( (i32) group_order[j] << 4 );
        }
        t[k] = carry & 0xFF;
        carry >>= 8;
    }
    t[32] = carry;
    carry = 0;

    /* Do a more careful reduction, by employing the identity:
     *
     *                 2^252 = -c (mod L)
     *
     * and combine it with a writeback to the s buffer.
     */
    i32 overflow = (t[32] << 4) | (t[31] >> 4);
    t[31] &= 0x0f;
    for (int k = 0; k < 16; k++) {

        carry += t[k];
        carry -= overflow * group_order[k];
        s[k] = carry & 0xFF;
        carry >>= 8;
    }
    /* Propagate the carry */
    for (int k = 16; k < 32; k++) {

        carry += t[k];
        s[k] = carry & 0xFF;
        carry >>= 8;
    }
    FE3C_SANITY_CHECK(carry <= 0, NULL);
    carry = -carry;
    /* At this point the scalar is in the range [-L, L). Conditionally add
     * the group order and use that as a result if underflow occurred. */
    ed25519_scalar_add_internal(s, s, group_order, 1 - carry);
    purge_secrets(t, sizeof(t));
}

static void ed25519_scalars_muladd(u8 * r, const u8 * a, const u8 * b, const u8 * c) {

    u8 t[64];
    u32 carry = 0;
    for (int k = 0; k < 32; k++) {

        carry += c[k];
        /* Convolve a and b */
        for (int i = 0; i <= k; i++) {

            int j = k - i;
            carry += a[i] * b[j];
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }

    for (int k = 32; k < 64; k++) {

        /* Convolve a and b */
        for (int i = k + 1 - 32; i < 32; i++) {

            int j = k - i;
            carry += a[i] * b[j];
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }

    ed25519_scalar_reduce(t);
    /* Copy to the output buffer */
    (void) memcpy(r, t, 32);
    /* Lower half of t is public (response of the signature), but
     * the top half needs to be purged as it reveals information
     * about the long-term secret key and the secret ephemeral. */
    purge_secrets(t, sizeof(t));
}

const scalar_ops ed25519_scalar_ops = {
    .is_canonical = ed25519_scalar_is_canonical,
    .reduce = ed25519_scalar_reduce,
    .muladd = ed25519_scalars_muladd
};
