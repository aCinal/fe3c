#include <functional_tests.h>
#include <field_elements/field_elements_ed448.h>

FE3C_TEST(FIELD_ELEMENTS_ED448_32, EncodeMinusOne_EncodeCorrectly) {

    fe448 input = {
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    u8 output[57];
    u8 expected[] = \
        "\xfe\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xfe\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\x00";
    fe448_encode(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(output));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, DecodeMinusOne_DecodeSuccessfully) {

    u8 input[] = \
        "\xfe\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xfe\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\x00";
    fe448 output;
    fe448 expected = {
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    int ok = fe448_decode(output, input);
    FAIL_UNLESS_EQUAL(1, ok);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, TryDecodingModulusPlusOne_FailToDecodeNotCanonical) {

    u8 input[] = \
        "\x00\x00\x00\x00\x00\x00\x00\x00" \
        "\x00\x00\x00\x00\x00\x00\x00\x00" \
        "\x00\x00\x00\x00\x00\x00\x00\x00" \
        "\x00\x00\x00\x00\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\x00";
    fe448 output;
    int ok = fe448_decode(output, input);
    /* Expect failure */
    FAIL_UNLESS_EQUAL(0, ok);
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, TryDecodingModulus_FailToDecodeNotCanonical) {

    u8 input[] = \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xfe\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\x00";
    fe448 output;
    int ok = fe448_decode(output, input);
    /* Expect failure */
    FAIL_UNLESS_EQUAL(0, ok);
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, CmovFlagCleared_NoOp) {

    fe448 input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    fe448 output = { 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    /* Expect output to remain untouched */
    fe448 expected = { 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    fe448_conditional_move(output, input, 0);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, CmovFlagSet_DoMove) {

    fe448 input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    fe448 output = { 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    /* Expect a move to take place */
    fe448 expected = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    fe448_conditional_move(output, input, 1);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, StrongReduce_ElementInOkRange_NoOp) {

    fe448 input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    fe448 expected = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    fe448 output;
    fe448_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, StrongReduce_ElementEqualToModulus_NormalizeToZero) {

    fe448 input = {
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 expected = {};
    fe448 output;
    fe448_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, StrongReduce_ElementEqualToTwiceTheModulusMinusOne_NormalizeToMinusOne) {

    fe448 input = {
        0x3fff3ffd, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffd, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x7fff3fff
    };
    fe448 expected = {
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 output;
    fe448_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, StrongReduce_ElementEqualToOneMoreThanTheModulus_ReduceToOne) {

    fe448 input = {
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 expected = { 1 };
    fe448 output;
    fe448_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe448));
}


FE3C_TEST(FIELD_ELEMENTS_ED448_32, StrongReduce_ElementEqualToOneLessThanTheModulus_NoOp) {

    fe448 input = {
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 expected = {
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 output;
    fe448_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, Equal_ElementsEqual_ReturnTrue) {

    fe448 input1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    fe448 input2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    int equal = fe448_equal(input1, input2);
    FAIL_UNLESS_EQUAL(1, equal);
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, Equal_ElementsDifferent_ReturnFalse) {

    fe448 input1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17 };
    fe448 input2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    int equal = fe448_equal(input1, input2);
    FAIL_UNLESS_EQUAL(0, equal);
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, Equal_ElementsCongruentButNotEqual_ReturnFalse) {

    fe448 input1 = {
        0x370b07d8, 0x0aa41fbd, 0x363837ea, 0x190236bc,
        0x0c672269, 0x2e4424d6, 0x1d2c0d9c, 0x02ec2d03,
        0x37952dc2, 0x23820958, 0x39872db7, 0x34863615,
        0x1bc93b03, 0x036c3862, 0x09b51fbf, 0x796e3817
    };
    fe448 input2 = {
        0x370b07d9, 0x0aa41fbd, 0x363837ea, 0x190236bc,
        0x0c672269, 0x2e4424d6, 0x1d2c0d9c, 0x02ec2d03,
        0x37952dc3, 0x23820958, 0x39872db7, 0x34863615,
        0x1bc93b03, 0x036c3862, 0x09b51fbf, 0x396e3817
    };
    int equal = fe448_equal(input1, input2);
    FAIL_UNLESS_EQUAL(0, equal);
    /* Do a reduction of input1 and check again */
    fe448_strong_reduce(input1, input1);
    equal = fe448_equal(input1, input2);
    FAIL_IF_MEMCMP(input1, input2, sizeof(fe448));
    FAIL_UNLESS_EQUAL(1, equal);
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, Add_OnePlusOne_EqualsTwo) {

    fe448 input1 = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe448 input2 = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe448 expected = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe448 output;
    fe448_add(output, input1, input2);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, Add_ModulusPlusModulus_CongruentToZero) {

    fe448 input1 = {
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 input2 = {
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 canonical = {};
    fe448 output;
    fe448_add(output, input1, input2);
    fe448_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, Neg_NegateNonCanonical_NegateCorrectly) {

    fe448 input = {
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 output;
    fe448 expected = {
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffd, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448_neg(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, Sub_ModulusMinusModulus_CongruentToZero) {

    fe448 input1 = {
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 input2 = {
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 canonical = {};
    fe448 output;
    fe448_sub(output, input1, input2);
    fe448_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, Sub_OneMinusTwo_CongruentToModulusMinusOne) {

    fe448 input1 = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe448 input2 = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe448 canonical = {
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };

    fe448 output;
    fe448_sub(output, input1, input2);
    fe448_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, Mul_ModulusPlusOneTimesTwo_CongruentToTwo) {

    fe448 input1 = {
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 input2 = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe448 canonical = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe448 output;
    /* Expect multiplication to result in a non-canonical form */
    fe448_mul(output, input1, input2);
    /* Do a reduction and check the canonical form */
    fe448_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, Mul_ModulusTimesModulus_CongruentToZero) {

    fe448 input1 = {
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 input2 = {
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 canonical = {};
    fe448 output;
    /* Expect multiplication to result in a non-canonical form */
    fe448_mul(output, input1, input2);
    /* Do a reduction and check the canonical form */
    fe448_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, Mul_ZeroTimesAnything_EqualToZero) {

    fe448 input1 = {};
    fe448 input2 = { 14, 0, 0, 0, 123, 2, 3, 4, 5, 6, 7, 19, 20, 21, 22, 23 };
    fe448 expected = {};
    fe448 output;
    fe448_mul(output, input1, input2);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, Mul_StabilityTest_RemainBoundedAndProduceCorrectResult) {

    /* Check the stability of multiplication, i.e. that the overflow does not grow without bounds */
    fe448 input = {
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 accumulator = { 1 };
    fe448 expected = {
        0x2a680eab, 0x035e3589, 0x16673c69, 0x052237ff,
        0x3ab61549, 0x0a771821, 0x1e52386b, 0x1db80d54,
        0x2931140d, 0x120228a3, 0x2b741968, 0x20e31f13,
        0x27761992, 0x1e8b03cc, 0x28261210, 0x0239285a
    };

    for (int i = 0; i < 1024; i++) {

        fe448_mul(accumulator, accumulator, input);
    }

    fe448_strong_reduce(accumulator, accumulator);
    FAIL_IF_MEMCMP(expected, accumulator, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, Mul_StabilityTest2_RemainBoundedAndProduceCorrectResult) {

    /* Check the stability of multiplication, i.e. that the overflow does not grow without bounds */
    fe448 input = {
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 expected = {
        0x122603a8, 0x3dd02dda, 0x1dd92357, 0x24af031e,
        0x172d3f6d, 0x0e7f23d1, 0x1beb36d4, 0x36b92dc0,
        0x13f21e24, 0x1b9f2904, 0x1f3534dc, 0x13f202c0,
        0x0f3f043d, 0x07422b76, 0x1b3c212f, 0x1e6e25ae
    };

    for (int i = 0; i < 1024; i++) {

        fe448_mul(input, input, input);
    }

    fe448_strong_reduce(input, input);
    FAIL_IF_MEMCMP(expected, input, sizeof(fe448));
}

FE3C_TEST(FIELD_ELEMENTS_ED448_32, Invert_Canonical_CorrectlyInvert) {

    fe448 input = {
        0x3fff3fba, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3ffe, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff,
        0x3fff3fff, 0x3fff3fff, 0x3fff3fff, 0x3fff3fff
    };
    fe448 output;
    fe448 product;
    fe448 expected_product = { 1 };

    fe448_invert(output, input);
    fe448_mul(product, input, output);
    fe448_strong_reduce(product, product);

    FAIL_IF_MEMCMP(expected_product, &product, sizeof(fe448));
}
