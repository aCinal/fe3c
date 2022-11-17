
#include <scalars/scalars.h>
#include <curves/curves.h>
#include <field_elements/field_elements.h>
#include <points/points.h>

#if !FE3C_SUPPORT_CURVE_ED25519
    #error "Build system inconsistency detected! curve_ed25519.c in use despite FE3C_SUPPORT_CURVE_ED25519 not being set"
#endif /* !FE3C_SUPPORT_CURVE_ED25519 */

static void prune_buffer(u8 * buffer);

static curve s_ed25519;

void ed25519_init_curve(void) {

    s_ed25519.hash_function = hash_sha512;
    s_ed25519.gops = &ed25519_group_ops;
    s_ed25519.sops = &ed25519_scalar_ops;
    s_ed25519.prune_buffer = prune_buffer;
    s_ed25519.b_in_bytes = 256 / 8;
    s_ed25519.dom_string = "SigEd25519 no Ed25519 collisions";
    s_ed25519.dom_string_length = 32;

    /* Register the curve */
    curves[EDDSA_CURVE_ED25519] = &s_ed25519;
}

static void prune_buffer(u8 * buffer) {

    /* Prune the buffer or "clamp" the encoded scalar as specified by RFC 8032 */

    /* The lowest three bits of the first octet are cleared, so that the scalar is a multiple of the cofactor 8 */
    buffer[0] &= 0xf8;
    /* The highest bit of the last octet is cleared... */
    buffer[31] &= 0x7f;
    /* ...and the second highest bit of the last octet is set */
    buffer[31] |= 0x40;
}
