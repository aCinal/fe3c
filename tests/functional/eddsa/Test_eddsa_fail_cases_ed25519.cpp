#include <CppUTest/TestHarness.h>
#include <fe3c/eddsa.h>
#include <curves/curves.h>

TEST_GROUP(EDDSA_FAIL_CASES_ED25519) {

    TEST_SETUP() {

        ed25519_init_curve();
    }
};

TEST(EDDSA_FAIL_CASES_ED25519, Verify_PublicKeyDecodingFails_ExpectVerificationFailure) {

    const u8 signature[] = \
        "\xe5\x56\x43\x00\xc3\x60\xac\x72\x90\x86\xe2\xcc\x80\x6e\x82\x8a" \
        "\x84\x87\x7f\x1e\xb8\xe5\xd9\x74\xd8\x73\xe0\x65\x22\x49\x01\x55" \
        "\x5f\xb8\x82\x15\x90\xa3\x3b\xac\xc6\x1e\x39\x70\x1c\xf9\xb4\x6b" \
        "\xd2\x5b\xf5\xf0\x59\x5b\xbe\x24\x65\x51\x41\x43\x8e\x7a\x10\x0b";
    /* Set a low order point as the public key */
    const u8 public_key[] = \
        "\x26\xe8\x95\x8f\xc2\xb2\x27\xb0\x45\xc3\xf4\x89\xf2\xef\x98\xf0" \
        "\xd5\xdf\xac\x05\xd3\xc6\x33\x39\xb1\x38\x02\x88\x6d\x53\xfc\x05";

    eddsa_verify_request req = {
        .signature = signature,
        .message = nullptr,
        .public_key = public_key,
        .context = nullptr,
        .message_length = 0,
        .context_length = 0,
        .curve_id = EDDSA_CURVE_ED25519,
        .phflag = 0
    };
    int verified = eddsa_verify(&req);
    CHECK_EQUAL(0, verified);
}

TEST(EDDSA_FAIL_CASES_ED25519, Verify_CommitmentDecodingFails_ExpectVerificationFailure) {

    /* Set a low order point as the commitment */
    const u8 signature[] = \
        "\x26\xe8\x95\x8f\xc2\xb2\x27\xb0\x45\xc3\xf4\x89\xf2\xef\x98\xf0" \
        "\xd5\xdf\xac\x05\xd3\xc6\x33\x39\xb1\x38\x02\x88\x6d\x53\xfc\x05" \
        "\x5f\xb8\x82\x15\x90\xa3\x3b\xac\xc6\x1e\x39\x70\x1c\xf9\xb4\x6b" \
        "\xd2\x5b\xf5\xf0\x59\x5b\xbe\x24\x65\x51\x41\x43\x8e\x7a\x10\x0b";
    const u8 public_key[] = \
        "\xd7\x5a\x98\x01\x82\xb1\x0a\xb7\xd5\x4b\xfe\xd3\xc9\x64\x07\x3a" \
        "\x0e\xe1\x72\xf3\xda\xa6\x23\x25\xaf\x02\x1a\x68\xf7\x07\x51\x1a";

    eddsa_verify_request req = {
        .signature = signature,
        .message = nullptr,
        .public_key = public_key,
        .context = nullptr,
        .message_length = 0,
        .context_length = 0,
        .curve_id = EDDSA_CURVE_ED25519,
        .phflag = 0
    };
    int verified = eddsa_verify(&req);
    CHECK_EQUAL(0, verified);
}

TEST(EDDSA_FAIL_CASES_ED25519, Verify_NonCanonicalResponse_ExpectVerificationFailure) {

    /* Make the response a non-canonical scalar (larger than the main subgroup order) */
    const u8 signature[] = \
        "\xe5\x56\x43\x00\xc3\x60\xac\x72\x90\x86\xe2\xcc\x80\x6e\x82\x8a" \
        "\x84\x87\x7f\x1e\xb8\xe5\xd9\x74\xd8\x73\xe0\x65\x22\x49\x01\x55" \
        "\x5f\xb8\x82\x15\x90\xa3\x3b\xac\xc6\x1e\x39\x70\x1c\xf9\xb4\x6b" \
        "\xd2\x5b\xf5\xf0\x59\x5b\xbe\x24\x65\x51\x41\x43\x8e\x7a\x10\xfb";
    const u8 public_key[] = \
        "\xd7\x5a\x98\x01\x82\xb1\x0a\xb7\xd5\x4b\xfe\xd3\xc9\x64\x07\x3a" \
        "\x0e\xe1\x72\xf3\xda\xa6\x23\x25\xaf\x02\x1a\x68\xf7\x07\x51\x1a";

    eddsa_verify_request req = {
        .signature = signature,
        .message = nullptr,
        .public_key = public_key,
        .context = nullptr,
        .message_length = 0,
        .context_length = 0,
        .curve_id = EDDSA_CURVE_ED25519,
        .phflag = 0
    };
    int verified = eddsa_verify(&req);
    CHECK_EQUAL(0, verified);
}

TEST(EDDSA_FAIL_CASES_ED25519, Verify_GroupEquationNotSatisifed_ExpectVerificationFailure) {

    const u8 signature[] = \
        "\xe5\x56\x43\x00\xc3\x60\xac\x72\x90\x86\xe2\xcc\x80\x6e\x82\x8a" \
        "\x84\x87\x7f\x1e\xb8\xe5\xd9\x74\xd8\x73\xe0\x65\x22\x49\x01\x55" \
        "\x5f\xb8\x82\x15\x90\xa3\x3b\xac\xc6\x1e\x39\x70\x1c\xf9\xb4\x6b" \
        "\xd2\x5b\xf5\xf0\x59\x5b\xbe\x24\x65\x51\x41\x43\x8e\x7a\x10\x0b";
    /* Set the public key to a random other point */
    const u8 public_key[] = \
        "\xe0\xa0\xbb\x40\xd7\x12\x68\xac\x46\x0c\x02\x67\xa1\x9c\x08\x57" \
        "\xd8\xfb\x2a\x1b\xeb\x5d\x0d\x41\x91\x3b\xeb\x20\x7a\xd1\xc0\x92";

    eddsa_verify_request req = {
        .signature = signature,
        .message = nullptr,
        .public_key = public_key,
        .context = nullptr,
        .message_length = 0,
        .context_length = 0,
        .curve_id = EDDSA_CURVE_ED25519,
        .phflag = 0
    };
    int verified = eddsa_verify(&req);
    CHECK_EQUAL(0, verified);
}
