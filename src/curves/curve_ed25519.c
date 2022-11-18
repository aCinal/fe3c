
#include <scalars/scalars.h>
#include <curves/curves.h>
#include <field_elements/field_elements.h>
#include <points/points.h>

#if !FE3C_SUPPORT_CURVE_ED25519
    #error "Build system inconsistency detected! curve_ed25519.c in use despite FE3C_SUPPORT_CURVE_ED25519 not being set"
#endif /* !FE3C_SUPPORT_CURVE_ED25519 */

static void prune_buffer(u8 * buffer);

curve ed25519_curve = {
    .hash_function = hash_sha512,
    .gops = &ed25519_group_ops,
    .sops = &ed25519_scalar_ops,
    .prune_buffer = prune_buffer,
    .b_in_bytes = 256 / 8,
    .dom_string = "SigEd25519 no Ed25519 collisions",
    .dom_string_length = 32
};

static void prune_buffer(u8 * buffer) {

    /* Prune the buffer or "clamp" the encoded scalar as specified by RFC 8032 */

    /* The lowest three bits of the first octet are cleared, so that the scalar is a multiple of the cofactor 8 */
    buffer[0] &= 0xf8;
    /* The highest bit of the last octet is cleared... */
    buffer[31] &= 0x7f;
    /* ...and the second highest bit of the last octet is set */
    buffer[31] |= 0x40;
}
