
#ifndef __FE3C_EDDSA_H
#define __FE3C_EDDSA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fe3c/global_defs.h>

/**
 * @brief Enumeration of different supported curves
 */
typedef enum eddsa_curve {

#if defined(FE3C_SUPPORT_CURVE_ED25519)
    EDDSA_CURVE_ED25519,              /**< Ed25519 */
#endif /* FE3C_SUPPORT_CURVE_ED25519 */

#if defined(FE3C_SUPPORT_CURVE_ED448)
    EDDSA_CURVE_ED448,                /**< Ed448 */
#endif /* FE3C_SUPPORT_CURVE_ED448 */

    /* Keep this last for boundary checking */
    EDDSA_NUMBER_OF_SUPPORTED_CURVES  /**< Number of supported curves */

} eddsa_curve;

/**
 * @brief Structure for passing arguments to eddsa_sign
 * @see eddsa_sign
 */
typedef struct eddsa_sign_request {

    /**
     * @brief Output buffer for the signature (use eddsa_get_signature_length to obtain the necessary size)
     * @see eddsa_get_signature_length
     */
    u8 * signature;

    /**
     * @brief Message over which the signature has been produced
     */
    const u8 * message;

    /**
     * @brief Private key
     * @see eddsa_get_secret_key_length
     */
    const u8 * secret_key;

    /**
     * @brief Pointer to context, can be NULL if context_length=0
     */
    const u8 * context;

    /**
     * @brief Length of the message
     */
    size_t message_length;

    /**
     * @brief Length of the context
     */
    u8 context_length;

    /**
     * @brief Curve identifier
     * @see eddsa_curve
     */
    eddsa_curve curve_id;

    /**
     * @brief 1 to use a prehashed (PH) variant of EdDSA, 0 otherwise
     */
    u8 phflag;

} eddsa_sign_request;

/**
 * @brief Structure for passing arguments to eddsa_verify
 * @see eddsa_verify
 */
typedef struct eddsa_verify_request {

    /**
     * @brief EdDSA signature of the format (R, S) where R is an Edwards curve point (commitment) and S is a scalar (response)
     */
    const u8 * signature;

    /**
     * @brief Message over which the signature has been produced
     */
    const u8 * message;

    /**
     * @brief Public key encoded according to RFC 8032
     * @see eddsa_get_public_key_length
     */
    const u8 * public_key;

    /**
     * @brief Pointer to context, can be NULL if context_length=0
     */
    const u8 * context;

    /**
     * @brief Length of the message
     */
    size_t message_length;

    /**
     * @brief Length of the context
     */
    u8 context_length;

    /**
     * @brief Curve identifier
     * @see eddsa_curve
     */
    eddsa_curve curve_id;

    /**
     * @brief 1 to use a prehashed (PH) variant of EdDSA, 0 otherwise
     */
    u8 phflag;

} eddsa_verify_request;

/**
 * @brief Initialize the library
 */
void fe3c_init(void);

/**
 * @brief Produce an EdDSA signature over a given message
 * @param req A signature request structure
 * @see eddsa_sign_request
 */
void eddsa_sign(const eddsa_sign_request * req);

/**
 * @brief Verify an EdDSA signature over a given message
 * @param req A verification request structure
 * @return Non-zero value on verification success or zero on verification failure
 * @see eddsa_verify_request
 */
int eddsa_verify(const eddsa_verify_request * req);

/**
 * @brief Derive the public key from the secret key
 * @param[out] public_key Output buffer for the public key
 * @param[in] secret_key Secret key
 * @param[in] curve_id Identifier of the curve
 * @note The buffer for both keys must be of adequate length
 * @see eddsa_curve, eddsa_get_public_key_length, eddsa_get_secret_key_length
 */
void eddsa_derive_public_key(u8 * public_key, const u8 * secret_key, eddsa_curve curve_id);

/**
 * @brief Return the signature length for a given Edwards curve
 * @param curve_id Identifier of the curve
 * @return Signature length for the given curve
 * @see eddsa_curve
 */
int eddsa_get_signature_length(eddsa_curve curve_id);

/**
 * @brief Return the public key length for a given Edwards curve
 * @param curve_id Identifier of the curve
 * @return Public key length for the given curve
 * @see eddsa_curve
 */
int eddsa_get_public_key_length(eddsa_curve curve_id);

/**
 * @brief Return the secret key length for a given Edwards curve
 * @param curve_id Identifier of the curve
 * @return Secret key length for the given curve
 * @see eddsa_curve
 */
int eddsa_get_secret_key_length(eddsa_curve curve_id);

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_EDDSA_H */
