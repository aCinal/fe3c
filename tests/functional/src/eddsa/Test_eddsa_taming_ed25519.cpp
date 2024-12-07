#include <functional_tests.h>
#include <fe3c/eddsa.h>

#if FE3C_FUNCTIONAL_TESTS_HOST
TEST_GROUP(EDDSA_TAMING_ED25519)
{
    /* Test vectors published by Konstantinos Chalkias, François Garillot, and Valeria Nikolaenko
     * in "Taming the many EdDSAs" (DOI: 10.1007/978-3-030-64357-7_4). Notation corresponds to
     * the notation used in the paper as well as in RFC 8032, i.e. S is the response scalar,
     * A is the public key, R is the commitment, h = H(R||A||M) is the hash output and B is the
     * group generator (basepoint). Conditions for each test vector are presented below (courtesy
     * of the paper authors). */
};
#endif /* FE3C_FUNCTIONAL_TESTS_HOST */

FE3C_TEST(EDDSA_TAMING_ED25519, Test0)
{
    /* Conditions satisified by the test vector:
     *
     *                  S:        S = 0
     *          A's order:        small
     *          R's order:        small
     * 8(SB) = 8R + 8(hA):        ok
     *        SB = R + hA:        ok
     */

    const u8 message[] = \
        "\x8c\x93\x25\x5d\x71\xdc\xab\x10" \
        "\xe8\xf3\x79\xc2\x62\x00\xf3\xc7" \
        "\xbd\x5f\x09\xd9\xbc\x30\x68\xd3" \
        "\xef\x4e\xde\xb4\x85\x30\x22\xb6";
    const u8 pub_key[] = \
        "\xc7\x17\x6a\x70\x3d\x4d\xd8\x4f" \
        "\xba\x3c\x0b\x76\x0d\x10\x67\x0f" \
        "\x2a\x20\x53\xfa\x2c\x39\xcc\xc6" \
        "\x4e\xc7\xfd\x77\x92\xac\x03\xfa";
    const u8 signature[] = \
        "\xc7\x17\x6a\x70\x3d\x4d\xd8\x4f" \
        "\xba\x3c\x0b\x76\x0d\x10\x67\x0f" \
        "\x2a\x20\x53\xfa\x2c\x39\xcc\xc6" \
        "\x4e\xc7\xfd\x77\x92\xac\x03\x7a" \
        "\x00\x00\x00\x00\x00\x00\x00\x00" \
        "\x00\x00\x00\x00\x00\x00\x00\x00" \
        "\x00\x00\x00\x00\x00\x00\x00\x00" \
        "\x00\x00\x00\x00\x00\x00\x00\x00";

    eddsa_verify_request req = {
        .signature = signature,
        .message = message,
        .public_key = pub_key,
        .message_length = sizeof(message) - 1,
        .curve_id = EDDSA_CURVE_ED25519,
    };
    int verified = eddsa_verify(&req);
    /* Fail the vector because of small order points */
    FAIL_UNLESS_EQUAL(0, verified);
}

FE3C_TEST(EDDSA_TAMING_ED25519, Test1)
{
    /* Conditions satisified by the test vector:
     *
     *                  S:        0 < S < L
     *          A's order:        small
     *          R's order:        mixed
     * 8(SB) = 8R + 8(hA):        ok
     *        SB = R + hA:        ok
     */

    const u8 message[] = \
        "\x9b\xd9\xf4\x4f\x4d\xcc\x75\xbd" \
        "\x53\x1b\x56\xb2\xcd\x28\x0b\x0b" \
        "\xb3\x8f\xc1\xcd\x6d\x12\x30\xe1" \
        "\x48\x61\xd8\x61\xde\x09\x2e\x79";
    const u8 pub_key[] = \
        "\xc7\x17\x6a\x70\x3d\x4d\xd8\x4f" \
        "\xba\x3c\x0b\x76\x0d\x10\x67\x0f" \
        "\x2a\x20\x53\xfa\x2c\x39\xcc\xc6" \
        "\x4e\xc7\xfd\x77\x92\xac\x03\xfa";
    const u8 signature[] = \
        "\xf7\xba\xde\xc5\xb8\xab\xea\xf6" \
        "\x99\x58\x39\x92\x21\x9b\x7b\x22" \
        "\x3f\x1d\xf3\xfb\xbe\xa9\x19\x84" \
        "\x4e\x3f\x7c\x55\x4a\x43\xdd\x43" \
        "\xa5\xbb\x70\x47\x86\xbe\x79\xfc" \
        "\x47\x6f\x91\xd3\xf3\xf8\x9b\x03" \
        "\x98\x4d\x80\x68\xdc\xf1\xbb\x7d" \
        "\xfc\x66\x37\xb4\x54\x50\xac\x04";

    eddsa_verify_request req = {
        .signature = signature,
        .message = message,
        .public_key = pub_key,
        .message_length = sizeof(message) - 1,
        .curve_id = EDDSA_CURVE_ED25519,
    };
    int verified = eddsa_verify(&req);
    /* Fail the vector because of small order public key */
    FAIL_UNLESS_EQUAL(0, verified);
}

FE3C_TEST(EDDSA_TAMING_ED25519, Test2)
{
    /* Conditions satisified by the test vector:
     *
     *                  S:        0 < S < L
     *          A's order:        mixed
     *          R's order:        small
     * 8(SB) = 8R + 8(hA):        ok
     *        SB = R + hA:        ok
     */

    const u8 message[] = \
        "\xae\xbf\x3f\x26\x01\xa0\xc8\xc5" \
        "\xd3\x9c\xc7\xd8\x91\x16\x42\xf7" \
        "\x40\xb7\x81\x68\x21\x8d\xa8\x47" \
        "\x17\x72\xb3\x5f\x9d\x35\xb9\xab";
    const u8 pub_key[] = \
        "\xf7\xba\xde\xc5\xb8\xab\xea\xf6" \
        "\x99\x58\x39\x92\x21\x9b\x7b\x22" \
        "\x3f\x1d\xf3\xfb\xbe\xa9\x19\x84" \
        "\x4e\x3f\x7c\x55\x4a\x43\xdd\x43";
    const u8 signature[] = \
        "\xc7\x17\x6a\x70\x3d\x4d\xd8\x4f" \
        "\xba\x3c\x0b\x76\x0d\x10\x67\x0f" \
        "\x2a\x20\x53\xfa\x2c\x39\xcc\xc6" \
        "\x4e\xc7\xfd\x77\x92\xac\x03\xfa" \
        "\x8c\x4b\xd4\x5a\xec\xac\xa5\xb2" \
        "\x4f\xb9\x7b\xc1\x0a\xc2\x7a\xc8" \
        "\x75\x1a\x7d\xfe\x1b\xaf\xf8\xb9" \
        "\x53\xec\x9f\x58\x33\xca\x26\x0e";

    eddsa_verify_request req = {
        .signature = signature,
        .message = message,
        .public_key = pub_key,
        .message_length = sizeof(message) - 1,
        .curve_id = EDDSA_CURVE_ED25519,
    };
    int verified = eddsa_verify(&req);
    /* Fail the vector because of small order commitment */
    FAIL_UNLESS_EQUAL(0, verified);
}

FE3C_TEST(EDDSA_TAMING_ED25519, Test3)
{
    /* Conditions satisified by the test vector:
     *
     *                  S:        0 < S < L
     *          A's order:        mixed
     *          R's order:        mixed
     * 8(SB) = 8R + 8(hA):        ok
     *        SB = R + hA:        ok
     */

    const u8 message[] = \
        "\x9b\xd9\xf4\x4f\x4d\xcc\x75\xbd" \
        "\x53\x1b\x56\xb2\xcd\x28\x0b\x0b" \
        "\xb3\x8f\xc1\xcd\x6d\x12\x30\xe1" \
        "\x48\x61\xd8\x61\xde\x09\x2e\x79";
    const u8 pub_key[] = \
        "\xcd\xb2\x67\xce\x40\xc5\xcd\x45" \
        "\x30\x6f\xa5\xd2\xf2\x97\x31\x45" \
        "\x93\x87\xdb\xf9\xeb\x93\x3b\x7b" \
        "\xd5\xae\xd9\xa7\x65\xb8\x8d\x4d";
    const u8 signature[] = \
        "\x90\x46\xa6\x47\x50\x44\x49\x38" \
        "\xde\x19\xf2\x27\xbb\x80\x48\x5e" \
        "\x92\xb8\x3f\xdb\x4b\x65\x06\xc1" \
        "\x60\x48\x4c\x01\x6c\xc1\x85\x2f" \
        "\x87\x90\x9e\x14\x42\x8a\x7a\x1d" \
        "\x62\xe9\xf2\x2f\x3d\x3a\xd7\x80" \
        "\x2d\xb0\x2e\xb2\xe6\x88\xb6\xc5" \
        "\x2f\xcd\x66\x48\xa9\x8b\xd0\x09";

    eddsa_verify_request req = {
        .signature = signature,
        .message = message,
        .public_key = pub_key,
        .message_length = sizeof(message) - 1,
        .curve_id = EDDSA_CURVE_ED25519,
    };
    int verified = eddsa_verify(&req);
    /* Accept mixed-order points */
    FAIL_UNLESS_EQUAL(1, verified);
}

FE3C_TEST(EDDSA_TAMING_ED25519, Test4)
{
    /* Conditions satisified by the test vector:
     *
     *                  S:        0 < S < L
     *          A's order:        mixed
     *          R's order:        mixed
     * 8(SB) = 8R + 8(hA):        ok
     *        SB = R + hA:        nok
     */

    const u8 message[] = \
        "\xe4\x7d\x62\xc6\x3f\x83\x0d\xc7" \
        "\xa6\x85\x1a\x0b\x1f\x33\xae\x4b" \
        "\xb2\xf5\x07\xfb\x6c\xff\xec\x40" \
        "\x11\xea\xcc\xd5\x5b\x53\xf5\x6c";
    const u8 pub_key[] = \
        "\xcd\xb2\x67\xce\x40\xc5\xcd\x45" \
        "\x30\x6f\xa5\xd2\xf2\x97\x31\x45" \
        "\x93\x87\xdb\xf9\xeb\x93\x3b\x7b" \
        "\xd5\xae\xd9\xa7\x65\xb8\x8d\x4d";
    const u8 signature[] = \
        "\x16\x0a\x1c\xb0\xdc\x9c\x02\x58" \
        "\xcd\x0a\x7d\x23\xe9\x4d\x8f\xa8" \
        "\x78\xbc\xb1\x92\x5f\x2c\x64\x24" \
        "\x6b\x2d\xee\x17\x96\xbe\xd5\x12" \
        "\x5e\xc6\xbc\x98\x2a\x26\x9b\x72" \
        "\x3e\x06\x68\xe5\x40\x91\x1a\x9a" \
        "\x6a\x58\x92\x1d\x69\x25\xe4\x34" \
        "\xab\x10\xaa\x79\x40\x55\x1a\x09";

    eddsa_verify_request req = {
        .signature = signature,
        .message = message,
        .public_key = pub_key,
        .message_length = sizeof(message) - 1,
        .curve_id = EDDSA_CURVE_ED25519,
    };
    int verified = eddsa_verify(&req);
    FAIL_UNLESS_EQUAL(1, verified);
}

FE3C_TEST(EDDSA_TAMING_ED25519, Test5)
{
    /* Conditions satisified by the test vector:
     *
     *                  S:        0 < S < L
     *          A's order:        mixed
     *          R's order:        L
     * 8(SB) = 8R + 8(hA):        ok (unless 8h is prereduced modulo L)
     *        SB = R + hA:        nok
     */

    const u8 message[] = \
        "\xe4\x7d\x62\xc6\x3f\x83\x0d\xc7" \
        "\xa6\x85\x1a\x0b\x1f\x33\xae\x4b" \
        "\xb2\xf5\x07\xfb\x6c\xff\xec\x40" \
        "\x11\xea\xcc\xd5\x5b\x53\xf5\x6c";
    const u8 pub_key[] = \
        "\xcd\xb2\x67\xce\x40\xc5\xcd\x45" \
        "\x30\x6f\xa5\xd2\xf2\x97\x31\x45" \
        "\x93\x87\xdb\xf9\xeb\x93\x3b\x7b" \
        "\xd5\xae\xd9\xa7\x65\xb8\x8d\x4d";
    const u8 signature[] = \
        "\x21\x12\x2a\x84\xe0\xb5\xfc\xa4" \
        "\x05\x2f\x5b\x12\x35\xc8\x0a\x53" \
        "\x78\x78\xb3\x8f\x31\x42\x35\x6b" \
        "\x2c\x23\x84\xeb\xad\x46\x68\xb7" \
        "\xe4\x0b\xc8\x36\xda\xc0\xf7\x10" \
        "\x76\xf9\xab\xe3\xa5\x3f\x9c\x03" \
        "\xc1\xce\xee\xdd\xb6\x58\xd0\x03" \
        "\x04\x94\xac\xe5\x86\x68\x74\x05";

    eddsa_verify_request req = {
        .signature = signature,
        .message = message,
        .public_key = pub_key,
        .message_length = sizeof(message) - 1,
        .curve_id = EDDSA_CURVE_ED25519,
    };
    int verified = eddsa_verify(&req);
    FAIL_UNLESS_EQUAL(1, verified);
}

FE3C_TEST(EDDSA_TAMING_ED25519, Test6)
{
    /* Conditions satisified by the test vector:
     *
     *                  S:        S > L
     *          A's order:        L
     *          R's order:        L
     * 8(SB) = 8R + 8(hA):        ok
     *        SB = R + hA:        ok
     */

    const u8 message[] = \
        "\x85\xe2\x41\xa0\x7d\x14\x8b\x41" \
        "\xe4\x7d\x62\xc6\x3f\x83\x0d\xc7" \
        "\xa6\x85\x1a\x0b\x1f\x33\xae\x4b" \
        "\xb2\xf5\x07\xfb\x6c\xff\xec\x40";
    const u8 pub_key[] = \
        "\x44\x2a\xad\x9f\x08\x9a\xd9\xe1" \
        "\x46\x47\xb1\xef\x90\x99\xa1\xff" \
        "\x47\x98\xd7\x85\x89\xe6\x6f\x28" \
        "\xec\xa6\x9c\x11\xf5\x82\xa6\x23";
    const u8 signature[] = \
        "\xe9\x6f\x66\xbe\x97\x6d\x82\xe6" \
        "\x01\x50\xba\xec\xff\x99\x06\x68" \
        "\x4a\xeb\xb1\xef\x18\x1f\x67\xa7" \
        "\x18\x9a\xc7\x8e\xa2\x3b\x6c\x0e" \
        "\x54\x7f\x76\x90\xa0\xe2\xdd\xcd" \
        "\x04\xd8\x7d\xbc\x34\x90\xdc\x19" \
        "\xb3\xb3\x05\x2f\x7f\xf0\x53\x8c" \
        "\xb6\x8a\xfb\x36\x9b\xa3\xa5\x14";

    eddsa_verify_request req = {
        .signature = signature,
        .message = message,
        .public_key = pub_key,
        .message_length = sizeof(message) - 1,
        .curve_id = EDDSA_CURVE_ED25519,
    };
    int verified = eddsa_verify(&req);
    /* Fail the vector because of non-canonical response scalar */
    FAIL_UNLESS_EQUAL(0, verified);
}

FE3C_TEST(EDDSA_TAMING_ED25519, Test7)
{
    /* Conditions satisified by the test vector:
     *
     *                  S:        S >> L
     *          A's order:        L
     *          R's order:        L
     * 8(SB) = 8R + 8(hA):        ok
     *        SB = R + hA:        ok
     */

    const u8 message[] = \
        "\x85\xe2\x41\xa0\x7d\x14\x8b\x41" \
        "\xe4\x7d\x62\xc6\x3f\x83\x0d\xc7" \
        "\xa6\x85\x1a\x0b\x1f\x33\xae\x4b" \
        "\xb2\xf5\x07\xfb\x6c\xff\xec\x40";
    const u8 pub_key[] = \
        "\x44\x2a\xad\x9f\x08\x9a\xd9\xe1" \
        "\x46\x47\xb1\xef\x90\x99\xa1\xff" \
        "\x47\x98\xd7\x85\x89\xe6\x6f\x28" \
        "\xec\xa6\x9c\x11\xf5\x82\xa6\x23";
    const u8 signature[] = \
        "\x8c\xe5\xb9\x6c\x8f\x26\xd0\xab" \
        "\x6c\x47\x95\x8c\x9e\x68\xb9\x37" \
        "\x10\x4c\xd3\x6e\x13\xc3\x35\x66" \
        "\xac\xd2\xfe\x8d\x38\xaa\x19\x42" \
        "\x7e\x71\xf9\x8a\x47\x34\xe7\x4f" \
        "\x2f\x13\xf0\x6f\x97\xc2\x0d\x58" \
        "\xcc\x3f\x54\xb8\xbd\x0d\x27\x2f" \
        "\x42\xb6\x95\xdd\x7e\x89\xa8\xc2" \
        "\x02";

    eddsa_verify_request req = {
        .signature = signature,
        .message = message,
        .public_key = pub_key,
        .message_length = sizeof(message) - 1,
        .curve_id = EDDSA_CURVE_ED25519,
    };
    int verified = eddsa_verify(&req);
    /* Fail the vector because of non-canonical response scalar */
    FAIL_UNLESS_EQUAL(0, verified);
}

FE3C_TEST(EDDSA_TAMING_ED25519, Test8)
{
    /* Conditions satisified by the test vector:
     *
     *                  S:        0 < S < L
     *          A's order:        mixed
     *          R's order:        small
     * 8(SB) = 8R + 8(hA):        -
     *        SB = R + hA:        -
     *
     * In this vector R.y is non-canonical and implementations which reduce it before
     * hashing will accept the proof, those that do not will reject it. We reject
     * the signature either way because of non-canonical encoding and small order.
     */

    /* TODO: Double check the paper. R.y = p-1 in this case which is canonical */
    const u8 message[] = \
        "\x9b\xed\xc2\x67\x42\x37\x25\xd4" \
        "\x73\x88\x86\x31\xeb\xf4\x59\x88" \
        "\xba\xd3\xdb\x83\x85\x1e\xe8\x5c" \
        "\x85\xe2\x41\xa0\x7d\x14\x8b\x41";
    const u8 pub_key[] = \
        "\xf7\xba\xde\xc5\xb8\xab\xea\xf6" \
        "\x99\x58\x39\x92\x21\x9b\x7b\x22" \
        "\x3f\x1d\xf3\xfb\xbe\xa9\x19\x84" \
        "\x4e\x3f\x7c\x55\x4a\x43\xdd\x43";
    const u8 signature[] = \
        "\xec\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\x03\xbe\x96\x78\xac\x10\x2e\xdc" \
        "\xd9\x2b\x02\x10\xbb\x34\xd7\x42" \
        "\x8d\x12\xff\xc5\xdf\x5f\x37\xe3" \
        "\x59\x94\x12\x66\xa4\xe3\x5f\x0f";

    eddsa_verify_request req = {
        .signature = signature,
        .message = message,
        .public_key = pub_key,
        .message_length = sizeof(message) - 1,
        .curve_id = EDDSA_CURVE_ED25519,
    };
    int verified = eddsa_verify(&req);
    /* Fail the vector because of small order commitment.
     * Also note that its encoding is not canonical, as
     * the sign bit is set for x=0. */
    FAIL_UNLESS_EQUAL(0, verified);
}

FE3C_TEST(EDDSA_TAMING_ED25519, Test9)
{
    /* Conditions satisified by the test vector:
     *
     *                  S:        0 < S < L
     *          A's order:        mixed
     *          R's order:        small
     * 8(SB) = 8R + 8(hA):        -
     *        SB = R + hA:        -
     *
     * In this vector R.y is non-canonical and implementations which reduce it before
     * hashing will reject the proof, those that do not will accept it. We reject
     * the signature either way because of non-canonical encoding and small order.
     */

    const u8 message[] = \
        "\x9b\xed\xc2\x67\x42\x37\x25\xd4" \
        "\x73\x88\x86\x31\xeb\xf4\x59\x88" \
        "\xba\xd3\xdb\x83\x85\x1e\xe8\x5c" \
        "\x85\xe2\x41\xa0\x7d\x14\x8b\x41";
    const u8 pub_key[] = \
        "\xf7\xba\xde\xc5\xb8\xab\xea\xf6" \
        "\x99\x58\x39\x92\x21\x9b\x7b\x22" \
        "\x3f\x1d\xf3\xfb\xbe\xa9\x19\x84" \
        "\x4e\x3f\x7c\x55\x4a\x43\xdd\x43";
    const u8 signature[] = \
        "\xec\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xca\x8c\x5b\x64\xcd\x20\x89\x82" \
        "\xaa\x38\xd4\x93\x66\x21\xa4\x77" \
        "\x5a\xa2\x33\xaa\x05\x05\x71\x1d" \
        "\x8f\xdc\xfd\xaa\x94\x3d\x49\x08";

    eddsa_verify_request req = {
        .signature = signature,
        .message = message,
        .public_key = pub_key,
        .message_length = sizeof(message) - 1,
        .curve_id = EDDSA_CURVE_ED25519,
    };
    int verified = eddsa_verify(&req);
    /* Fail the vector because of small order commitment.
     * Also note that its encoding is not canonical, as
     * the sign bit is set for x=0. */
    FAIL_UNLESS_EQUAL(0, verified);
}

FE3C_TEST(EDDSA_TAMING_ED25519, Test10)
{
    /* Conditions satisified by the test vector:
     *
     *                  S:        0 < S < L
     *          A's order:        small
     *          R's order:        mixed
     * 8(SB) = 8R + 8(hA):        -
     *        SB = R + hA:        -
     *
     * In this vector A.y is non-canonical and implementations which reduce it before
     * hashing will accept the proof, those that do not will reject it. We reject
     * the signature either way because of non-canonical encoding and small order.
     */

    /* TODO: Double check the paper. A.y = p-1 in this case which is canonical */
    const u8 message[] = \
        "\xe9\x6b\x70\x21\xeb\x39\xc1\xa1" \
        "\x63\xb6\xda\x4e\x30\x93\xdc\xd3" \
        "\xf2\x13\x87\xda\x4c\xc4\x57\x2b" \
        "\xe5\x88\xfa\xfa\xe2\x3c\x15\x5b";
    const u8 pub_key[] = \
        "\xec\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff";
    const u8 signature[] = \
        "\xa9\xd5\x52\x60\xf7\x65\x26\x1e" \
        "\xb9\xb8\x4e\x10\x6f\x66\x5e\x00" \
        "\xb8\x67\x28\x7a\x76\x19\x90\xd7" \
        "\x13\x59\x63\xee\x0a\x7d\x59\xdc" \
        "\xa5\xbb\x70\x47\x86\xbe\x79\xfc" \
        "\x47\x6f\x91\xd3\xf3\xf8\x9b\x03" \
        "\x98\x4d\x80\x68\xdc\xf1\xbb\x7d" \
        "\xfc\x66\x37\xb4\x54\x50\xac\x04";

    eddsa_verify_request req = {
        .signature = signature,
        .message = message,
        .public_key = pub_key,
        .message_length = sizeof(message) - 1,
        .curve_id = EDDSA_CURVE_ED25519,
    };
    int verified = eddsa_verify(&req);
    /* Fail the vector because of small order public key */
    FAIL_UNLESS_EQUAL(0, verified);
}

FE3C_TEST(EDDSA_TAMING_ED25519, Test11)
{
    /* Conditions satisified by the test vector:
     *
     *                  S:        0 < S < L
     *          A's order:        small
     *          R's order:        mixed
     * 8(SB) = 8R + 8(hA):        -
     *        SB = R + hA:        -
     *
     * In this vector A.y is non-canonical and implementations which reduce it before
     * hashing will reject the proof, those that do not will accept it. We reject
     * the signature either way because of non-canonical encoding and small order.
     */

    /* TODO: Double check the paper. A.y = p-1 in this case which is canonical */
    const u8 message[] = \
        "\x39\xa5\x91\xf5\x32\x1b\xbe\x07" \
        "\xfd\x5a\x23\xdc\x2f\x39\xd0\x25" \
        "\xd7\x45\x26\x61\x57\x46\x72\x7c" \
        "\xee\xfd\x6e\x82\xae\x65\xc0\x6f";
    const u8 pub_key[] = \
        "\xec\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff";
    const u8 signature[] = \
        "\xa9\xd5\x52\x60\xf7\x65\x26\x1e" \
        "\xb9\xb8\x4e\x10\x6f\x66\x5e\x00" \
        "\xb8\x67\x28\x7a\x76\x19\x90\xd7" \
        "\x13\x59\x63\xee\x0a\x7d\x59\xdc" \
        "\xa5\xbb\x70\x47\x86\xbe\x79\xfc" \
        "\x47\x6f\x91\xd3\xf3\xf8\x9b\x03" \
        "\x98\x4d\x80\x68\xdc\xf1\xbb\x7d" \
        "\xfc\x66\x37\xb4\x54\x50\xac\x04";

    eddsa_verify_request req = {
        .signature = signature,
        .message = message,
        .public_key = pub_key,
        .message_length = sizeof(message) - 1,
        .curve_id = EDDSA_CURVE_ED25519,
    };
    int verified = eddsa_verify(&req);
    /* Fail the vector because of small order public key */
    FAIL_UNLESS_EQUAL(0, verified);
}
