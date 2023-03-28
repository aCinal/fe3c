#include <functional_tests.h>
#include <field_elements/field_elements_ed25519.h>

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, EncodeMinusOne_EncodeCorrectly) {

    fe25519 input = { 0x7fff7fec, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    u8 output[32];
    u8 expected[] = \
        "\xec\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\x7f";
    fe25519_encode(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(output));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, DecodeMinusOne_DecodeSuccessfully) {

    u8 input[] = \
        "\xec\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\x7f";
    fe25519 output;
    fe25519 expected = { 0x7fff7fec, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    int ok = fe25519_decode(output, input);
    FAIL_UNLESS_EQUAL(1, ok);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, TryDecodingModulusPlusOne_FailToDecodeNotCanonical) {

    u8 input[] = \
        "\xee\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\x7f";
    fe25519 output;
    int ok = fe25519_decode(output, input);
    /* Expect failure */
    FAIL_UNLESS_EQUAL(0, ok);
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, TryDecodingModulus_FailToDecodeNotCanonical) {

    u8 input[] = \
        "\xed\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\xff" \
        "\xff\xff\xff\xff\xff\xff\xff\x7f";
    fe25519 output;
    int ok = fe25519_decode(output, input);
    /* Expect failure */
    FAIL_UNLESS_EQUAL(0, ok);
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, CmovFlagCleared_NoOp) {

    fe25519 input = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    fe25519 output = { 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    /* Expect output to remain untouched */
    fe25519 expected = { 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    fe25519_conditional_move(output, input, 0);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, CmovFlagSet_DoMove) {

    fe25519 input = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    fe25519 output = { 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    /* Expect a move to take place */
    fe25519 expected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    fe25519_conditional_move(output, input, 1);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementInOkRange_NoOp) {

    fe25519 input = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    fe25519 expected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    fe25519 output;
    fe25519_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementInOkRange_NoOp) {

    fe25519 input = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    fe25519 expected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    fe25519 output;
    fe25519_weak_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementEqualToModulus_NormalizeToZero) {

    fe25519 input = { 0x7fff7fed, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 expected = {};
    fe25519 output;
    fe25519_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementEqualToModulus_NoOp) {

    fe25519 input = { 0x7fff7fed, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 expected = { 0x7fff7fed, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 output;
    fe25519_weak_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementEqualToOneMoreThanTheModulus_ReduceToOne) {

    fe25519 input = { 0x7fff7fed + 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 expected = { 1 };
    fe25519 output;
    fe25519_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementEqualToOneMoreThanTheModulus_NoOp) {

    fe25519 input = { 0x7fff7fed + 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 expected = { 0x7fff7fed + 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 output;
    fe25519_weak_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementEqualToOneLessThanTwiceTheModulus_ReduceToModulusMinusOne) {

    fe25519 input = { 0x7fff7fda - 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x17fff };
    fe25519 expected = { 0x7fff7fed - 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 output;
    fe25519_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementEqualToOneLessThanTwiceTheModulus_ReduceToModulusMinusOne) {

    fe25519 input = { 0x7fff7fda - 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x17fff };
    fe25519 expected = { 0x7fff7fed - 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 output;
    fe25519_weak_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementEqualToOneLessThanTheModulus_NoOp) {

    fe25519 input = { 0x7fff7fed - 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 expected = { 0x7fff7fed - 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 output;
    fe25519_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementEqualToOneLessThanTheModulus_NoOp) {

    fe25519 input = { 0x7fff7fed - 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 expected = { 0x7fff7fed - 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 output;
    fe25519_weak_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Equal_ElementsEqual_ReturnTrue) {

    fe25519 input1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    fe25519 input2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int equal = fe25519_equal(input1, input2);
    FAIL_UNLESS_EQUAL(1, equal);
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Equal_ElementsDifferent_ReturnFalse) {

    fe25519 input1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    fe25519 input2 = { 1, 2, 3, 4, 5, 6, 7, 8, 10 };
    int equal = fe25519_equal(input1, input2);
    FAIL_UNLESS_EQUAL(0, equal);
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Equal_ElementsCongruentButNotEqual_ReturnFalse) {

    fe25519 input1 = { 0x7fff7fed + 10, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 input2 = { 10 };
    int equal = fe25519_equal(input1, input2);
    FAIL_UNLESS_EQUAL(0, equal);
    /* Do a reduction of input1 and check again */
    fe25519_strong_reduce(input1, input1);
    equal = fe25519_equal(input1, input2);
    FAIL_IF_MEMCMP(input1, input2, sizeof(fe25519));
    FAIL_UNLESS_EQUAL(1, equal);
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Add_OnePlusOne_EqualsTwo) {

    fe25519 input1 = { 1, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe25519 input2 = { 1, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe25519 expected = { 2, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe25519 output;
    fe25519_add(output, input1, input2);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Add_ModulusPlusModulus_EqualsToTwiceTheModulus) {

    fe25519 input1 = { 0x7fff7fed, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 input2 = { 0x7fff7fed, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 expected = { 0x7fff7fda, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x17fff };
    fe25519 output;
    fe25519_add(output, input1, input2);

    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Sub_ModulusMinusModulus_CongruentToZero) {

    fe25519 input1 = { 0x7fff7fed, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 input2 = { 0x7fff7fed, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 canonical = {};
    fe25519 output;
    fe25519_sub(output, input1, input2);
    /* Expect subtraction to result in a non-canonical form, e.g. for a - b we may expect to get a + 2p - b */
    /* Do a reduction and check the canonical form */
    fe25519_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Sub_OneMinusTwo_CongruentToModulusMinusOne) {

    fe25519 input1 = { 1, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe25519 input2 = { 2, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe25519 canonical = { 0x7fff7fed - 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 output;
    fe25519_sub(output, input1, input2);
    /* Expect subtraction to result in a non-canonical form, e.g. for a - b we may expect to get a + 2p - b */
    /* Do a reduction and check the canonical form */
    fe25519_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Mul_ModulusPlusOneTimesTwo_CongruentToTwo) {

    fe25519 input1 = { 0x7fff7fed + 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 input2 = { 2, 0, 0, 0, 0, 0, 0, 0, 0, };
    fe25519 canonical = { 2, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe25519 output;
    /* Expect multiplication to result in a non-canonical form */
    fe25519_mul(output, input1, input2);
    /* Do a reduction and check the canonical form */
    fe25519_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Mul_ModulusTimesModulus_CongruentToZero) {

    fe25519 input1 = { 0x7fff7fed, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 input2 = { 0x7fff7fed, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 canonical = {};
    fe25519 output;
    /* Expect multiplication to result in a non-canonical form */
    fe25519_mul(output, input1, input2);
    /* Do a reduction and check the canonical form */
    fe25519_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Mul_TwoModuliMinusOneSquared_CongruentToOne) {

    fe25519 input1 = { 0x7fff7fda - 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x17fff };
    fe25519 input2 = { 0x7fff7fda - 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x17fff };
    fe25519 canonical = { 1 };
    fe25519 output;
    /* Expect multiplication to result in a non-canonical form */
    fe25519_mul(output, input1, input2);
    /* Do a reduction and check the canonical form */
    fe25519_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Mul_ZeroTimesAnything_EqualToZero) {

    fe25519 input1 = {};
    fe25519 input2 = { 14, 0, 0, 0, 123, 0, 0, 1, 2 };
    fe25519 expected = {};
    fe25519 output;
    fe25519_mul(output, input1, input2);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Mul_StabilityTest_RemainBoundedAndProduceCorrectResult) {

    /* Check the stability of multiplication, i.e. that the overflow does not grow without bounds */
    fe25519 input = { 0x7fff7fda - 0x1234, 0x7fff7fff - 0x1234, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff - 0x1234, 0x17fff };
    fe25519 accumulator = { 1 };
    fe25519 expected = { 0x3a637e31, 0x7d9b4ded, 0x2ccf29b4, 0x38f59b4, 0x1981709, 0x383d4b80, 0xfd86741, 0x7e716d92, 0x9eb };

    for (int i = 0; i < 1024; i++) {

        fe25519_mul(accumulator, accumulator, input);
    }

    fe25519_strong_reduce(accumulator, accumulator);
    FAIL_IF_MEMCMP(expected, &accumulator, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Mul_StabilityTest2_RemainBoundedAndProduceCorrectResult) {

    /* Check the stability of multiplication, i.e. that the overflow does not grow without bounds */
    fe25519 input = { 0x7fff7fda - 0x1234, 0x7fff7fff - 0x1234, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff - 0x1234, 0x17fff };
    fe25519 expected = { 0xfca2abb, 0x4b3b3733, 0x97c5e39, 0x718f19ab, 0x45d60668, 0x4f34375, 0x18f9216e, 0x72c23683, 0x480c };

    for (int i = 0; i < 1024; i++) {

        fe25519_mul(input, input, input);
    }

    fe25519_strong_reduce(input, input);
    FAIL_IF_MEMCMP(expected, input, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Invert_Canonical_CorrectlyInvert) {

    fe25519 input = { 0x7fff7fed - 0x1234, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };
    fe25519 output;
    fe25519 product;
    fe25519 expected_product = { 1 };

    fe25519_invert(output, input);
    fe25519_mul(product, input, output);
    fe25519_strong_reduce(product, product);

    FAIL_IF_MEMCMP(expected_product, product, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Invert_NonCanonical_CorrectlyInvert) {

    fe25519 input = { 0x7fff7fda - 0x1234, 0x7fff7fff - 0x1234, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff - 0x1234, 0x17fff };
    fe25519 output;
    fe25519 product;
    fe25519 expected_product = { 1 };

    fe25519_invert(output, input);
    fe25519_mul(product, input, output);
    fe25519_strong_reduce(product, product);

    FAIL_IF_MEMCMP(expected_product, product, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, I_SquareI_GetMinusOne) {

    fe25519 output;
    fe25519 canonical = { 0x7fff7fed - 1, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff7fff, 0x7fff };

    fe25519_square(output, fe25519_i);
    fe25519_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, RaiseToPowerPMinusFiveOverEight_ExpontentiateCorrectly) {

    fe25519 input = { 0x7a48348e, 0x2c25457, 0x1d27558f, 0x4e7e5200, 0x6e266a12, 0x608c7055, 0x7b5f5937, 0x6476eee, 0x12cf };
    fe25519 output;
    fe25519 expected = { 0x12bc389d, 0x4eaf45e2, 0x27745491, 0x52b27741, 0x690f7754, 0x55590ab1, 0x233a1550, 0x3de42b04, 0x12fe };
    fe25519_exp_p_minus_5_over_8(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}
