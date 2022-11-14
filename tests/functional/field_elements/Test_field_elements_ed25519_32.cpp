#include <CppUTest/TestHarness.h>
#include <field_elements/field_elements.h>
#include <field_elements/field_elements_ed25519_32.h>

TEST_GROUP(FIELD_ELEMENTS_ED25519_32) {

};

TEST(FIELD_ELEMENTS_ED25519_32, CmovFlagCleared_NoOp) {

    fe input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    fe output = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    /* Expect output to remain untouched */
    fe expected = output;
    fe_conditional_move(&output, &input, 0);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, CmovFlagSet_DoMove) {

    fe input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    fe output = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    /* Expect a move to take place */
    fe expected = input;
    fe_conditional_move(&output, &input, 1);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementInOkRange_NoOp) {

    fe input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    fe expected = input;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementInOkRange_NoOp) {

    fe input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    fe expected = input;
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementInOkRangeWithOverflowInLimb_Normalize) {

    fe input = { 1, 2, 3, 4, 5, 6, 7, 8, 0x4000000, 10 };
    fe expected = { 1, 2, 3, 4, 5, 6, 7, 8, 0, 11 };
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementInOkRangeWithOverflowInLimb_Normalize) {

    fe input = { 1, 2, 3, 4, 5, 6, 7, 8, 0x4000000, 10 };
    fe expected = { 1, 2, 3, 4, 5, 6, 7, 8, 0, 11 };
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementEqualToModulus_NormalizeToZero) {

    fe input = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe expected = fe_zero;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementEqualToModulus_NoOp) {

    fe input = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe expected = input;
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementEqualToTwiceTheModulus_NormalizeToZero) {

    fe input = { 0x7ffffda, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe };
    fe expected = fe_zero;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementEqualToTwiceTheModulus_ReduceToModulus) {

    fe input = { 0x7ffffda, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe };
    fe expected = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementEqualToFourTimesTheModulus_NormalizeToZero) {

    fe input = { 0xfffffb4, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc };
    fe expected = fe_zero;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementEqualToFourTimesTheModulus_ReduceToModulus) {

    fe input = { 0xfffffb4, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc };
    fe expected = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementEqualToOneMoreThanTheModulus_ReduceToOne) {

    fe input = { 0x3ffffed + 1, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe expected = fe_one;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementEqualToOneMoreThanTheModulus_NoOp) {

    fe input = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe expected = input;
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementEqualToOneLessThanTheModulus_NoOp) {

    fe input = { 0x3ffffed - 1, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe expected = input;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementEqualToOneLessThanTheModulus_NoOp) {

    fe input = { 0x3ffffed - 1, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe expected = input;
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementWithLargeOverflowInHighLimb_NormalizeSuccessfully) {

    fe input = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff + 0xffffff };
    fe expected = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xffffff };
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementWithOverflowInEachLimb_NormalizeSuccessfully) {

    fe input = {
        0x3ffffed + 0xffff,
        0x1ffffff + 0xffff,
        0x3ffffff + 0xffff,
        0x1ffffff + 0xffff,
        0x3ffffff + 0xffff,
        0x1ffffff + 0xffff,
        0x3ffffff + 0xffff,
        0x1ffffff + 0xffff,
        0x3ffffff + 0xffff,
        0x1ffffff + 0xffff
    };
    fe expected = {
        0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
    };
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementWithMaxOverflowInEachLimb_NormalizeSuccessfully) {

    fe input = {
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
    };
    fe expected = { 0x97f, 0x3f, 0x7f, 0x3f, 0x7f, 0x3f, 0x7f, 0x3f, 0x7f, 0x3f };
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, Equal_ElementsEqual_ReturnTrue) {

    fe input1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    fe input2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    int equal = fe_equal(&input1, &input2);
    CHECK_EQUAL(1, equal);
}

TEST(FIELD_ELEMENTS_ED25519_32, Equal_ElementsDifferent_ReturnFalse) {

    fe input1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    fe input2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 11 };
    int equal = fe_equal(&input1, &input2);
    CHECK_EQUAL(0, equal);
}

TEST(FIELD_ELEMENTS_ED25519_32, Equal_ElementsCongruentButNotEqual_ReturnFalse) {

    fe input1 = {
        0x3ffffed + 1,
        0x1ffffff + 2,
        0x3ffffff + 3,
        0x1ffffff + 4,
        0x3ffffff + 5,
        0x1ffffff + 6,
        0x3ffffff + 7,
        0x1ffffff + 8,
        0x3ffffff + 9,
        0x1ffffff + 10,
    };
    fe input2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    int equal = fe_equal(&input1, &input2);
    CHECK_EQUAL(0, equal);
    /* Do a reduction of input1 and check again */
    fe_strong_reduce(&input1, &input1);
    equal = fe_equal(&input1, &input2);
    MEMCMP_EQUAL(&input1, &input2, sizeof(fe));
    CHECK_EQUAL(1, equal);
}

TEST(FIELD_ELEMENTS_ED25519_32, Add_OnePlusOne_EqualsTwo) {

    fe input1 = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe input2 = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe expected = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe output;
    fe_add(&output, &input1, &input2);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, Add_ModulusPlusModulus_EqualsTwoModuli) {

    fe input1 = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe input2 = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe expected = { 0x7ffffda, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe };
    fe output;
    fe_add(&output, &input1, &input2);

    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, Sub_ModulusMinusModulus_CongruentToZero) {

    fe input1 = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe input2 = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe canonical = {};
    fe output;
    fe_sub(&output, &input1, &input2);
    /* Expect subtraction to result in a non-canonical form, e.g. for a - b we may expect to get a + 2p - b */
    /* Do a reduction and check the canonical form */
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, Sub_OneMinusTwo_CongruentToModulusMinusOne) {

    fe input1 = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe input2 = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe canonical = { 0x3ffffed - 1, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe output;
    fe_sub(&output, &input1, &input2);
    /* Expect subtraction to result in a non-canonical form, e.g. for a - b we may expect to get a + 2p - b */
    /* Do a reduction and check the canonical form */
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, Mul_ModulusPlusOneTimesTwo_CongruentToTwo) {

    fe input1 = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe input2 = { 0x3ffffed + 1, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe canonical = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe output;
    /* Expect multiplication to result in a non-canonical form */
    fe_mul(&output, &input1, &input2);
    /* Do a reduction and check the canonical form */
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, Mul_ModulusTimesModulus_CongruentToZero) {

    fe input1 = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe input2 = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe canonical = fe_zero;
    fe output;
    /* Expect multiplication to result in a non-canonical form */
    fe_mul(&output, &input1, &input2);
    /* Do a reduction and check the canonical form */
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, Mul_TwiceTheModulusTimesFourTimesTheModulusPlusFive_CongruentToZero) {

    fe input1 = { 0x7ffffda, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe };
    fe input2 = { 0xfffffb4 + 5, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc };
    fe canonical = fe_zero;
    fe output;
    /* Expect multiplication to result in a non-canonical form */
    fe_mul(&output, &input1, &input2);
    /* Do a reduction and check the canonical form */
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, Mul_FourModuliSquared_CongruentToZero) {

    fe input = { 0xfffffb4, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc };
    fe canonical = fe_zero;
    fe output;
    fe_mul(&output, &input, &input);
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, Mul_ZeroTimesAnything_EqualToZero) {

    fe input1 = fe_zero;
    fe input2 = { 14, 0, 0, 0, 123, 0, 0, 1, 2, 3 };
    fe expected = fe_zero;
    fe output;
    fe_mul(&output, &input1, &input2);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, Mul_StabilityTest_RemainBoundedAndProduceCorrectResult) {

    /* Check the stability of multiplication, i.e. that the overflow does not grow without bounds */
    fe input = { 0xfffffb40 + 0x100, 0x7fffffc0, 0xffffffc0, 0x7fffffc0, 0xffffffc0, 0x7fffffc0, 0xffffffc0, 0x7fffffc0, 0xffffffc0, 0x7fffffc0 };
    fe accumulator = fe_one;
    fe expected = { 0x0, 0xc12040, 0x28c2c7a, 0x10bf314, 0x1fac6d8, 0x18e6ff2, 0x7a3f };

    for (int i = 0; i < 1024; i++) {

        fe_mul(&accumulator, &accumulator, &input);

        /* Check that the result is always bounded by 2*p */
        CHECK_TRUE(accumulator.ed25519[0] <  0x7ffffda);
        CHECK_TRUE(accumulator.ed25519[1] <= 0x3fffffe);
        CHECK_TRUE(accumulator.ed25519[2] <= 0x7fffffe);
        CHECK_TRUE(accumulator.ed25519[3] <= 0x3fffffe);
        CHECK_TRUE(accumulator.ed25519[4] <= 0x7fffffe);
        CHECK_TRUE(accumulator.ed25519[5] <= 0x3fffffe);
        CHECK_TRUE(accumulator.ed25519[6] <= 0x7fffffe);
        CHECK_TRUE(accumulator.ed25519[7] <= 0x3fffffe);
        CHECK_TRUE(accumulator.ed25519[8] <= 0x7fffffe);
        CHECK_TRUE(accumulator.ed25519[9] <= 0x3fffffe);
    }

    MEMCMP_EQUAL(&expected, &accumulator, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, Mul_StabilityTest2_RemainBoundedAndProduceCorrectResult) {

    /* Check the stability of multiplication, i.e. that the overflow does not grow without bounds */
    fe input = { 0x7ffffda + 0x100, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe };
    fe expected = { 0x31cd790, 0x1f8f090, 0x1638e07, 0x19b2c96, 0x1ea9baf, 0xe89a80, 0xb4c78f, 0x12f8e2b, 0x2ca5638, 0xbcc46d };

    for (int i = 0; i < 1024; i++) {

        fe_mul(&input, &input, &input);

        /* Check that the result is always bounded by 2*p */
        CHECK_TRUE(input.ed25519[0] <  0x7ffffda);
        CHECK_TRUE(input.ed25519[1] <= 0x3fffffe);
        CHECK_TRUE(input.ed25519[2] <= 0x7fffffe);
        CHECK_TRUE(input.ed25519[3] <= 0x3fffffe);
        CHECK_TRUE(input.ed25519[4] <= 0x7fffffe);
        CHECK_TRUE(input.ed25519[5] <= 0x3fffffe);
        CHECK_TRUE(input.ed25519[6] <= 0x7fffffe);
        CHECK_TRUE(input.ed25519[7] <= 0x3fffffe);
        CHECK_TRUE(input.ed25519[8] <= 0x7fffffe);
        CHECK_TRUE(input.ed25519[9] <= 0x3fffffe);
    }

    MEMCMP_EQUAL(&expected, &input, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, Invert_Canonical_CorrectlyInvert) {

    fe input = { 0xdead, 0xbeef, 0xcafe, 0xdeed, 0xface, 0xdead, 0xbeef, 0xcafe, 0xdeed, 0xface };
    fe output;
    fe product;
    fe expected_product = fe_one;

    fe_invert(&output, &input);
    fe_mul(&product, &input, &output);
    fe_strong_reduce(&product, &product);

    MEMCMP_EQUAL(&expected_product, &product, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, Invert_NonCanonical_CorrectlyInvert) {

    fe input = {
        0x3ffffed - 1,
        0x1ffffff + 2,
        0x3ffffff - 3,
        0x1ffffff + 4,
        0x3ffffff - 5,
        0x1ffffff + 0xfff,
        0x3ffffff - 0xfff,
        0x1ffffff + 0xfff,
        0x3ffffff + 0xfff,
        0x1ffffff + 0xfff
    };
    fe output;
    fe product;
    fe expected_product = fe_one;

    fe_invert(&output, &input);
    fe_mul(&product, &input, &output);
    fe_strong_reduce(&product, &product);

    MEMCMP_EQUAL(&expected_product, &product, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED25519_32, I_SquareI_GetMinusOne) {

    fe output;
    fe canonical = { 0x3ffffed - 1, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };

    fe_square(&output, &fe_i);
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}
