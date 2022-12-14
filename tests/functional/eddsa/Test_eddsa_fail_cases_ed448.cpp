#include <CppUTest/TestHarness.h>
#include <fe3c/eddsa.h>
#include <curves/curves.h>

TEST_GROUP(EDDSA_FAIL_CASES_ED448) {

};

TEST(EDDSA_FAIL_CASES_ED448, Verify_PublicKeyDecodingFails_ExpectVerificationFailure) {

    const u8 signature[] = \
        "\x53\x3a\x37\xf6\xbb\xe4\x57\x25\x1f\x02\x3c\x0d\x88\xf9\x76\xae" \
        "\x2d\xfb\x50\x4a\x84\x3e\x34\xd2\x07\x4f\xd8\x23\xd4\x1a\x59\x1f" \
        "\x2b\x23\x3f\x03\x4f\x62\x82\x81\xf2\xfd\x7a\x22\xdd\xd4\x7d\x78" \
        "\x28\xc5\x9b\xd0\xa2\x1b\xfd\x39\x80\xff\x0d\x20\x28\xd4\xb1\x8a" \
        "\x9d\xf6\x3e\x00\x6c\x5d\x1c\x2d\x34\x5b\x92\x5d\x8d\xc0\x0b\x41" \
        "\x04\x85\x2d\xb9\x9a\xc5\xc7\xcd\xda\x85\x30\xa1\x13\xa0\xf4\xdb" \
        "\xb6\x11\x49\xf0\x5a\x73\x63\x26\x8c\x71\xd9\x58\x08\xff\x2e\x65" \
        "\x26\x00";
    /* Set a low order point as the public key */
    const u8 public_key[] = \
        "\xfe\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xfe\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff\x00";

    eddsa_verify_request req = {
        .signature = signature,
        .message = nullptr,
        .public_key = public_key,
        .context = nullptr,
        .message_length = 0,
        .context_length = 0,
        .curve_id = EDDSA_CURVE_ED448,
        .phflag = 0
    };
    int verified = eddsa_verify(&req);
    CHECK_EQUAL(0, verified);
}

TEST(EDDSA_FAIL_CASES_ED448, Verify_CommitmentDecodingFails_ExpectVerificationFailure) {

    /* Set a low order point as the commitment */
    const u8 signature[] = \
        "\xfe\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xfe\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff\x00\xff\x0d\x20\x28\xd4\xb1\x8a" \
        "\x9d\xf6\x3e\x00\x6c\x5d\x1c\x2d\x34\x5b\x92\x5d\x8d\xc0\x0b\x41" \
        "\x04\x85\x2d\xb9\x9a\xc5\xc7\xcd\xda\x85\x30\xa1\x13\xa0\xf4\xdb" \
        "\xb6\x11\x49\xf0\x5a\x73\x63\x26\x8c\x71\xd9\x58\x08\xff\x2e\x65" \
        "\x26\x00";
    const u8 public_key[] = \
        "\x5f\xd7\x44\x9b\x59\xb4\x61\xfd\x2c\xe7\x87\xec\x61\x6a\xd4\x6a" \
        "\x1d\xa1\x34\x24\x85\xa7\x0e\x1f\x8a\x0e\xa7\x5d\x80\xe9\x67\x78" \
        "\xed\xf1\x24\x76\x9b\x46\xc7\x06\x1b\xd6\x78\x3d\xf1\xe5\x0f\x6c" \
        "\xd1\xfa\x1a\xbe\xaf\xe8\x25\x61\x80";

    eddsa_verify_request req = {
        .signature = signature,
        .message = nullptr,
        .public_key = public_key,
        .context = nullptr,
        .message_length = 0,
        .context_length = 0,
        .curve_id = EDDSA_CURVE_ED448,
        .phflag = 0
    };
    int verified = eddsa_verify(&req);
    CHECK_EQUAL(0, verified);
}

TEST(EDDSA_FAIL_CASES_ED448, Verify_NonCanonicalResponse_ExpectVerificationFailure) {

    /* Make the response a non-canonical scalar (larger than the main subgroup order) */
    const u8 signature[] = \
        "\x53\x3a\x37\xf6\xbb\xe4\x57\x25\x1f\x02\x3c\x0d\x88\xf9\x76\xae" \
        "\x2d\xfb\x50\x4a\x84\x3e\x34\xd2\x07\x4f\xd8\x23\xd4\x1a\x59\x1f" \
        "\x2b\x23\x3f\x03\x4f\x62\x82\x81\xf2\xfd\x7a\x22\xdd\xd4\x7d\x78" \
        "\x28\xc5\x9b\xd0\xa2\x1b\xfd\x39\x80\xff\x0d\x20\x28\xd4\xb1\x8a" \
        "\x9d\xf6\x3e\x00\x6c\x5d\x1c\x2d\x34\x5b\x92\x5d\x8d\xc0\x0b\x41" \
        "\x04\x85\x2d\xb9\x9a\xc5\xc7\xcd\xda\x85\x30\xa1\x13\xa0\xf4\xdb" \
        "\xb6\x11\x49\xf0\x5a\x73\x63\x26\x8c\x71\xd9\x58\x08\xff\x2e\x65" \
        "\xff\x00";
    const u8 public_key[] = \
        "\x5f\xd7\x44\x9b\x59\xb4\x61\xfd\x2c\xe7\x87\xec\x61\x6a\xd4\x6a" \
        "\x1d\xa1\x34\x24\x85\xa7\x0e\x1f\x8a\x0e\xa7\x5d\x80\xe9\x67\x78" \
        "\xed\xf1\x24\x76\x9b\x46\xc7\x06\x1b\xd6\x78\x3d\xf1\xe5\x0f\x6c" \
        "\xd1\xfa\x1a\xbe\xaf\xe8\x25\x61\x80";

    eddsa_verify_request req = {
        .signature = signature,
        .message = nullptr,
        .public_key = public_key,
        .context = nullptr,
        .message_length = 0,
        .context_length = 0,
        .curve_id = EDDSA_CURVE_ED448,
        .phflag = 0
    };
    int verified = eddsa_verify(&req);
    CHECK_EQUAL(0, verified);
}

TEST(EDDSA_FAIL_CASES_ED448, Verify_GroupEquationNotSatisifed_ExpectVerificationFailure) {

    const u8 signature[] = \
        "\x53\x3a\x37\xf6\xbb\xe4\x57\x25\x1f\x02\x3c\x0d\x88\xf9\x76\xae" \
        "\x2d\xfb\x50\x4a\x84\x3e\x34\xd2\x07\x4f\xd8\x23\xd4\x1a\x59\x1f" \
        "\x2b\x23\x3f\x03\x4f\x62\x82\x81\xf2\xfd\x7a\x22\xdd\xd4\x7d\x78" \
        "\x28\xc5\x9b\xd0\xa2\x1b\xfd\x39\x80\xff\x0d\x20\x28\xd4\xb1\x8a" \
        "\x9d\xf6\x3e\x00\x6c\x5d\x1c\x2d\x34\x5b\x92\x5d\x8d\xc0\x0b\x41" \
        "\x04\x85\x2d\xb9\x9a\xc5\xc7\xcd\xda\x85\x30\xa1\x13\xa0\xf4\xdb" \
        "\xb6\x11\x49\xf0\x5a\x73\x63\x26\x8c\x71\xd9\x58\x08\xff\x2e\x65" \
        "\x26\x00";
    /* Set the public key to a random other point */
    const u8 public_key[] = \
        "\x7c\xa8\xfd\x25\xf1\x6c\xc1\x8d\xaa\x0a\x8c\x7f\x04\x88\xa7\x82" \
        "\xe9\x7f\xeb\x8d\x8a\x11\xfb\x65\x4f\x02\x74\xe4\x3d\x0c\x90\xc2" \
        "\x71\xed\x4c\xeb\xcc\x90\x59\xff\xd7\x30\xd4\xba\x0b\x39\x71\x23" \
        "\x45\xa8\x22\x74\xb0\x89\xe0\x71\x80";

    eddsa_verify_request req = {
        .signature = signature,
        .message = nullptr,
        .public_key = public_key,
        .context = nullptr,
        .message_length = 0,
        .context_length = 0,
        .curve_id = EDDSA_CURVE_ED448,
        .phflag = 0
    };
    int verified = eddsa_verify(&req);
    CHECK_EQUAL(0, verified);
}
