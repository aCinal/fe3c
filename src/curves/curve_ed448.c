
#include <scalars/scalars.h>
#include <curves/curves.h>
#include <field_elements/field_elements.h>
#include <points/points.h>

#if !FE3C_SUPPORT_CURVE_ED448
    #error "Build system inconsistency detected! curve_ed448.c in use despite FE3C_SUPPORT_CURVE_ED448 not being set"
#endif /* !FE3C_SUPPORT_CURVE_ED448 */

static void prune_buffer(u8 *buffer);

curve ed448_curve = {
    .hash_function = hash_shake256,
    .gops = &ed448_group_ops,
    .sops = &ed448_scalar_ops,
    .prune_buffer = prune_buffer,
    .b_in_bytes = 456 / 8,
    .dom_string = "SigEd448",
    .dom_string_length = 8
};

static void prune_buffer(u8 *buffer)
{
    /* Prune the buffer or "clamp" the encoded scalar as specified by RFC 8032 */

    /* The two least significant bits of the first octet are cleared, so that the scalar is a multiple of the cofactor 4 */
    buffer[0] &= 0xfc;
    /* All eight bits of the last octet are cleared... */
    buffer[56] = 0;
    /* ...and the highest bit of the second to last octet is set */
    buffer[55] |= 0x80;
}
