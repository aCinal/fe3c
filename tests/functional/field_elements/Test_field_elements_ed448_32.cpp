#include <CppUTest/TestHarness.h>
#include <field_elements/field_elements.h>
#include <field_elements/field_elements_ed448_32.h>

TEST_GROUP(FIELD_ELEMENTS_ED448_32) {

};

TEST(FIELD_ELEMENTS_ED448_32, CmovFlagCleared_NoOp) {

    fe input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    fe output = { 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    /* Expect output to remain untouched */
    fe expected = output;
    fe_conditional_move(&output, &input, 0);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, CmovFlagSet_DoMove) {

    fe input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    fe output = { 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    /* Expect a move to take place */
    fe expected = input;
    fe_conditional_move(&output, &input, 1);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, StrongReduce_ElementInOkRange_NoOp) {

    fe input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    fe expected = input;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, WeakReduce_ElementInOkRange_NoOp) {

    fe input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    fe expected = input;
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, StrongReduce_ElementInOkRangeWithOverflowInLimb_Normalize) {

    fe input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, (1 << 28), 16 };
    fe expected = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 17 };
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, WeakReduce_ElementInOkRangeWithOverflowInLimb_Normalize) {

    fe input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, (1 << 28), 16 };
    fe expected = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 17 };
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, StrongReduce_ElementEqualToModulus_NormalizeToZero) {

    fe input = {
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff
    };
    fe expected = fe_zero;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, WeakReduce_ElementEqualToModulus_NoOp) {

    fe input = {
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff
    };
    fe expected = input;
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, StrongReduce_ElementEqualToTwiceTheModulus_NormalizeToZero) {

    fe input = {
        0x1ffffffe, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe,
        0x1ffffffe, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe,
        0x1ffffffc, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe,
        0x1ffffffe, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe
    };
    fe expected = fe_zero;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, WeakReduce_ElementEqualToTwiceTheModulus_ReduceToModulus) {

    fe input = {
        0x1ffffffe, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe,
        0x1ffffffe, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe,
        0x1ffffffc, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe,
        0x1ffffffe, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe
    };
    fe expected = {
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff
    };
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, StrongReduce_ElementEqualToFourTimesTheModulus_NormalizeToZero) {

    fe input = {
        0x3ffffffc, 0x3ffffffc, 0x3ffffffc, 0x3ffffffc,
        0x3ffffffc, 0x3ffffffc, 0x3ffffffc, 0x3ffffffc,
        0x3ffffff8, 0x3ffffffc, 0x3ffffffc, 0x3ffffffc,
        0x3ffffffc, 0x3ffffffc, 0x3ffffffc, 0x3ffffffc
    };
    fe expected = fe_zero;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, WeakReduce_ElementEqualToFourTimesTheModulus_ReduceToModulus) {

    fe input = {
        0x3ffffffc, 0x3ffffffc, 0x3ffffffc, 0x3ffffffc,
        0x3ffffffc, 0x3ffffffc, 0x3ffffffc, 0x3ffffffc,
        0x3ffffff8, 0x3ffffffc, 0x3ffffffc, 0x3ffffffc,
        0x3ffffffc, 0x3ffffffc, 0x3ffffffc, 0x3ffffffc
    };
    fe expected = {
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff
    };
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, StrongReduce_ElementEqualToOneMoreThanTheModulus_ReduceToOne) {

    fe input = {
        0xfffffff + 1, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff,     0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe,     0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff,     0xfffffff, 0xfffffff, 0xfffffff
    };
    fe expected = fe_one;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, WeakReduce_ElementEqualToOneMoreThanTheModulus_HandleOverflowsButDoNotReduce) {

    /* Note that in 28-bit reduced-radix representation the least-significant limb is fully occupied by ones when representing the field modulus.
     * The correct behaviour of fe_weak_reduce() in this case is to distribute the overflows, i.e. normalize the reduced-radix representation,
     * but do not reduce the number modulo the field modulus. */
    fe input = {
        0xfffffff + 1, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff,     0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe,     0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff,     0xfffffff, 0xfffffff, 0xfffffff
    };
    fe expected = {
        0x0000000, 0x0000000, 0x0000000, 0x0000000,
        0x0000000, 0x0000000, 0x0000000, 0x0000000,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff
    };
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, StrongReduce_ElementEqualToOneLessThanTheModulus_NoOp) {

    fe input = {
        0xfffffff - 1, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff,     0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe,     0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff,     0xfffffff, 0xfffffff, 0xfffffff
    };
    fe expected = input;
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, WeakReduce_ElementEqualToOneLessThanTheModulus_NoOp) {

    fe input = {
        0xfffffff - 1, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff,     0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe,     0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff,     0xfffffff, 0xfffffff, 0xfffffff
    };
    fe expected = input;
    fe output;
    fe_weak_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, StrongReduce_ElementWithLargeOverflowInHighLimb_NormalizeSuccessfully) {

    fe input = {
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff + 0xffffff
    };
    fe expected = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xffffff };
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, StrongReduce_ElementWithOverflowInEachLimb_NormalizeSuccessfully) {

    fe input = {
        0xfffffff + 0xffffff, 0xfffffff + 0xffffff, 0xfffffff + 0xffffff, 0xfffffff + 0xffffff,
        0xfffffff + 0xffffff, 0xfffffff + 0xffffff, 0xfffffff + 0xffffff, 0xfffffff + 0xffffff,
        0xffffffe + 0xffffff, 0xfffffff + 0xffffff, 0xfffffff + 0xffffff, 0xfffffff + 0xffffff,
        0xfffffff + 0xffffff, 0xfffffff + 0xffffff, 0xfffffff + 0xffffff, 0xfffffff + 0xffffff
    };
    fe expected = {
        0xffffff, 0xffffff, 0xffffff, 0xffffff,
        0xffffff, 0xffffff, 0xffffff, 0xffffff,
        0xffffff, 0xffffff, 0xffffff, 0xffffff,
        0xffffff, 0xffffff, 0xffffff, 0xffffff
    };
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, StrongReduce_ElementWithMaxOverflowInEachLimb_NormalizeSuccessfully) {

    fe input = {
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
    };
    fe expected = {
        0xf, 0xf, 0xf, 0xf,
        0xf, 0xf, 0xf, 0xf,
        0x1f, 0xf, 0xf, 0xf,
        0xf, 0xf, 0xf, 0xf
    };
    fe output;
    fe_strong_reduce(&output, &input);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, Equal_ElementsEqual_ReturnTrue) {

    fe input1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    fe input2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    int equal = fe_equal(&input1, &input2);
    CHECK_EQUAL(1, equal);
}

TEST(FIELD_ELEMENTS_ED448_32, Equal_ElementsDifferent_ReturnFalse) {

    fe input1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17 };
    fe input2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    int equal = fe_equal(&input1, &input2);
    CHECK_EQUAL(0, equal);
}

TEST(FIELD_ELEMENTS_ED448_32, Equal_ElementsCongruentButNotEqual_ReturnFalse) {

    fe input1 = {
        0xfffffff +  1, 0xfffffff +  2, 0xfffffff +  3, 0xfffffff +  4,
        0xfffffff +  5, 0xfffffff +  6, 0xfffffff +  7, 0xfffffff +  8,
        0xffffffe +  9, 0xfffffff + 10, 0xfffffff + 11, 0xfffffff + 12,
        0xfffffff + 13, 0xfffffff + 14, 0xfffffff + 15, 0xfffffff + 16
    };
    fe input2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    int equal = fe_equal(&input1, &input2);
    CHECK_EQUAL(0, equal);
    /* Do a reduction of input1 and check again */
    fe_strong_reduce(&input1, &input1);
    equal = fe_equal(&input1, &input2);
    MEMCMP_EQUAL(&input1, &input2, sizeof(fe));
    CHECK_EQUAL(1, equal);
}

TEST(FIELD_ELEMENTS_ED448_32, Add_OnePlusOne_EqualsTwo) {

    fe input1 = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe input2 = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe expected = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe output;
    fe_add(&output, &input1, &input2);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, Add_ModulusPlusModulus_EqualsTwoModuli) {

    fe input1 = {
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff
    };
    fe input2 = {
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff
    };
    fe expected = {
        0x1ffffffe, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe,
        0x1ffffffe, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe,
        0x1ffffffc, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe,
        0x1ffffffe, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe
    };
    fe output;
    fe_add(&output, &input1, &input2);

    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, Sub_ModulusMinusModulus_CongruentToZero) {

    fe input1 = {
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff
    };
    fe input2 = {
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff
    };
    fe canonical = fe_zero;
    fe output;
    fe_sub(&output, &input1, &input2);
    /* Expect subtraction to result in a non-canonical form, e.g. for a - b we may expect to get a + 2p - b */
    /* Do a reduction and check the canonical form */
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, Sub_OneMinusTwo_CongruentToModulusMinusOne) {

    fe input1 = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe input2 = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe canonical = {
        0xfffffff - 1, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff,     0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe,     0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff,     0xfffffff, 0xfffffff, 0xfffffff
    };
    fe output;
    fe_sub(&output, &input1, &input2);
    /* Expect subtraction to result in a non-canonical form, e.g. for a - b we may expect to get a + 2p - b */
    /* Do a reduction and check the canonical form */
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, Mul_ModulusPlusOneTimesTwo_CongruentToTwo) {

    fe input1 = {
        0xfffffff + 1, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff,     0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe,     0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff,     0xfffffff, 0xfffffff, 0xfffffff
    };
    fe input2 = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe canonical = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe output;
    /* Expect multiplication to result in a non-canonical form */
    fe_mul(&output, &input1, &input2);
    /* Do a reduction and check the canonical form */
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, Mul_ModulusTimesModulus_CongruentToZero) {

    fe input1 = {
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff
    };
    fe input2 = {
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff
    };
    fe canonical = fe_zero;
    fe output;
    /* Expect multiplication to result in a non-canonical form */
    fe_mul(&output, &input1, &input2);
    /* Do a reduction and check the canonical form */
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, Mul_TwiceTheModulusTimesFourTimesTheModulusPlusFive_CongruentToZero) {

    fe input1 = {
        0x1ffffffe, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe,
        0x1ffffffe, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe,
        0x1ffffffc, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe,
        0x1ffffffe, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe
    };
    fe input2 = {
        0x3ffffffc + 5, 0x3ffffffc, 0x3ffffffc, 0x3ffffffc,
        0x3ffffffc,     0x3ffffffc, 0x3ffffffc, 0x3ffffffc,
        0x3ffffff8,     0x3ffffffc, 0x3ffffffc, 0x3ffffffc,
        0x3ffffffc,     0x3ffffffc, 0x3ffffffc, 0x3ffffffc
    };
    fe canonical = fe_zero;
    fe output;
    /* Expect multiplication to result in a non-canonical form */
    fe_mul(&output, &input1, &input2);
    /* Do a reduction and check the canonical form */
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, Mul_ThreeModuliSquared_CongruentToZero) {

    fe input = {
        0x2ffffffd, 0x2ffffffd, 0x2ffffffd, 0x2ffffffd,
        0x2ffffffd, 0x2ffffffd, 0x2ffffffd, 0x2ffffffd,
        0x2ffffffa, 0x2ffffffd, 0x2ffffffd, 0x2ffffffd,
        0x2ffffffd, 0x2ffffffd, 0x2ffffffd, 0x2ffffffd
    };
    fe canonical = fe_zero;
    fe output;
    fe_mul(&output, &input, &input);
    fe_strong_reduce(&output, &output);
    MEMCMP_EQUAL(&canonical, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, Mul_ZeroTimesAnything_EqualToZero) {

    fe input1 = fe_zero;
    fe input2 = { 14, 0, 0, 0, 123, 2, 3, 4, 5, 6, 7, 19, 20, 21, 22, 23 };
    fe expected = fe_zero;
    fe output;
    fe_mul(&output, &input1, &input2);
    MEMCMP_EQUAL(&expected, &output, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, Mul_StabilityTest_RemainBoundedAndProduceCorrectResult) {

    /* Check the stability of multiplication, i.e. that the overflow does not grow without bounds */
    fe input = {
        0x3ffffffc + 0xfff, 0x3ffffffc, 0x3ffffffc, 0x3ffffffc,
        0x3ffffffc,         0x3ffffffc, 0x3ffffffc, 0x3ffffffc,
        0x3ffffff8,         0x3ffffffc, 0x3ffffffc, 0x3ffffffc,
        0x3ffffffc,         0x3ffffffc, 0x3ffffffc, 0x3ffffffc
    };
    fe accumulator = fe_one;
    fe expected = {
        0x39cb9f, 0xd553276, 0x3d1f9f9, 0x9f8bb93,
        0x9db2f26, 0x673958f, 0xb29772a, 0x81f9f69,
        0x28b4ec6, 0xf7d15bb, 0x38d4625, 0x45884ee,
        0x1d67792, 0x149373d, 0x3d70f3b, 0x68b3845
    };

    for (int i = 0; i < 1024; i++) {

        fe_mul(&accumulator, &accumulator, &input);

        /* Check that the result is always bounded by 2*p */
        CHECK_TRUE(accumulator.ed448[ 0] <  0x1ffffffe);
        CHECK_TRUE(accumulator.ed448[ 1] <= 0x1ffffffe);
        CHECK_TRUE(accumulator.ed448[ 2] <= 0x1ffffffe);
        CHECK_TRUE(accumulator.ed448[ 3] <= 0x1ffffffe);
        CHECK_TRUE(accumulator.ed448[ 4] <= 0x1ffffffe);
        CHECK_TRUE(accumulator.ed448[ 5] <= 0x1ffffffe);
        CHECK_TRUE(accumulator.ed448[ 6] <= 0x1ffffffe);
        CHECK_TRUE(accumulator.ed448[ 7] <= 0x1ffffffe);
        CHECK_TRUE(accumulator.ed448[ 8] <= 0x1ffffffc);
        CHECK_TRUE(accumulator.ed448[ 9] <= 0x1ffffffe);
        CHECK_TRUE(accumulator.ed448[10] <= 0x1ffffffe);
        CHECK_TRUE(accumulator.ed448[11] <= 0x1ffffffe);
        CHECK_TRUE(accumulator.ed448[12] <= 0x1ffffffe);
        CHECK_TRUE(accumulator.ed448[13] <= 0x1ffffffe);
        CHECK_TRUE(accumulator.ed448[14] <= 0x1ffffffe);
        CHECK_TRUE(accumulator.ed448[15] <= 0x1ffffffe);
    }

    MEMCMP_EQUAL(&expected, &accumulator, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, Mul_StabilityTest2_RemainBoundedAndProduceCorrectResult) {

    /* Check the stability of multiplication, i.e. that the overflow does not grow without bounds */
    fe input = {
        0x1ffffffe + 0xfff, 0x1ffffffe, 0x1ffffffe, 0x1ffffffe,
        0x1ffffffe,         0x1ffffffe, 0x1ffffffe, 0x1ffffffe,
        0x1ffffffc,         0x1ffffffe, 0x1ffffffe, 0x1ffffffe,
        0x1ffffffe,         0x1ffffffe, 0x1ffffffe, 0x1ffffffe
    };
    fe expected = {
        0x9a3d0a0, 0x6ba5e72, 0x60956d2, 0x37ab768,
        0xd4d74d1, 0xfb8c6b9, 0xf298eb3, 0xc0c31d2,
        0x39eb81b, 0xde2de3c, 0x80656dc, 0xae3532f,
        0x399fea6, 0xc2ac26, 0x932467c, 0xbd3c1cd
    };

    for (int i = 0; i < 1024; i++) {

        fe_mul(&input, &input, &input);

        /* Check that the result is always bounded by 2*p */
        CHECK_TRUE(input.ed448[ 0] <  0x1ffffffe);
        CHECK_TRUE(input.ed448[ 1] <= 0x1ffffffe);
        CHECK_TRUE(input.ed448[ 2] <= 0x1ffffffe);
        CHECK_TRUE(input.ed448[ 3] <= 0x1ffffffe);
        CHECK_TRUE(input.ed448[ 4] <= 0x1ffffffe);
        CHECK_TRUE(input.ed448[ 5] <= 0x1ffffffe);
        CHECK_TRUE(input.ed448[ 6] <= 0x1ffffffe);
        CHECK_TRUE(input.ed448[ 7] <= 0x1ffffffe);
        CHECK_TRUE(input.ed448[ 8] <= 0x1ffffffc);
        CHECK_TRUE(input.ed448[ 9] <= 0x1ffffffe);
        CHECK_TRUE(input.ed448[10] <= 0x1ffffffe);
        CHECK_TRUE(input.ed448[11] <= 0x1ffffffe);
        CHECK_TRUE(input.ed448[12] <= 0x1ffffffe);
        CHECK_TRUE(input.ed448[13] <= 0x1ffffffe);
        CHECK_TRUE(input.ed448[14] <= 0x1ffffffe);
        CHECK_TRUE(input.ed448[15] <= 0x1ffffffe);
    }

    MEMCMP_EQUAL(&expected, &input, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, Invert_Canonical_CorrectlyInvert) {

    fe input = {
        0xdead, 0xbeef, 0xcafe, 0xdeed,
        0xface, 0xdead, 0xbeef, 0xcafe,
        0xdead, 0xbeef, 0xcafe, 0xdeed,
        0xface, 0xdead, 0xbeef, 0xcafe
    };
    fe output;
    fe product;
    fe expected_product = fe_one;

    fe_invert(&output, &input);
    fe_mul(&product, &input, &output);
    fe_strong_reduce(&product, &product);

    MEMCMP_EQUAL(&expected_product, &product, sizeof(fe));
}

TEST(FIELD_ELEMENTS_ED448_32, Invert_NonCanonical_CorrectlyInvert) {

    fe input = {
        0xfffffff -  1, 0xfffffff +  2, 0xfffffff -  3, 0xfffffff +  4,
        0xfffffff -  5, 0xfffffff +  6, 0xfffffff -  7, 0xfffffff +  8,
        0xffffffe -  9, 0xfffffff + 10, 0xfffffff - 11, 0xfffffff + 12,
        0xfffffff - 13, 0xfffffff + 14, 0xfffffff - 15, 0xfffffff + 16
    };
    fe output;
    fe product;
    fe expected_product = fe_one;

    fe_invert(&output, &input);
    fe_mul(&product, &input, &output);
    fe_strong_reduce(&product, &product);

    MEMCMP_EQUAL(&expected_product, &product, sizeof(fe));
}
