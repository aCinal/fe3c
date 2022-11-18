#include <fe3c/eddsa.h>
#include <curves/curves.h>
#include <utils/utils.h>

void eddsa_sign(const eddsa_sign_request * req) {

    FE3C_SANITY_CHECK(req);
    FE3C_SANITY_CHECK(req->signature);
    FE3C_SANITY_CHECK(req->secret_key);
    FE3C_SANITY_CHECK(req->message || req->message_length == 0);
    FE3C_SANITY_CHECK(req->context_length == 0 || req->context);
    FE3C_SANITY_CHECK(req->curve_id < EDDSA_NUMBER_OF_SUPPORTED_CURVES);
    FE3C_SANITY_CHECK(curves[req->curve_id]);
    FE3C_SANITY_CHECK(req->phflag <= 1);

    /* Iverson brackets testing if the context is used
     * and/or the message has been prehashed */
    int use_context = (req->context_length > 0);
    int use_dom_string = (use_context | req->phflag);
#if FE3C_SUPPORT_CURVE_ED448
    /* Note that EdDSA with curve Ed448 always uses the dom string */
        use_dom_string |= (req->curve_id == EDDSA_CURVE_ED448);
#endif
    /* Edwards curve to be used */
    const curve * curve = curves[req->curve_id];
    /* iovec buffer for hash function requests */
    struct iovec iov[6];
    /* Buffers for hash function outputs/scalars */
    u8 hash_secret_key[2 * curve->b_in_bytes];
    u8 ephemeral_scalar[2 * curve->b_in_bytes];
    u8 hash_commit_key_message[2 * curve->b_in_bytes];

    /* Buffers for encoded elliptic curve points */
    u8 encoded_public_key[curve->b_in_bytes];
    u8 encoded_commitment[curve->b_in_bytes];
    /* Buffer for the context variant of EdDSA */
    const u8 dom_bytes[] = {
        req->phflag,
        req->context_length
    };

    /* Recover pointers to the virtual method tables for the group and the
     * scalars of that group (allow polymorphism) */
    scalar_ops * sops = curve->sops;
    group_ops * gops = curve->gops;
    /* Recover the hash function associated with the curve, e.g.
     * SHA-512 for Ed25519 */
    hash h = curve->hash_function;

    /* Expand the secret key by hashing it */
    iov[0].iov_base = req->secret_key;
    iov[0].iov_len = curve->b_in_bytes;
    h(hash_secret_key, iov, 1);

    /* Prune the buffer before generating the public key as specified by RFC 8032
     * sections 5.1.5 (Ed25519) and 5.2.5 (Ed448) */
    curve->prune_buffer(hash_secret_key);
    /* Recover the public key */
    point public_key;
    gops->multiply_basepoint(&public_key, hash_secret_key);
    gops->encode(encoded_public_key, &public_key);

    /* Hash the second half of the digest output concatenated with the message. Conditionally
     * prepend the dom string */
    iov[0].iov_base = curve->dom_string;
    iov[0].iov_len = curve->dom_string_length * use_dom_string;
    iov[1].iov_base = dom_bytes;
    iov[1].iov_len = sizeof(dom_bytes) * use_dom_string;
    iov[2].iov_base = req->context;
    iov[2].iov_len = req->context_length * use_context;
    iov[3].iov_base = &hash_secret_key[curve->b_in_bytes];
    iov[3].iov_len = curve->b_in_bytes;
    iov[4].iov_base = req->message;
    iov[4].iov_len = req->message_length;
    h(ephemeral_scalar, iov, 5);

    sops->reduce(ephemeral_scalar);

    /* Compute the public commitment */
    point commitment;
    gops->multiply_basepoint(&commitment, ephemeral_scalar);
    gops->encode(encoded_commitment, &commitment);

    /* Note that iov[0]-iov[2] are correctly set to the context-related values */
    iov[3].iov_base = encoded_commitment;
    iov[3].iov_len = curve->b_in_bytes;
    iov[4].iov_base = encoded_public_key;
    iov[4].iov_len = curve->b_in_bytes;
    iov[5].iov_base = req->message;
    iov[5].iov_len = req->message_length;
    h(hash_commit_key_message, iov, 6);
    sops->reduce(hash_commit_key_message);

    /* Write the commitment into the signature */
    (void) memcpy(req->signature, encoded_commitment, curve->b_in_bytes);

    /* Compute H(R|A|M) s + r (notation as in RFC 8032) */
    sops->muladd(&req->signature[curve->b_in_bytes], hash_commit_key_message, hash_secret_key, ephemeral_scalar);

    /* Purge the stack reliably, i.e. memset all intermediate buffers to 0 */
    purge_secrets(&hash_secret_key, sizeof(hash_secret_key));
    purge_secrets(&ephemeral_scalar, sizeof(ephemeral_scalar));
}

int eddsa_verify(const eddsa_verify_request * req) {

    /* Do sanity-checks about parameters which are under the control of
     * the caller (not a remote adversarial party) */
    FE3C_SANITY_CHECK(req);
    FE3C_SANITY_CHECK(req->signature);
    FE3C_SANITY_CHECK(req->public_key);
    FE3C_SANITY_CHECK(req->message || req->message_length == 0);
    FE3C_SANITY_CHECK(req->context_length == 0 || req->context);
    FE3C_SANITY_CHECK(req->curve_id < EDDSA_NUMBER_OF_SUPPORTED_CURVES);
    FE3C_SANITY_CHECK(curves[req->curve_id]);
    FE3C_SANITY_CHECK(req->phflag <= 1);

    /* Verification status */
    int verified = 1;
    /* Iverson brackets testing if the context is used
     * and/or the message has been prehashed */
    int use_context = (req->context_length > 0);
    int use_dom_string = (use_context | req->phflag);
#if FE3C_SUPPORT_CURVE_ED448
    /* Note that EdDSA with curve Ed448 always uses the dom string */
        use_dom_string |= (req->curve_id == EDDSA_CURVE_ED448);
#endif
    /* Edwards curve to be used */
    const curve * curve = curves[req->curve_id];
    /* iovec buffer for hash function requests */
    struct iovec iov[6];
    /* Intermediate buffer for hash function output */
    u8 digest[2 * curve->b_in_bytes];
    /* Buffer for the context variant of EdDSA */
    const u8 dom_bytes[] = {
        req->phflag,
        req->context_length
    };

    /* Recover pointers to the virtual method tables for the group and the
     * scalars of that group (allow polymorphism) */
    scalar_ops * sops = curve->sops;
    group_ops * gops = curve->gops;
    /* Recover the hash function associated with the curve, e.g.
     * SHA-512 for Ed25519 */
    hash h = curve->hash_function;

    /* Note that point decoding may fail - we will still however continue with the verification to the end */
    /* TODO: Check if decode needs to set the points to something valid (e.g. the neutral element) so as to
     * not risk any undefined behaviour/fpe during the dummy computations that follow */
    point public_key;
    verified &= gops->decode(&public_key, req->public_key);
    point commitment;
    verified &= gops->decode(&commitment, req->signature);

    /* Check that the response scalar is valid, i.e. less than the prime order of the subgroup (do not do the
     * reduction ourselves) */
    verified &= sops->is_canonical(&req->signature[curve->b_in_bytes]);

    /* Compute the hash over the commitment, public key, the message and
     * the dom string (if the context variant of EdDSA is used) */
    iov[0].iov_base = curve->dom_string;
    iov[0].iov_len = curve->dom_string_length * use_dom_string;
    iov[1].iov_base = dom_bytes;
    iov[1].iov_len = sizeof(dom_bytes) * use_dom_string;
    iov[2].iov_base = req->context;
    iov[2].iov_len = req->context_length * use_context;
    iov[3].iov_base = req->signature;
    iov[3].iov_len = curve->b_in_bytes;  /* Only hash the first half of the signature, i.e. the commitment R */
    iov[4].iov_base = req->public_key;
    iov[4].iov_len = curve->b_in_bytes;
    iov[5].iov_base = req->message;
    iov[5].iov_len = req->message_length;
    h(digest, iov, 6);
    /* Reduce the digest output as a scalar */
    sops->reduce(digest);

    /* TODO: Study when is it ok to skip multiplication by the cofactor (as mentioned by RFC 8032) */

    /* TODO: Study WolfSSL's implementation of S*B - h*A concurrent multplication */
    gops->scalar_multiply(&public_key, &public_key, digest);
    gops->points_add(&commitment, &commitment, &public_key);

    /* Compute S B */
    point response_point;
    gops->multiply_basepoint(&response_point, &req->signature[curve->b_in_bytes]);
    /* Check if 2^c S B == 2^c R + h A */
    verified &= gops->points_equal(&response_point, &commitment);

    return verified;
}

void eddsa_derive_public_key(u8 * public_key, const u8 * secret_key, eddsa_curve curve_id) {

    FE3C_SANITY_CHECK(curve_id < EDDSA_NUMBER_OF_SUPPORTED_CURVES);
    FE3C_SANITY_CHECK(curves[curve_id])
    FE3C_SANITY_CHECK(public_key);
    FE3C_SANITY_CHECK(secret_key);

    /* Edwards curve to be used */
    const curve * curve = curves[curve_id];
    /* Intermediate buffer for hash function output */
    u8 digest[2 * curve->b_in_bytes];
    struct iovec iov[1];

    /* Recover the hash function associated with the curve, e.g.
     * SHA-512 for Ed25519 */
    hash h = curve->hash_function;

    /* Hash the secret key */
    iov[0].iov_base = secret_key;
    iov[0].iov_len = curve->b_in_bytes;
    h(digest, iov, 1);

    /* "Clamp" the result */
    curve->prune_buffer(digest);

    /* Recover the pointer to the virtual method table for the group */
    group_ops * gops = curve->gops;

    /* Multiply the base point by the scalar */
    point public_key_point;
    gops->multiply_basepoint(&public_key_point, digest);

    /* Encode the public point into the provided buffer */
    gops->encode(public_key, &public_key_point);
}

int eddsa_get_signature_length(eddsa_curve curve_id) {

    FE3C_SANITY_CHECK(curve_id < EDDSA_NUMBER_OF_SUPPORTED_CURVES);
    FE3C_SANITY_CHECK(curves[curve_id]);

    return 2 * curves[curve_id]->b_in_bytes;
}

int eddsa_get_public_key_length(eddsa_curve curve_id) {

    FE3C_SANITY_CHECK(curve_id < EDDSA_NUMBER_OF_SUPPORTED_CURVES);
    FE3C_SANITY_CHECK(curves[curve_id]);

    return curves[curve_id]->b_in_bytes;
}

int eddsa_get_secret_key_length(eddsa_curve curve_id) {

    FE3C_SANITY_CHECK(curve_id < EDDSA_NUMBER_OF_SUPPORTED_CURVES);
    FE3C_SANITY_CHECK(curves[curve_id]);

    return curves[curve_id]->b_in_bytes;
}

int eddsa_is_curve_supported(eddsa_curve curve_id) {

    FE3C_SANITY_CHECK(curve_id < EDDSA_NUMBER_OF_SUPPORTED_CURVES);
    return (curves[curve_id] != NULL);
}
