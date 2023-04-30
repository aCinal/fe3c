#include <scalars/scalars.h>
#include <utils/utils.h>

#if !FE3C_32BIT
    #error "Build system inconsistency detected! scalars_ed448_32.c in use despite FE3C_32BIT not being set"
#endif /* !FE3C_32BIT */

/* To reduce clutter, define a symbol for inline asm line termination */
#define _ "\n\t"

static const u8 group_order[] = {
    0xf3, 0x44, 0x58, 0xab, 0x92, 0xc2, 0x78, 0x23,
    0x55, 0x8f, 0xc5, 0x8d, 0x72, 0xc2, 0x6c, 0x21,
    0x90, 0x36, 0xd6, 0xae, 0x49, 0xdb, 0x4e, 0xc4,
    0xe9, 0x23, 0xca, 0x7c, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f,
    0x00
};

/* The c part of the group order L = 2^446 - c */
static const u8 group_order_crandall_tail[] = {
    0x0d, 0xbb, 0xa7, 0x54, 0x6d, 0x3d, 0x87, 0xdc,
    0xaa, 0x70, 0x3a, 0x72, 0x8d, 0x3d, 0x93, 0xde,
    0x6f, 0xc9, 0x29, 0x51, 0xb6, 0x24, 0xb1, 0x3b,
    0x16, 0xdc, 0x35, 0x83
};

static int ed448_scalar_is_canonical(const u8 * s) {

    u32 temp;
    u32 borrow = 0;
    /* Store a flag in notq indicating whether the scalar is equal
     * to the group order. If this is the case, borrow will be zero
     * after the loop, despite the scalar not being canonical. */
    u32 notq = 0;

    /* Try subtracting s from the group order q and check if borrow is set afterwards */
    for (int i = 0; i < sizeof(group_order); i++) {

        u32 diff = group_order[i] - s[i];
        /* If the subtraction was non-zero, then s != q,
         * let the notq variable reflect that */
        asm volatile("movnez %[notq], %[diff], %[diff]" : [notq] "+&r" (notq) : [diff] "r" (diff) : );
        diff -= borrow;
        /* Extract the sign bit to identify the borrow */
        asm volatile("extui %[borrow], %[diff], 31, 1" : [borrow] "+&r" (borrow) : [diff] "r" (diff) : );
    }

    /* Normalize the equality indicator */
    asm volatile(
        _ "movi.n %[temp], 1"
        _ "movnez %[notq], %[temp], %[notq]"

        : [notq] "+r" (notq),
          [temp] "=r" (temp)
        :
        :
    );

    /* A scalar is canonical if and only if there was
     * no borrow at the end of the loop, i.e. s <= q, and
     * notq is set, i.e. s != q */
    return ~borrow & notq;
}

static void ed448_scalar_sub_internal(u8 * r, const u8 * a, const u8 * b, int mock) {

    /* Note that we could do subtraction with u16 granularity since we use packed-radix
     * representation, thus better utilizing the hardware adder and halving the number
     * of iterations. It does not seem to have any bearing on performance, however, so
     * for code clarity we make byte-based accesses and subtractions. */
    u32 borrow = 0;
    for (int i = 0; i < sizeof(group_order); i++) {

        u32 subtrahend = b[i];
        /* If mocked subtraction, set the subtrahend to zero */
        asm volatile("movnez %0, %1, %2" : "+&r" (subtrahend) : "r" (borrow), "r" (mock) : );
        u32 diff = a[i] - subtrahend - borrow;
        /* Check the sign bit for underflow */
        borrow = (diff >> 31);
        r[i] = (u8) diff;
    }
}

static void ed448_scalar_reduce(u8 * s) {

    /* Let L be the group order and c = 2^446 - L. Then we have:
     *
     *                        2^448 = 4c (mod L)
     *
     * and so bytes 56 and beyond of the input scalar can be brought down
     * below the 2^448 boundary after multiplying by 4c. Note that c
     * is stored in group_order_crandall_tail.
     *
     * We shall start by clearing the top half of s using the identity
     * above. We do this three times, followed by a "careful" reduction
     * using the identity:
     *
     *                        2^446 = c  (mod L)
     *
     * This will guarantee the scalar is in in the range [0, 2L), i.e.
     * weakly reduced. At that point we shall multiplex between (conditionally
     * select one of) s and s - L.
     *
     * To each of the reduction steps correspond three loops (four at times, see below).
     * Recall the schoolbook multiplication algorithm and the shape of the
     * columns it produces:
     *                                ___ loop 2 end      ______ loop 1 end
     *                               |                   |
     *                       x  x  x |  x  x  x  x  x  x |  x  x  x  x  x
     *                               |  y  y  y  y  y  y |  y  y  y  y  y
     * ----------------------------- --------------------|----------------
     *                               | xy xy xy xy xy xy | xy xy xy xy xy
     *                            xy | xy xy xy xy xy xy | xy xy xy xy
     *                         xy xy | xy xy xy xy xy xy | xy xy xy
     *                      xy xy xy | xy xy xy xy xy xy | xy xy
     *                   xy xy xy xy | xy xy xy xy xy xy | xy
     *                                         .
     *                                         .
     *                                         .
     *
     * In the algorithm we run the multiplication algorithm column-by-column, accumulating
     * each columns completely before moving on to the next one (and propagating carry).
     * First loop of each step corresponds to the part of the array above where the columns
     * continue to increase in size. Loop 2 corresponds to the middle part where the column
     * size stays constant, and finally loop 3 corresponds to the tail, where the columns are
     * slowly shrinking.
     *
     * Sometimes loop 2 or 3 may be split into two parts to accomodate for crossing the
     * 56-byte boundary. Otherwise, we would incorrectly count some of the limbs (bytes)
     * of the operands more than once.
     */

    /* Note that we could precompute 4c and store it in a separate array thereby removing
     * the need to do the shifts by 2 in the loops below. Benchmarks show, however, that
     * this has no effect on the overall performance, so we go with smaller binary size. */
    u8 t[87];
    u32 carry = 0;
    for (int k = 0; k < 28; k++) {

        carry += s[k];
        for (int i = 0; i <= k; i++) {

            int j = k - i;
            carry += s[56 + i] * ( (u32) group_order_crandall_tail[j] << 2 );
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 28; k < 56; k++) {

        carry += s[k];
        for (int i = k + 1 - 28; i <= k; i++) {

            int j = k - i;
            carry += s[56 + i] * ( (u32) group_order_crandall_tail[j] << 2 );
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 56; k < 58; k++) {

        for (int i = k + 1 - 28; i <= k; i++) {

            int j = k - i;
            carry += s[56 + i] * ( (u32) group_order_crandall_tail[j] << 2 );
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 58; k < 86; k++) {

        for (int i = k + 1 - 28; i < 58; i++) {

            int j = k - i;
            carry += s[56 + i] * ( (u32) group_order_crandall_tail[j] << 2 );
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }
    t[86] = (u8) carry;

    carry = 0;
    /* We have reduced s down to (at most) 87 bytes. Do the same trick again and
     * bring down the 31 bytes above the 56-byte boundary. This way we shall get down to
     * 31 + 29 - 1 = 59 bytes (after multiplying the 31 bytes by the 29-byte
     * 4 * group_order_crandall_tail). */
    for (int k = 0; k < 28; k++) {

        carry += t[k];
        for (int i = 0; i <= k; i++) {

            int j = k - i;
            carry += t[56 + i] * ( (u32) group_order_crandall_tail[j] << 2 );
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 28; k < 31; k++) {

        carry += t[k];
        for (int i = k + 1 - 28; i <= k; i++) {

            int j = k - i;
            carry += t[56 + i] * ( (u32) group_order_crandall_tail[j] << 2 );
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 31; k < 56; k++) {

        carry += t[k];
        for (int i = k + 1 - 28; i < 31; i++) {

            int j = k - i;
            carry += t[56 + i] * ( (u32) group_order_crandall_tail[j] << 2 );
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 56; k < 59; k++) {

        for (int i = k + 1 - 28; i < 31; i++) {

            int j = k - i;
            carry += t[56 + i] * ( (u32) group_order_crandall_tail[j] << 2 );
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }
    FE3C_SANITY_CHECK(carry == 0, NULL);

    /* Do one last rough reduction. At this point we have only three bytes above
     * the 56-byte boundary. */
    for (int k = 0; k < 3; k++) {

        carry += t[k];
        for (int i = 0; i <= k; i++) {

            int j = k - i;
            carry += t[56 + i] * ( (u32) group_order_crandall_tail[j] << 2 );
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 3; k < 28; k++) {

        carry += t[k];
        for (int i = 0; i < 3; i++) {

            int j = k - i;
            carry += t[56 + i] * ( (u32) group_order_crandall_tail[j] << 2 );
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 28; k < 31; k++) {

        carry += t[k];
        for (int i = k + 1 - 28; i < 3; i++) {

            int j = k - i;
            carry += t[56 + i] * ( (u32) group_order_crandall_tail[j] << 2 );
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }
    /* Propagate the carry */
    for (int k = 31; k < 56; k++) {

        carry += t[k];
        t[k] = (u8) carry;
        carry >>= 8;
    }
    FE3C_SANITY_CHECK(carry == 0, NULL);

    /* Do a careful (albeit still weak) reduction */
    u32 overflow = (t[55] >> 6);
    t[55] &= 0x3f;
    for (int k = 0; k < 28; k++) {

        carry += t[k];
        carry += overflow * group_order_crandall_tail[k];
        t[k] = (u8) carry;
        carry >>= 8;
    }
    /* Propagate the carry */
    for (int k = 28; k < 56; k++) {

        carry += t[k];
        t[k] = (u8) carry;
        carry >>= 8;
    }
    FE3C_SANITY_CHECK(carry == 0, NULL);
    t[56] = 0;

    /* At this point we have successfully done a weak reduction and
     * the scalar is now smaller than 2L. Compute s - L and use that
     * conditionally if s is not canonical at this point. */
    int canonical = ed448_scalar_is_canonical(t);
    ed448_scalar_sub_internal(s, t, group_order, canonical);
}

static void ed448_scalars_muladd(u8 * r, const u8 * a, const u8 * b, const u8 * c) {

    u8 t[114];
    u32 carry = 0;
    for (int k = 0; k < 56; k++) {

        carry += c[k];
        /* Convolve a and b */
        for (int i = 0; i <= k; i++) {

            int j = k - i;
            carry += a[i] * b[j];
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }

    for (int k = 56; k < 112; k++) {

        /* Convolve a and b */
        for (int i = k + 1 - 56; i < 56; i++) {

            int j = k - i;
            carry += a[i] * b[j];
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }
    /* Clear the top two bytes to be compatible with ed448_scalar_reduce */
    t[113] = t[112] = 0;

    ed448_scalar_reduce(t);
    /* Copy to the output buffer */
    (void) memcpy(r, t, 57);
    /* Lower half of t is public (response of the signature), but
     * the top half needs to be purged as it reveals information
     * about the long-term secret key and the secret ephemeral. */
    purge_secrets(t, sizeof(t));
}

const scalar_ops ed448_scalar_ops = {
    .is_canonical = ed448_scalar_is_canonical,
    .reduce = ed448_scalar_reduce,
    .muladd = ed448_scalars_muladd
};
