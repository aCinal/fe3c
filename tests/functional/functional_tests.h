

#ifndef __FUNCTIONAL_TESTS_H
#define __FUNCTIONAL_TESTS_H

/* ------------------------------------------------------------------------------------------------------- */

#if FE3C_FUNCTIONAL_TESTS_HOST

/* Include CppUtest framework header */
#include <CppUTest/TestHarness.h>
#define FE3C_TEST(group, name)                 TEST(group, name)
/* Provide wrappers around CppUTest macros */
#define FAIL_UNLESS(condition)                 CHECK_TRUE(condition)
#define FAIL_UNLESS_EQUAL(expected, actual)    CHECK_EQUAL(expected, actual)
#define FAIL_IF_MEMCMP(expected, actual, len)  MEMCMP_EQUAL(expected, actual, len)

#endif /* FE3C_FUNCTIONAL_TESTS_HOST */

/* ------------------------------------------------------------------------------------------------------- */

#if FE3C_FUNCTIONAL_TESTS_ESP32

/* Include the Unity framework header */
#include <unity.h>
#define FE3C_TEST(group, name)                 TEST_CASE(#name, #group)
/* Provide wrappers around Unity macros */
#define FAIL_UNLESS(condition)                 TEST_ASSERT(condition)
#define FAIL_UNLESS_EQUAL(expected, actual)    TEST_ASSERT_EQUAL(expected, actual)
#define FAIL_IF_MEMCMP(expected, actual, len)  TEST_ASSERT_EQUAL_MEMORY(expected, actual, len)

#endif /* FE3C_FUNCTIONAL_TESTS_ESP32 */

/* ------------------------------------------------------------------------------------------------------- */

#endif /* __FUNCTIONAL_TESTS_H */
