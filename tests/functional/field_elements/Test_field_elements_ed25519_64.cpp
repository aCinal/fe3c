#include <CppUTest/TestHarness.h>
#include <field_elements/field_elements.h>
#include <field_elements/field_elements_ed25519_64.h>

TEST_GROUP(FIELD_ELEMENTS_ED25519_64) {

};

TEST(FIELD_ELEMENTS_ED25519_64, CmovFlagCleared_NoOp) {

    fe input = { 0x1122334455667, 0x1998877665544, 0x1deadbeefcafe, 0x1cafebeefcafe, 0x1234567891234 };
    fe output = { 0x1234567891234, 0x1cafebeefcafe, 0x1deadbeefcafe, 0x1998877665544, 0x1122334455667 };
    /* Expect output to remain untouched */
    fe expected = output;
    fe_conditional_move(&output, &input, 0);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, CmovFlagSet_DoMove) {

    fe input = { 0x1122334455667, 0x1998877665544, 0x1deadbeefcafe, 0x1cafebeefcafe, 0x1234567891234 };
    fe output = { 0x1234567891234, 0x1cafebeefcafe, 0x1deadbeefcafe, 0x1998877665544, 0x1122334455667 };
    /* Expect a move to take place */
    fe expected = input;
    fe_conditional_move(&output, &input, 1);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, StrongReduce_ElementInOkRange_NoOp) {

    fe input = { 0x1122334455667, 0x1998877665544, 0x1deadbeefcafe, 0x1cafebeefcafe, 0x1234567891234 };
    fe expected = input;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, WeakReduce_ElementInOkRange_NoOp) {

    fe input = { 0x1122334455667, 0x1998877665544, 0x1deadbeefcafe, 0x1cafebeefcafe, 0x1234567891234 };
    fe expected = input;
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, StrongReduce_ElementInOkRangeWithOverflowInLimb_Normalize) {

    fe input = { 0x1122334455667, 0x1998877665544, 0x2deadbeefcafe, 0x9cafebeefcafe, 0x1234567891234 };
    fe expected = { 0x1122334455667, 0x1998877665544, 0x2deadbeefcafe, 0x1cafebeefcafe, 0x1234567891235 };
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, WeakReduce_ElementInOkRangeWithOverflowInLimb_Normalize) {

    fe input = { 0x1122334455667, 0x1998877665544, 0x2deadbeefcafe, 0x9cafebeefcafe, 0x1234567891234 };
    fe expected = { 0x1122334455667, 0x1998877665544, 0x2deadbeefcafe, 0x1cafebeefcafe, 0x1234567891235 };
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, StrongReduce_ElementEqualToModulus_NormalizeToZero) {

    fe input = { 0x7ffffffffffedULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe expected = fe_zero;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, WeakReduce_ElementEqualToModulus_NoOp) {

    fe input = { 0x7ffffffffffedULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe expected = input;
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, StrongReduce_ElementEqualToTwiceTheModulus_NormalizeToZero) {

    fe input = { 0xfffffffffffdaULL, 0xffffffffffffeULL, 0xffffffffffffeULL, 0xffffffffffffeULL, 0xffffffffffffeULL };
    fe expected = fe_zero;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, WeakReduce_ElementEqualToTwiceTheModulus_ReduceToModulus) {

    fe input = { 0xfffffffffffdaULL, 0xffffffffffffeULL, 0xffffffffffffeULL, 0xffffffffffffeULL, 0xffffffffffffeULL };
    fe expected = { 0x7ffffffffffedULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, StrongReduce_ElementEqualToFourTimesTheModulus_NormalizeToZero) {

    fe input = { 0x1fffffffffffb4ULL, 0x1ffffffffffffcULL, 0x1ffffffffffffcULL, 0x1ffffffffffffcULL, 0x1ffffffffffffcULL };
    fe expected = fe_zero;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, WeakReduce_ElementEqualToFourTimesTheModulus_ReduceToModulus) {

    fe input = { 0x1fffffffffffb4ULL, 0x1ffffffffffffcULL, 0x1ffffffffffffcULL, 0x1ffffffffffffcULL, 0x1ffffffffffffcULL };
    fe expected = { 0x7ffffffffffedULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, StrongReduce_ElementEqualToOneMoreThanTheModulus_ReduceToOne) {

    fe input = { 0x7ffffffffffedULL + 1, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe expected = fe_one;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, WeakReduce_ElementEqualToOneMoreThanTheModulus_NoOp) {

    fe input = { 0x7ffffffffffedULL + 1, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe expected = input;
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, StrongReduce_ElementEqualToOneLessThanTheModulus_NoOp) {

    fe input = { 0x7ffffffffffedULL - 1, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe expected = input;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, WeakReduce_ElementEqualToOneLessThanTheModulus_NoOp) {

    fe input = { 0x7ffffffffffedULL - 1, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe expected = input;
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, StrongReduce_ElementWithLargeOverflowInHighLimb_NormalizeSuccessfully) {

    fe input = { 0x7ffffffffffedULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL + 0xffffffff };
    fe expected = { 0, 0, 0, 0, 0xffffffff };
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, StrongReduce_ElementWithOverflowInEachLimb_NormalizeSuccessfully) {

    fe input = { 0x7ffffffffffedULL + 0xffffffff, 0x7ffffffffffffULL + 0xffffffff, 0x7ffffffffffffULL + 0xffffffff, 0x7ffffffffffffULL + 0xffffffff, 0x7ffffffffffffULL + 0xffffffff };
    fe expected = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, StrongReduce_ElementWithMaxOverflowInEachLimb_NormalizeSuccessfully) {

    fe input = { 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff };
    fe expected = { 0x25fff, 0x1fff, 0x1fff, 0x1fff, 0x1fff };
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, Equal_ElementsEqual_ReturnTrue) {

    fe input1 = { 1, 2, 3, 4, 5 };
    fe input2 = { 1, 2, 3, 4, 5 };
    int equal = fe_equal(&input1, &input2);
    CHECK_EQUAL(1, equal);
}

TEST(FIELD_ELEMENTS_ED25519_64, Equal_ElementsDifferent_ReturnFalse) {

    fe input1 = { 1, 2, 3, 4, 6 };
    fe input2 = { 1, 2, 3, 4, 5 };
    int equal = fe_equal(&input1, &input2);
    CHECK_EQUAL(0, equal);
}

TEST(FIELD_ELEMENTS_ED25519_64, Equal_ElementsCongruentButNotEqual_ReturnFalse) {

    fe input1 = { 0x7ffffffffffedULL + 1, 0x7ffffffffffffULL + 2, 0x7ffffffffffffULL + 3, 0x7ffffffffffffULL + 4, 0x7ffffffffffffULL + 5 };
    fe input2 = { 1, 2, 3, 4, 5 };
    int equal = fe_equal(&input1, &input2);
    CHECK_EQUAL(0, equal);
    /* Do a reduction of input1 and check again */
    fe_strong_reduce(&input1, &input1);
    equal = fe_equal(&input1, &input2);
    MEMCMP_EQUAL(&input1, &input2, sizeof(fe));
    CHECK_EQUAL(1, equal);
}

TEST(FIELD_ELEMENTS_ED25519_64, Add_OnePlusOne_EqualsTwo) {

    fe input1 = { 1, 0, 0, 0, 0 };
    fe input2 = { 1, 0, 0, 0, 0 };
    fe expected = { 2, 0, 0, 0, 0 };
    fe output;
    fe_add(&output, &input1, &input2);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, Add_ModulusPlusModulus_EqualsTwoModuli) {

    fe input1 = { 0x7ffffffffffedULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe input2 = { 0x7ffffffffffedULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe expected = { 0xfffffffffffdaULL, 0xffffffffffffeULL, 0xffffffffffffeULL, 0xffffffffffffeULL, 0xffffffffffffeULL };
    fe output;
    fe_add(&output, &input1, &input2);

    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, Sub_ModulusMinusModulus_CongruentToZero) {

    fe input1 = { 0x7ffffffffffedULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe input2 = { 0x7ffffffffffedULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe canonical = {};
    fe output;
    fe_sub(&output, &input1, &input2);
    /* Expect subtraction to result in a non-canonical form, e.g. for a - b we may expect to get a + 2p - b */
    /* Do a reduction and check the canonical form */
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, Sub_OneMinusTwo_CongruentToModulusMinusOne) {

    fe input1 = { 1, 0, 0, 0, 0 };
    fe input2 = { 2, 0, 0, 0, 0 };
    fe canonical = { 0x7ffffffffffedULL - 1, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe output;
    fe_sub(&output, &input1, &input2);
    /* Expect subtraction to result in a non-canonical form, e.g. for a - b we may expect to get a + 2p - b */
    /* Do a reduction and check the canonical form */
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, Mul_ModulusPlusOneTimesTwo_CongruentToTwo) {

    fe input1 = { 2, 0, 0, 0, 0 };
    fe input2 = { 0x7ffffffffffedULL + 1, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe canonical = { 2, 0, 0, 0, 0 };
    fe output;
    /* Expect multiplication to result in a non-canonical form */
    fe_mul(&output, &input1, &input2);
    /* Do a reduction and check the canonical form */
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, Mul_ModulusTimesModulus_CongruentToZero) {

    fe input1 = { 0x7ffffffffffedULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe input2 = { 0x7ffffffffffedULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };
    fe canonical = fe_zero;
    fe output;
    /* Expect multiplication to result in a non-canonical form */
    fe_mul(&output, &input1, &input2);
    /* Do a reduction and check the canonical form */
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, Mul_TwiceTheModulusTimesFourTimesTheModulusPlusFive_CongruentToZero) {

    fe input1 = { 0xfffffffffffdaULL, 0xffffffffffffeULL, 0xffffffffffffeULL, 0xffffffffffffeULL, 0xffffffffffffeULL };
    fe input2 = { 0x1fffffffffffb4ULL + 5, 0x1ffffffffffffcULL, 0x1ffffffffffffcULL, 0x1ffffffffffffcULL, 0x1ffffffffffffcULL };
    fe canonical = fe_zero;
    fe output;
    /* Expect multiplication to result in a non-canonical form */
    fe_mul(&output, &input1, &input2);
    /* Do a reduction and check the canonical form */
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, Mul_HundredTwentyEightModuliSquared_CongruentToZero) {

    fe input = { 0x3fffffffffff680ULL, 0x3ffffffffffff80ULL, 0x3ffffffffffff80ULL, 0x3ffffffffffff80ULL, 0x3ffffffffffff80ULL };
    fe canonical = fe_zero;
    fe output;
    fe_mul(&output, &input, &input);
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, Mul_ZeroTimesAnything_EqualToZero) {

    fe input1 = fe_zero;
    fe input2 = { 14, 0, 0, 0, 123 };
    fe expected = fe_zero;
    fe output;
    fe_mul(&output, &input1, &input2);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, Mul_StabilityTest_RemainBoundedAndProduceCorrectResult) {

    /* Check the stability of multiplication, i.e. that the overflow does not grow without bounds */
    fe input = { 0x3fffffffffff680ULL + 0xfff, 0x3ffffffffffff80ULL, 0x3ffffffffffff80ULL, 0x3ffffffffffff80ULL, 0x3ffffffffffff80ULL };
    fe accumulator = fe_one;
    fe expected = { 0x2c993cf214452, 0x4e8f526816c0, 0x2fd5d6914466b, 0x2f2e524fc3a72, 0x67eb6d5f0b146 };

    for (int i = 0; i < 1024; i++) {

        fe_mul(&accumulator, &accumulator, &input);

        /* Check that the result is always bounded by 2*p */
        CHECK_TRUE(accumulator.ed25519[0] <  0xfffffffffffdaULL);
        CHECK_TRUE(accumulator.ed25519[1] <= 0xffffffffffffeULL);
        CHECK_TRUE(accumulator.ed25519[2] <= 0xffffffffffffeULL);
        CHECK_TRUE(accumulator.ed25519[3] <= 0xffffffffffffeULL);
        CHECK_TRUE(accumulator.ed25519[4] <= 0xffffffffffffeULL);
    }

    MEMCMP_EQUAL(&expected, &accumulator, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, Mul_StabilityTest2_RemainBoundedAndProduceCorrectResult) {

    /* Check the stability of multiplication, i.e. that the overflow does not grow without bounds */
    fe input = { 0x3fffffffffff680ULL + 0xfff, 0x3ffffffffffff80ULL, 0x3ffffffffffff80ULL, 0x3ffffffffffff80ULL, 0x3ffffffffffff80ULL };
    fe expected = { 0xf323f4d5de31, 0x4db9c9f6306c8, 0x513268d3581fa, 0x286fb13def6cc, 0x94e290d55ba6 };

    for (int i = 0; i < 1024; i++) {

        fe_mul(&input, &input, &input);

        /* Check that the result is always bounded by 2*p */
        CHECK_TRUE(input.ed25519[0] <  0xfffffffffffdaULL);
        CHECK_TRUE(input.ed25519[1] <= 0xffffffffffffeULL);
        CHECK_TRUE(input.ed25519[2] <= 0xffffffffffffeULL);
        CHECK_TRUE(input.ed25519[3] <= 0xffffffffffffeULL);
        CHECK_TRUE(input.ed25519[4] <= 0xffffffffffffeULL);
    }

    MEMCMP_EQUAL(&expected, &input, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, Invert_Canonical_CorrectlyInvert) {

    fe input = { 0xffff, 0x1234, 0xdeadbeef, 0xcafecafecafe, 0xcafecafecafe };
    fe output;
    fe product;
    fe expected_product = fe_one;

    fe_invert(&output, &input);
    fe_mul(&product, &input, &output);
    fe_strong_reduce(&product, &product);

    MEMCMP_EQUAL(&expected_product, &product, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, Invert_NonCanonical_CorrectlyInvert) {

    fe input = { 0x7ffffffffffedULL - 1, 0x7ffffffffffffULL + 4, 0x7ffffffffffffULL + 3, 0x7ffffffffffffULL + 5, 0x7ffffffffffffULL + 0xfff };
    fe output;
    fe product;
    fe expected_product = fe_one;

    fe_invert(&output, &input);
    fe_mul(&product, &input, &output);
    fe_strong_reduce(&product, &product);

    MEMCMP_EQUAL(&expected_product, &product, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_64, I_SquareI_GetMinusOne) {

    fe output;
    fe canonical = { 0x7ffffffffffedULL - 1, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL, 0x7ffffffffffffULL };

    fe_square(&output, &fe_i);
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}
