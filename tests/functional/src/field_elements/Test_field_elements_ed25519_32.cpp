#include <functional_tests.h>
#include <field_elements/field_elements_ed25519.h>

#if FE3C_FUNCTIONAL_TESTS_HOST
TEST_GROUP(FIELD_ELEMENTS_ED25519_32)
{
};
#endif /* FE3C_FUNCTIONAL_TESTS_HOST */

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, CmovFlagCleared_NoOp)
{
    fe25519 input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    fe25519 output = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    /* Expect output to remain untouched */
    fe25519 expected = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    fe25519_conditional_move(output, input, 0);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, CmovFlagSet_DoMove)
{
    fe25519 input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    fe25519 output = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    /* Expect a move to take place */
    fe25519 expected = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    fe25519_conditional_move(output, input, 1);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementInOkRange_NoOp)
{
    fe25519 input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    fe25519 expected = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    fe25519 output;
    fe25519_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementInOkRange_NoOp)
{
    fe25519 input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    fe25519 expected = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    fe25519 output;
    fe25519_weak_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementInOkRangeWithOverflowInLimb_Normalize)
{
    fe25519 input = { 1, 2, 3, 4, 5, 6, 7, 8, 0x4000000, 10 };
    fe25519 expected = { 1, 2, 3, 4, 5, 6, 7, 8, 0, 11 };
    fe25519 output;
    fe25519_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementInOkRangeWithOverflowInLimb_Normalize)
{
    fe25519 input = { 1, 2, 3, 4, 5, 6, 7, 8, 0x4000000, 10 };
    fe25519 expected = { 1, 2, 3, 4, 5, 6, 7, 8, 0, 11 };
    fe25519 output;
    fe25519_weak_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementEqualToModulus_NormalizeToZero)
{
    fe25519 input = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 expected = {};
    fe25519 output;
    fe25519_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementEqualToModulus_NoOp)
{
    fe25519 input = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 expected = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 output;
    fe25519_weak_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementEqualToTwiceTheModulus_NormalizeToZero)
{
    fe25519 input = { 0x7ffffda, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe };
    fe25519 expected = {};
    fe25519 output;
    fe25519_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementEqualToTwiceTheModulus_ReduceToModulus)
{
    fe25519 input = { 0x7ffffda, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe };
    fe25519 expected = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 output;
    fe25519_weak_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementEqualToFourTimesTheModulus_NormalizeToZero)
{
    fe25519 input = { 0xfffffb4, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc };
    fe25519 expected = {};
    fe25519 output;
    fe25519_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementEqualToFourTimesTheModulus_ReduceToModulus)
{
    fe25519 input = { 0xfffffb4, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc };
    fe25519 expected = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 output;
    fe25519_weak_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementEqualToOneMoreThanTheModulus_ReduceToOne)
{
    fe25519 input = { 0x3ffffed + 1, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 expected = { 1 };
    fe25519 output;
    fe25519_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementEqualToOneMoreThanTheModulus_NoOp)
{
    fe25519 input = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 expected = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 output;
    fe25519_weak_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementEqualToOneLessThanTheModulus_NoOp)
{
    fe25519 input = { 0x3ffffed - 1, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 expected = { 0x3ffffed - 1, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 output;
    fe25519_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, WeakReduce_ElementEqualToOneLessThanTheModulus_NoOp)
{
    fe25519 input = { 0x3ffffed - 1, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 expected = { 0x3ffffed - 1, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 output;
    fe25519_weak_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementWithLargeOverflowInHighLimb_NormalizeSuccessfully)
{
    fe25519 input = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff + 0xffffff };
    fe25519 expected = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xffffff };
    fe25519 output;
    fe25519_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementWithOverflowInEachLimb_NormalizeSuccessfully)
{
    fe25519 input = {
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
    fe25519 expected = {
        0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
    };
    fe25519 output;
    fe25519_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, StrongReduce_ElementWithMaxOverflowInEachLimb_NormalizeSuccessfully)
{
    fe25519 input = {
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
    };
    fe25519 expected = { 0x97f, 0x3f, 0x7f, 0x3f, 0x7f, 0x3f, 0x7f, 0x3f, 0x7f, 0x3f };
    fe25519 output;
    fe25519_strong_reduce(output, input);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Equal_ElementsEqual_ReturnTrue)
{
    fe25519 input1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    fe25519 input2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    int equal = fe25519_equal(input1, input2);
    FAIL_UNLESS_EQUAL(1, equal);
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Equal_ElementsDifferent_ReturnFalse)
{
    fe25519 input1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    fe25519 input2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 11 };
    int equal = fe25519_equal(input1, input2);
    FAIL_UNLESS_EQUAL(0, equal);
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Equal_ElementsCongruentButNotEqual_ReturnFalse)
{
    fe25519 input1 = {
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
    fe25519 input2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    int equal = fe25519_equal(input1, input2);
    FAIL_UNLESS_EQUAL(0, equal);
    /* Do a reduction of input1 and check again */
    fe25519_strong_reduce(input1, input1);
    equal = fe25519_equal(input1, input2);
    FAIL_IF_MEMCMP(input1, input2, sizeof(fe25519));
    FAIL_UNLESS_EQUAL(1, equal);
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Add_OnePlusOne_EqualsTwo)
{
    fe25519 input1 = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe25519 input2 = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe25519 expected = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe25519 output;
    fe25519_add(output, input1, input2);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Add_ModulusPlusModulus_EqualsTwoModuli)
{
    fe25519 input1 = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 input2 = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 expected = { 0x7ffffda, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe };
    fe25519 output;
    fe25519_add(output, input1, input2);

    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Sub_ModulusMinusModulus_CongruentToZero)
{
    fe25519 input1 = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 input2 = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 canonical = {};
    fe25519 output;
    fe25519_sub(output, input1, input2);
    /* Expect subtraction to result in a non-canonical form, e.g. for a - b we may expect to get a + 2p - b */
    /* Do a reduction and check the canonical form */
    fe25519_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Sub_OneMinusTwo_CongruentToModulusMinusOne)
{
    fe25519 input1 = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe25519 input2 = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe25519 canonical = { 0x3ffffed - 1, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 output;
    fe25519_sub(output, input1, input2);
    /* Expect subtraction to result in a non-canonical form, e.g. for a - b we may expect to get a + 2p - b */
    /* Do a reduction and check the canonical form */
    fe25519_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Mul_ModulusPlusOneTimesTwo_CongruentToTwo)
{
    fe25519 input1 = { 0x3ffffed + 1, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 input2 = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe25519 canonical = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    fe25519 output;
    /* Expect multiplication to result in a non-canonical form */
    fe25519_mul(output, input1, input2);
    /* Do a reduction and check the canonical form */
    fe25519_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Mul_ModulusTimesModulus_CongruentToZero)
{
    fe25519 input1 = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 input2 = { 0x3ffffed, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };
    fe25519 canonical = {};
    fe25519 output;
    /* Expect multiplication to result in a non-canonical form */
    fe25519_mul(output, input1, input2);
    /* Do a reduction and check the canonical form */
    fe25519_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Mul_TwiceTheModulusTimesFourTimesTheModulusPlusFive_CongruentToZero)
{
    fe25519 input1 = { 0x7ffffda, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe };
    fe25519 input2 = { 0xfffffb4 + 5, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc };
    fe25519 canonical = {};
    fe25519 output;
    /* Expect multiplication to result in a non-canonical form */
    fe25519_mul(output, input1, input2);
    /* Do a reduction and check the canonical form */
    fe25519_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Mul_FourModuliSquared_CongruentToZero)
{
    fe25519 input = { 0xfffffb4, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc, 0xffffffc, 0x7fffffc };
    fe25519 canonical = {};
    fe25519 output;
    fe25519_mul(output, input, input);
    fe25519_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Mul_ZeroTimesAnything_EqualToZero)
{
    fe25519 input1 = {};
    fe25519 input2 = { 14, 0, 0, 0, 123, 0, 0, 1, 2, 3 };
    fe25519 expected = {};
    fe25519 output;
    fe25519_mul(output, input1, input2);
    FAIL_IF_MEMCMP(expected, output, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Mul_StabilityTest_RemainBoundedAndProduceCorrectResult)
{
    /* Check the stability of multiplication, i.e. that the overflow does not grow without bounds */
    fe25519 input = { 0xfffffb40 + 0x100, 0x7fffffc0, 0xffffffc0, 0x7fffffc0, 0xffffffc0, 0x7fffffc0, 0xffffffc0, 0x7fffffc0, 0xffffffc0, 0x7fffffc0 };
    fe25519 accumulator = { 1 };
    fe25519 expected = { 0x0, 0xc12040, 0x28c2c7a, 0x10bf314, 0x1fac6d8, 0x18e6ff2, 0x7a3f };

    for (int i = 0; i < 1024; i++) {

        fe25519_mul(accumulator, accumulator, input);

        /* Check that the result is always bounded by 2*p */
        FAIL_UNLESS(accumulator[0] <  0x7ffffda);
        FAIL_UNLESS(accumulator[1] <= 0x3fffffe);
        FAIL_UNLESS(accumulator[2] <= 0x7fffffe);
        FAIL_UNLESS(accumulator[3] <= 0x3fffffe);
        FAIL_UNLESS(accumulator[4] <= 0x7fffffe);
        FAIL_UNLESS(accumulator[5] <= 0x3fffffe);
        FAIL_UNLESS(accumulator[6] <= 0x7fffffe);
        FAIL_UNLESS(accumulator[7] <= 0x3fffffe);
        FAIL_UNLESS(accumulator[8] <= 0x7fffffe);
        FAIL_UNLESS(accumulator[9] <= 0x3fffffe);
    }

    FAIL_IF_MEMCMP(expected, &accumulator, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Mul_StabilityTest2_RemainBoundedAndProduceCorrectResult)
{
    /* Check the stability of multiplication, i.e. that the overflow does not grow without bounds */
    fe25519 input = { 0x7ffffda + 0x100, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe };
    fe25519 expected = { 0x31cd790, 0x1f8f090, 0x1638e07, 0x19b2c96, 0x1ea9baf, 0xe89a80, 0xb4c78f, 0x12f8e2b, 0x2ca5638, 0xbcc46d };

    for (int i = 0; i < 1024; i++) {

        fe25519_mul(input, input, input);

        /* Check that the result is always bounded by 2*p */
        FAIL_UNLESS(input[0] <  0x7ffffda);
        FAIL_UNLESS(input[1] <= 0x3fffffe);
        FAIL_UNLESS(input[2] <= 0x7fffffe);
        FAIL_UNLESS(input[3] <= 0x3fffffe);
        FAIL_UNLESS(input[4] <= 0x7fffffe);
        FAIL_UNLESS(input[5] <= 0x3fffffe);
        FAIL_UNLESS(input[6] <= 0x7fffffe);
        FAIL_UNLESS(input[7] <= 0x3fffffe);
        FAIL_UNLESS(input[8] <= 0x7fffffe);
        FAIL_UNLESS(input[9] <= 0x3fffffe);
    }

    FAIL_IF_MEMCMP(expected, input, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Invert_Canonical_CorrectlyInvert)
{
    fe25519 input = { 0xdead, 0xbeef, 0xcafe, 0xdeed, 0xface, 0xdead, 0xbeef, 0xcafe, 0xdeed, 0xface };
    fe25519 output;
    fe25519 product;
    fe25519 expected_product = { 1 };

    fe25519_invert(output, input);
    fe25519_mul(product, input, output);
    fe25519_strong_reduce(product, product);

    FAIL_IF_MEMCMP(expected_product, product, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, Invert_NonCanonical_CorrectlyInvert)
{
    fe25519 input = {
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
    fe25519 output;
    fe25519 product;
    fe25519 expected_product = { 1 };

    fe25519_invert(output, input);
    fe25519_mul(product, input, output);
    fe25519_strong_reduce(product, product);

    FAIL_IF_MEMCMP(expected_product, product, sizeof(fe25519));
}

FE3C_TEST(FIELD_ELEMENTS_ED25519_32, I_SquareI_GetMinusOne)
{
    fe25519 output;
    fe25519 canonical = { 0x3ffffed - 1, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff };

    fe25519_square(output, fe25519_i);
    fe25519_strong_reduce(output, output);
    FAIL_IF_MEMCMP(canonical, output, sizeof(fe25519));
}
