#include <CppUTest/TestHarness.h>
#include <hash_functions/hash_functions.h>
#include <string.h>
#include <algorithm>

#define HASH_OUTPUT_SIZE_IN_BYTES  ( 512 / 8 )
#define APPROPRIATE_IOV_LEN(iov)   (iov).iov_len = strlen(static_cast<const char *>((iov).iov_base));

TEST_GROUP(HASH_FUNCTION_SHA512) {

};

TEST(HASH_FUNCTION_SHA512, EmptyIovec) {

    u8 output[HASH_OUTPUT_SIZE_IN_BYTES];
    const u8 expected_output[] = \
        "\xcf\x83\xe1\x35\x7e\xef\xb8\xbd\xf1\x54\x28\x50\xd6\x6d\x80\x07" \
        "\xd6\x20\xe4\x05\x0b\x57\x15\xdc\x83\xf4\xa9\x21\xd3\x6c\xe9\xce" \
        "\x47\xd0\xd1\x3c\x5d\x85\xf2\xb0\xff\x83\x18\xd2\x87\x7e\xec\x2f" \
        "\x63\xb9\x31\xbd\x47\x41\x7a\x81\xa5\x38\x32\x7a\xf9\x27\xda\x3e";

    hash_sha512(output, nullptr, 0);

    MEMCMP_EQUAL(expected_output, output, HASH_OUTPUT_SIZE_IN_BYTES);
}

TEST(HASH_FUNCTION_SHA512, EmptyString) {

    u8 output[HASH_OUTPUT_SIZE_IN_BYTES];
    const u8 expected_output[] = \
        "\xcf\x83\xe1\x35\x7e\xef\xb8\xbd\xf1\x54\x28\x50\xd6\x6d\x80\x07" \
        "\xd6\x20\xe4\x05\x0b\x57\x15\xdc\x83\xf4\xa9\x21\xd3\x6c\xe9\xce" \
        "\x47\xd0\xd1\x3c\x5d\x85\xf2\xb0\xff\x83\x18\xd2\x87\x7e\xec\x2f" \
        "\x63\xb9\x31\xbd\x47\x41\x7a\x81\xa5\x38\x32\x7a\xf9\x27\xda\x3e";

    struct iovec iov = {
        .iov_base = nullptr,
        .iov_len = 0
    };
    hash_sha512(output, &iov, 1);

    MEMCMP_EQUAL(expected_output, output, HASH_OUTPUT_SIZE_IN_BYTES);
}

TEST(HASH_FUNCTION_SHA512, SingleMessageOfLength1024) {

    u8 output[HASH_OUTPUT_SIZE_IN_BYTES];
    const u8 expected_output[] = \
        "\x77\xfd\xca\x42\x1e\x16\xeb\xe2\xa2\xbc\xd5\x85\x62\x1c\x11\xd2" \
        "\xd4\xdc\xb7\x2e\x9d\xf5\xd5\xd4\x8a\x75\xf0\x41\x7f\x40\xd2\x15" \
        "\x56\x9c\x25\xf0\x08\x51\xff\xed\x13\x0c\xc5\x40\xbe\x28\x1b\x6a" \
        "\xcd\xac\x96\xbd\x83\xd7\x16\x31\xfc\xe5\x73\xac\x6e\x01\xe9\xf0";

    u8 message[128];
    std::fill(message, message + 128, 0xAA);
    struct iovec iov = {
        .iov_base = message,
        .iov_len = sizeof(message)
    };
    hash_sha512(output, &iov, 1);

    MEMCMP_EQUAL(expected_output, output, HASH_OUTPUT_SIZE_IN_BYTES);
}

TEST(HASH_FUNCTION_SHA512, HelloWorld) {

    u8 output[HASH_OUTPUT_SIZE_IN_BYTES];
    const u8 expected_output[] = \
        "\xb7\xf7\x83\xba\xed\x82\x97\xf0\xdb\x91\x74\x62\x18\x4f\xf4\xf0" \
        "\x8e\x69\xc2\xd5\xe5\xf7\x9a\x94\x26\x00\xf9\x72\x5f\x58\xce\x1f" \
        "\x29\xc1\x81\x39\xbf\x80\xb0\x6c\x0f\xff\x2b\xdd\x34\x73\x84\x52" \
        "\xec\xf4\x0c\x48\x8c\x22\xa7\xe3\xd8\x0c\xdf\x6f\x9c\x1c\x0d\x47";

    struct iovec iov;
    iov.iov_base = "Hello world";
    APPROPRIATE_IOV_LEN(iov);
    hash_sha512(output, &iov, 1);

    MEMCMP_EQUAL(expected_output, output, HASH_OUTPUT_SIZE_IN_BYTES);
}

TEST(HASH_FUNCTION_SHA512, GatherIoTest) {

    u8 output[HASH_OUTPUT_SIZE_IN_BYTES];
    const u8 expected_output[] = \
        "\xd4\xd5\xb5\x27\x6b\x7f\x08\x25\xb7\xec\x01\x22\xac\x94\x5f\x14" \
        "\x7b\xab\x8b\xd0\xf5\x9a\x3a\xdc\x09\x14\xea\x7f\xe0\x6c\x01\xd7" \
        "\xff\x15\x1c\xf0\x7e\xd1\xff\x5b\x5d\x37\x78\x0f\x62\x94\xbd\x6c" \
        "\x16\x5d\xde\x25\x65\x8b\x67\x01\x10\xf2\x5c\xca\xe2\x52\x70\xa3";

    struct iovec iov[3];
    iov[0].iov_base = "It is a good life we lead, brother.";
    APPROPRIATE_IOV_LEN(iov[0]);
    iov[1].iov_base = "The best. May it never change.";
    APPROPRIATE_IOV_LEN(iov[1]);
    iov[2].iov_base = "And may it never change us.";
    APPROPRIATE_IOV_LEN(iov[2]);
    hash_sha512(output, iov, 3);

    MEMCMP_EQUAL(expected_output, output, HASH_OUTPUT_SIZE_IN_BYTES);
}

TEST(HASH_FUNCTION_SHA512, BufferSkippingTest) {

    u8 output[HASH_OUTPUT_SIZE_IN_BYTES];
    const u8 expected_output[] = \
        "\x4e\xcd\xfe\x9c\x99\xbb\x3f\x66\xe8\xc7\x1c\x54\xc7\x94\xcf\xb5" \
        "\xd3\x47\x82\xbe\x82\xa3\x41\x20\xf6\xb6\x68\x05\xee\xb2\xbb\x4d" \
        "\x2b\xab\x70\x72\x82\xb8\x62\xd3\x41\x86\xef\x11\x03\xf1\xb3\xff" \
        "\x2d\xdf\x60\x4d\x49\xe8\x39\xdc\xc3\xa0\x23\xc5\xd3\x1c\xf1\x3e";

    struct iovec iov[3];
    iov[0].iov_base = "Why did you throw the jack of hearts away?";
    APPROPRIATE_IOV_LEN(iov[0]);
    iov[1].iov_base = "Why did you throw the jack of hearts away?";
    /* Expect the second buffer to be skipped */
    iov[1].iov_len = 0;
    iov[2].iov_base = "It was the only card in the deck that I had left to play.";
    APPROPRIATE_IOV_LEN(iov[2]);
    hash_sha512(output, iov, 3);

    MEMCMP_EQUAL(expected_output, output, HASH_OUTPUT_SIZE_IN_BYTES);
}