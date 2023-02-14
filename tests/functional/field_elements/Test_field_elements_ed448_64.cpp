#include <CppUTest/TestHarness.h>
#include <field_elements/field_elements_ed448.h>

TEST_GROUP(FIELD_ELEMENTS_ED448_64) {

};

TEST(FIELD_ELEMENTS_ED448_64, CmovFlagCleared_NoOp) {

    fe448 input = { 1, 2, 3, 4, 5, 6, 7, 8 };
    fe448 output = { 8, 7, 6, 5, 4, 3, 2, 1 };
    /* Expect output to remain untouched */
    fe448 expected = { 8, 7, 6, 5, 4, 3, 2, 1 };
    fe448_conditional_move(output, input, 0);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, CmovFlagSet_DoMove) {

    fe448 input = { 1, 2, 3, 4, 5, 6, 7, 8 };
    fe448 output = { 8, 7, 6, 5, 4, 3, 2, 1 };
    /* Expect a move to take place */
    fe448 expected = { 1, 2, 3, 4, 5, 6, 7, 8 };
    fe448_conditional_move(output, input, 1);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, StrongReduce_ElementInOkRange_NoOp) {

    fe448 input = { 1, 2, 3, 4, 5, 6, 7, 8 };
    fe448 expected = { 1, 2, 3, 4, 5, 6, 7, 8 };
    fe448 output;
    fe448_strong_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, WeakReduce_ElementInOkRange_NoOp) {

    fe448 input = { 1, 2, 3, 4, 5, 6, 7, 8 };
    fe448 expected = { 1, 2, 3, 4, 5, 6, 7, 8 };
    fe448 output;
    fe448_weak_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, StrongReduce_ElementInOkRangeWithOverflowInLimb_Normalize) {

    fe448 input = { 1, 2, 3, 4, 5, 6, (1ULL << 56), 8 };
    fe448 expected = { 1, 2, 3, 4, 5, 6, 0, 9 };
    fe448 output;
    fe448_strong_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, WeakReduce_ElementInOkRangeWithOverflowInLimb_Normalize) {

    fe448 input = { 1, 2, 3, 4, 5, 6, (1ULL << 56), 8 };
    fe448 expected = { 1, 2, 3, 4, 5, 6, 0, 9 };
    fe448 output;
    fe448_weak_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, StrongReduce_ElementEqualToModulus_NormalizeToZero) {

    fe448 input = { 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 expected = {};
    fe448 output;
    fe448_strong_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, WeakReduce_ElementEqualToModulus_NoOp) {

    fe448 input = { 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 expected = { 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 output;
    fe448_weak_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, StrongReduce_ElementEqualToTwiceTheModulus_NormalizeToZero) {

    fe448 input = { 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffcULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL };
    fe448 expected = {};
    fe448 output;
    fe448_strong_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, WeakReduce_ElementEqualToTwiceTheModulus_ReduceToModulus) {

    fe448 input = { 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffcULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL };
    fe448 expected = { 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 output;
    fe448_weak_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, StrongReduce_ElementEqualToFourTimesTheModulus_NormalizeToZero) {

    fe448 input = { 0x3fffffffffffffcULL, 0x3fffffffffffffcULL, 0x3fffffffffffffcULL, 0x3fffffffffffffcULL, 0x3fffffffffffff8ULL, 0x3fffffffffffffcULL, 0x3fffffffffffffcULL, 0x3fffffffffffffcULL };
    fe448 expected = {};
    fe448 output;
    fe448_strong_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, WeakReduce_ElementEqualToFourTimesTheModulus_ReduceToModulus) {

    fe448 input = { 0x3fffffffffffffcULL, 0x3fffffffffffffcULL, 0x3fffffffffffffcULL, 0x3fffffffffffffcULL, 0x3fffffffffffff8ULL, 0x3fffffffffffffcULL, 0x3fffffffffffffcULL, 0x3fffffffffffffcULL };
    fe448 expected = { 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 output;
    fe448_weak_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, StrongReduce_ElementEqualToOneMoreThanTheModulus_ReduceToOne) {

    fe448 input = { 0xffffffffffffffULL + 1, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 expected = { 1 };
    fe448 output;
    fe448_strong_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, WeakReduce_ElementEqualToOneMoreThanTheModulus_HandleOverflowsButDoNotReduce) {

    /* Note that in 56-bit reduced-radix representation the least-significant limb is fully occupied by ones when representing the field modulus.
     * The correct behaviour of fe448_weak_reduce() in this case is to distribute the overflows, i.e. normalize the reduced-radix representation,
     * but do not reduce the number modulo the field modulus. */
    fe448 input = { 0xffffffffffffffULL + 1, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 expected = { 0x00000000000000ULL, 0x00000000000000ULL, 0x00000000000000ULL, 0x00000000000000ULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 output;
    fe448_weak_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, StrongReduce_ElementEqualToOneLessThanTheModulus_NoOp) {

    fe448 input = { 0xffffffffffffffULL - 1, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 expected = { 0xffffffffffffffULL - 1, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 output;
    fe448_strong_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, WeakReduce_ElementEqualToOneLessThanTheModulus_NoOp) {

    fe448 input = { 0xffffffffffffffULL - 1, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 expected = { 0xffffffffffffffULL - 1, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 output;
    fe448_weak_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, StrongReduce_ElementWithLargeOverflowInHighLimb_NormalizeSuccessfully) {

    fe448 input = { 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL + 0xffffff };
    fe448 expected = { 0, 0, 0, 0, 0, 0, 0, 0xffffff };
    fe448 output;
    fe448_strong_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, StrongReduce_ElementWithOverflowInEachLimb_NormalizeSuccessfully) {

    fe448 input = {
        0xffffffffffffffULL + 0xffffffffff,
        0xffffffffffffffULL + 0xffffffffff,
        0xffffffffffffffULL + 0xffffffffff,
        0xffffffffffffffULL + 0xffffffffff,
        0xfffffffffffffeULL + 0xffffffffff,
        0xffffffffffffffULL + 0xffffffffff,
        0xffffffffffffffULL + 0xffffffffff,
        0xffffffffffffffULL + 0xffffffffff
    };
    fe448 expected = { 0xffffffffff, 0xffffffffff, 0xffffffffff, 0xffffffffff, 0xffffffffff, 0xffffffffff, 0xffffffffff, 0xffffffffff };
    fe448 output;
    fe448_strong_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, StrongReduce_ElementWithMaxOverflowInEachLimb_NormalizeSuccessfully) {

    fe448 input = {
        0xffffffffffffffffULL,
        0xffffffffffffffffULL,
        0xffffffffffffffffULL,
        0xffffffffffffffffULL,
        0xffffffffffffffffULL,
        0xffffffffffffffffULL,
        0xffffffffffffffffULL,
        0xffffffffffffffffULL
    };
    fe448 expected = { 0xff, 0xff, 0xff, 0xff, 0x1ff, 0xff, 0xff, 0xff };
    fe448 output;
    fe448_strong_reduce(output, input);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, Equal_ElementsEqual_ReturnTrue) {

    fe448 input1 = { 1, 2, 3, 4, 5, 6, 7, 8 };
    fe448 input2 = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int equal = fe448_equal(input1, input2);
    CHECK_EQUAL(1, equal);
}

TEST(FIELD_ELEMENTS_ED448_64, Equal_ElementsDifferent_ReturnFalse) {

    fe448 input1 = { 1, 2, 3, 4, 5, 6, 7, 9 };
    fe448 input2 = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int equal = fe448_equal(input1, input2);
    CHECK_EQUAL(0, equal);
}

TEST(FIELD_ELEMENTS_ED448_64, Equal_ElementsCongruentButNotEqual_ReturnFalse) {

    fe448 input1 = {
        0xffffffffffffffULL + 1,
        0xffffffffffffffULL + 2,
        0xffffffffffffffULL + 3,
        0xffffffffffffffULL + 4,
        0xfffffffffffffeULL + 5,
        0xffffffffffffffULL + 6,
        0xffffffffffffffULL + 7,
        0xffffffffffffffULL + 8
    };
    fe448 input2 = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int equal = fe448_equal(input1, input2);
    CHECK_EQUAL(0, equal);
    /* Do a reduction of input1 and check again */
    fe448_strong_reduce(input1, input1);
    equal = fe448_equal(input1, input2);
    MEMCMP_EQUAL(input1, input2, sizeof(fe448));
    CHECK_EQUAL(1, equal);
}

TEST(FIELD_ELEMENTS_ED448_64, Add_OnePlusOne_EqualsTwo) {

    fe448 input1 = { 1, 0, 0, 0, 0, 0, 0, 0 };
    fe448 input2 = { 1, 0, 0, 0, 0, 0, 0, 0 };
    fe448 expected = { 2, 0, 0, 0, 0, 0, 0, 0 };
    fe448 output;
    fe448_add(output, input1, input2);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, Add_ModulusPlusModulus_EqualsTwoModuli) {

    fe448 input1 = { 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 input2 = { 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 expected = { 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffcULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL };
    fe448 output;
    fe448_add(output, input1, input2);

    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, Sub_ModulusMinusModulus_CongruentToZero) {

    fe448 input1 = { 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 input2 = { 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 canonical = {};
    fe448 output;
    fe448_sub(output, input1, input2);
    /* Expect subtraction to result in a non-canonical form, e.g. for a - b we may expect to get a + 2p - b */
    /* Do a reduction and check the canonical form */
    fe448_strong_reduce(output, output);
    MEMCMP_EQUAL(canonical, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, Sub_OneMinusTwo_CongruentToModulusMinusOne) {

    fe448 input1 = { 1, 0, 0, 0, 0, 0, 0, 0 };
    fe448 input2 = { 2, 0, 0, 0, 0, 0, 0, 0 };
    fe448 canonical = { 0xffffffffffffffULL - 1, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 output;
    fe448_sub(output, input1, input2);
    /* Expect subtraction to result in a non-canonical form, e.g. for a - b we may expect to get a + 2p - b */
    /* Do a reduction and check the canonical form */
    fe448_strong_reduce(output, output);
    MEMCMP_EQUAL(canonical, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, Mul_ModulusPlusOneTimesTwo_CongruentToTwo) {

    fe448 input1 = { 0xffffffffffffffULL + 1, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 input2 = { 2, 0, 0, 0, 0, 0, 0, 0 };
    fe448 canonical = { 2, 0, 0, 0, 0, 0, 0, 0 };
    fe448 output;
    /* Expect multiplication to result in a non-canonical form */
    fe448_mul(output, input1, input2);
    /* Do a reduction and check the canonical form */
    fe448_strong_reduce(output, output);
    MEMCMP_EQUAL(canonical, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, Mul_ModulusTimesModulus_CongruentToZero) {

    fe448 input1 = { 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 input2 = { 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xfffffffffffffeULL, 0xffffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffULL };
    fe448 canonical = {};
    fe448 output;
    /* Expect multiplication to result in a non-canonical form */
    fe448_mul(output, input1, input2);
    /* Do a reduction and check the canonical form */
    fe448_strong_reduce(output, output);
    MEMCMP_EQUAL(canonical, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, Mul_TwiceTheModulusTimesFourTimesTheModulusPlusFive_CongruentToZero) {

    fe448 input1 = { 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffcULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL, 0x1fffffffffffffeULL };
    fe448 input2 = { 0x3fffffffffffffcULL + 5, 0x3fffffffffffffcULL, 0x3fffffffffffffcULL, 0x3fffffffffffffcULL, 0x3fffffffffffff8ULL, 0x3fffffffffffffcULL, 0x3fffffffffffffcULL, 0x3fffffffffffffcULL };
    fe448 canonical = {};
    fe448 output;
    /* Expect multiplication to result in a non-canonical form */
    fe448_mul(output, input1, input2);
    /* Do a reduction and check the canonical form */
    fe448_strong_reduce(output, output);
    MEMCMP_EQUAL(canonical, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, Mul_HundredTwentyEightModuliSquared_CongruentToZero) {

    fe448 input = {
        0x3fffffffffffffc0ULL,
        0x3fffffffffffffc0ULL,
        0x3fffffffffffffc0ULL,
        0x3fffffffffffffc0ULL,
        0x3fffffffffffff80ULL,
        0x3fffffffffffffc0ULL,
        0x3fffffffffffffc0ULL,
        0x3fffffffffffffc0ULL
    };
    fe448 canonical = {};
    fe448 output;
    fe448_mul(output, input, input);
    fe448_strong_reduce(output, output);
    MEMCMP_EQUAL(canonical, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, Mul_ZeroTimesAnything_EqualToZero) {

    fe448 input1 = {};
    fe448 input2 = { 14, 0, 0, 0, 123, 2, 3, 4 };
    fe448 expected = {};
    fe448 output;
    fe448_mul(output, input1, input2);
    MEMCMP_EQUAL(expected, output, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, Mul_StabilityTest_RemainBoundedAndProduceCorrectResult) {

    /* Check the stability of multiplication, i.e. that the overflow does not grow without bounds */
    fe448 input = {
        0x7fffffffffffff80ULL + 0xfff,
        0x7fffffffffffff80ULL,
        0x7fffffffffffff80ULL,
        0x7fffffffffffff80ULL,
        0x7fffffffffffff00ULL,
        0x7fffffffffffff80ULL,
        0x7fffffffffffff80ULL,
        0x7fffffffffffff80ULL
    };
    fe448 accumulator = { 1 };

    fe448 expected = { 0xd553276039cb9f, 0x9f8bb933d1f9f9, 0x673958f9db2f26, 0x81f9f69b29772a, 0xf7d15bb28b4ec6, 0x45884ee38d4625, 0x149373d1d67792, 0x68b38453d70f3b };

    for (int i = 0; i < 1024; i++) {

        fe448_mul(accumulator, accumulator, input);

        /* Check that the result is always bounded by 2*p */
        CHECK_TRUE(accumulator[0] <  0x1fffffffffffffeULL);
        CHECK_TRUE(accumulator[1] <= 0x1fffffffffffffeULL);
        CHECK_TRUE(accumulator[2] <= 0x1fffffffffffffeULL);
        CHECK_TRUE(accumulator[3] <= 0x1fffffffffffffeULL);
        CHECK_TRUE(accumulator[4] <= 0x1fffffffffffffcULL);
        CHECK_TRUE(accumulator[5] <= 0x1fffffffffffffeULL);
        CHECK_TRUE(accumulator[6] <= 0x1fffffffffffffeULL);
        CHECK_TRUE(accumulator[7] <= 0x1fffffffffffffeULL);
    }

    MEMCMP_EQUAL(expected, accumulator, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, Mul_StabilityTest2_RemainBoundedAndProduceCorrectResult) {

    /* Check the stability of multiplication, i.e. that the overflow does not grow without bounds */
    fe448 input = {
        0xffffffffffffffULL + 0xfff,
        0xffffffffffffffULL,
        0xffffffffffffffULL,
        0xffffffffffffffULL,
        0xfffffffffffffeULL,
        0xffffffffffffffULL,
        0xffffffffffffffULL,
        0xffffffffffffffULL
    };
    fe448 expected = { 0x6ba5e729a3d0a0, 0x37ab76860956d2, 0xfb8c6b9d4d74d1, 0xc0c31d2f298eb3, 0xde2de3c39eb81b, 0xae3532f80656dc, 0xc2ac26399fea6, 0xbd3c1cd932467c };

    for (int i = 0; i < 1024; i++) {

        fe448_mul(input, input, input);

        /* Check that the result is always bounded by 2*p */
        CHECK_TRUE(input[0] <  0x1fffffffffffffeULL);
        CHECK_TRUE(input[1] <= 0x1fffffffffffffeULL);
        CHECK_TRUE(input[2] <= 0x1fffffffffffffeULL);
        CHECK_TRUE(input[3] <= 0x1fffffffffffffeULL);
        CHECK_TRUE(input[4] <= 0x1fffffffffffffcULL);
        CHECK_TRUE(input[5] <= 0x1fffffffffffffeULL);
        CHECK_TRUE(input[6] <= 0x1fffffffffffffeULL);
        CHECK_TRUE(input[7] <= 0x1fffffffffffffeULL);
    }

    MEMCMP_EQUAL(expected, input, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, Invert_Canonical_CorrectlyInvert) {

    fe448 input = { 0xdead, 0xbeef, 0xcafe, 0xdeed, 0xface, 0xdead, 0xbeef, 0xcafe448 };
    fe448 output;
    fe448 product;
    fe448 expected_product = { 1 };

    fe448_invert(output, input);
    fe448_mul(product, input, output);
    fe448_strong_reduce(product, product);

    MEMCMP_EQUAL(expected_product, product, sizeof(fe448));
}

TEST(FIELD_ELEMENTS_ED448_64, Invert_NonCanonical_CorrectlyInvert) {

    fe448 input = {
        0xffffffffffffffULL - 1,
        0xffffffffffffffULL + 2,
        0xffffffffffffffULL - 3,
        0xffffffffffffffULL + 4,
        0xfffffffffffffeULL - 5,
        0xffffffffffffffULL + 6,
        0xffffffffffffffULL - 7,
        0xffffffffffffffULL + 8
    };
    fe448 output;
    fe448 product;
    fe448 expected_product = { 1 };

    fe448_invert(output, input);
    fe448_mul(product, input, output);
    fe448_strong_reduce(product, product);

    MEMCMP_EQUAL(expected_product, product, sizeof(fe448));
}
