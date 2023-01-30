#include <CppUTest/TestHarness.h>
#include <fe3c/eddsa.h>

TEST_GROUP(EDDSA_FAIL_CASES_ED25519) {

};

TEST(EDDSA_FAIL_CASES_ED25519, Verify_GroupEquationNotSatisifed_ExpectVerificationFailure) {

    const u8 signature[] = \
        "\xe5\x56\x43\x00\xc3\x60\xac\x72" \
        "\x90\x86\xe2\xcc\x80\x6e\x82\x8a" \
        "\x84\x87\x7f\x1e\xb8\xe5\xd9\x74" \
        "\xd8\x73\xe0\x65\x22\x49\x01\x55" \
        "\x5f\xb8\x82\x15\x90\xa3\x3b\xac" \
        "\xc6\x1e\x39\x70\x1c\xf9\xb4\x6b" \
        "\xd2\x5b\xf5\xf0\x59\x5b\xbe\x24" \
        "\x65\x51\x41\x43\x8e\x7a\x10\x0b";
    /* Set the public key to a random other point */
    const u8 public_key[] = \
        "\xe0\xa0\xbb\x40\xd7\x12\x68\xac" \
        "\x46\x0c\x02\x67\xa1\x9c\x08\x57" \
        "\xd8\xfb\x2a\x1b\xeb\x5d\x0d\x41" \
        "\x91\x3b\xeb\x20\x7a\xd1\xc0\x92";

    eddsa_verify_request req = {
        .signature = signature,
        .message = nullptr,
        .public_key = public_key,
        .message_length = 0,
        .curve_id = EDDSA_CURVE_ED25519
    };
    int verified = eddsa_verify(&req);
    CHECK_EQUAL(0, verified);
}

TEST(EDDSA_FAIL_CASES_ED25519, Verify_PublicKeyDecodingFails_ExpectVerificationFailure) {

    const u8 signature[] = \
        "\xe5\x56\x43\x00\xc3\x60\xac\x72" \
        "\x90\x86\xe2\xcc\x80\x6e\x82\x8a" \
        "\x84\x87\x7f\x1e\xb8\xe5\xd9\x74" \
        "\xd8\x73\xe0\x65\x22\x49\x01\x55" \
        "\x5f\xb8\x82\x15\x90\xa3\x3b\xac" \
        "\xc6\x1e\x39\x70\x1c\xf9\xb4\x6b" \
        "\xd2\x5b\xf5\xf0\x59\x5b\xbe\x24" \
        "\x65\x51\x41\x43\x8e\x7a\x10\x0b";
    /* Let the y-coordinate of the key lead to (y^2 - 1)/(dy^2 + 1)
     * being a quadratic non-residue in the field */
    const u8 public_key[] = \
        "\xb8\x3a\x29\x2e\xcb\x88\xa5\x88" \
        "\x06\x16\x12\x17\xba\x8a\x32\xe6" \
        "\x7e\x9b\x50\xa1\x4a\x29\x5c\x6e" \
        "\x3d\x45\xed\xe4\x71\xc9\xef\x1d";

    eddsa_verify_request req = {
        .signature = signature,
        .message = nullptr,
        .public_key = public_key,
        .message_length = 0,
        .curve_id = EDDSA_CURVE_ED25519
    };
    int verified = eddsa_verify(&req);
    CHECK_EQUAL(0, verified);
}

TEST(EDDSA_FAIL_CASES_ED25519, Verify_CommitmentDecodingFails_ExpectVerificationFailure) {

    /* Let the y-coordinate of the commitment lead to (y^2 - 1)/(dy^2 + 1)
     * being a quadratic non-residue in the field */
    const u8 signature[] = \
        "\x5a\x45\x90\x34\x66\x98\x2c\x34" \
        "\xd4\xde\x37\xe5\x45\x42\xfb\xda" \
        "\x6b\x3e\x4f\x76\xd6\xa2\xae\x9a" \
        "\xf2\xde\xe0\x39\xcc\xf5\xda\x0e" \
        "\x5f\xb8\x82\x15\x90\xa3\x3b\xac" \
        "\xc6\x1e\x39\x70\x1c\xf9\xb4\x6b" \
        "\xd2\x5b\xf5\xf0\x59\x5b\xbe\x24" \
        "\x65\x51\x41\x43\x8e\x7a\x10\x0b";
    const u8 public_key[] = \
        "\xd7\x5a\x98\x01\x82\xb1\x0a\xb7" \
        "\xd5\x4b\xfe\xd3\xc9\x64\x07\x3a" \
        "\x0e\xe1\x72\xf3\xda\xa6\x23\x25" \
        "\xaf\x02\x1a\x68\xf7\x07\x51\x1a";

    eddsa_verify_request req = {
        .signature = signature,
        .message = nullptr,
        .public_key = public_key,
        .message_length = 0,
        .curve_id = EDDSA_CURVE_ED25519
    };
    int verified = eddsa_verify(&req);
    CHECK_EQUAL(0, verified);
}
