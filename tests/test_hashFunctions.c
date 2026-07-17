/* BEGIN Header */
/**
 ******************************************************************************
 * \file            test_hashFunctions.c
 * \author          Andrea Vivani
 * \brief           Unit tests for hashFunctions.c
 ******************************************************************************
 * \copyright
 *
 * Copyright 2024 Andrea Vivani
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 ******************************************************************************
 */
/* END Header */

/* Includes ------------------------------------------------------------------*/

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include "hashFunctions.h"

#include <cmocka.h>

/* Support functions ---------------------------------------------------------*/

static uint8_t skipAssert = 0;

void ADVUtils_testAssert(const int result, const char* const expression, const char* const file, const int line) {
    if (skipAssert) {
        return;
    } else {
        mock_assert(result, expression, file, line);
    }
}

/* Functions -----------------------------------------------------------------*/

static void test_hash_FNV1A_basic(void** state) {
    (void)state; // unused
    assert_int_equal(hash_FNV1A("test"), 427567909);
}

static void test_hash_FNV1A_empty_string(void** state) {
    (void)state; // unused
    assert_int_equal(hash_FNV1A(""), 2216829733);
}

static void test_hash_FNV1A_null_pointer(void** state) {
    (void)state; // unused
    expect_assert_failure(hash_FNV1A(NULL));
}

static void test_hash_FNV1A_null_pointer_assert_disabled(void** state) {
    (void)state; // unused
    skipAssert = 1;
    assert_int_equal(hash_FNV1A(NULL), 2216829733U);
    skipAssert = 0;
}

static void test_hash_FNV1A_special_characters(void** state) {
    (void)state; // unused
    assert_int_equal(hash_FNV1A("!@#$%^&*()"), 114978727);
}

static void test_hash_FNV1A_different_cases(void** state) {
    (void)state; // unused
    assert_int_not_equal(hash_FNV1A("test"), hash_FNV1A("TEST"));
    assert_int_not_equal(hash_FNV1A("Test"), hash_FNV1A("tEsT"));
}

static void test_hash_FNV1A_long_string(void** state) {
    (void)state; // unused
    char* long_string = "This is a very long string to test the hash function for long inputs.";
    assert_int_equal(hash_FNV1A(long_string), 70682599);
}

static void test_hash_FNV1A_numeric_string(void** state) {
    (void)state; // unused
    assert_int_equal(hash_FNV1A("1234567890"), 3402609060);
}

// Test cases for hash_djb
static void test_hash_djb_basic(void** state) {
    (void)state; // unused
    assert_int_equal(hash_djb("test"), 2090756197);
}

static void test_hash_djb_empty_string(void** state) {
    (void)state; // unused
    assert_int_equal(hash_djb(""), 5381);
}

static void test_hash_djb_null_pointer(void** state) {
    (void)state; // unused
    skipAssert = 0;
    expect_assert_failure(hash_djb(NULL));
}

static void test_hash_djb_null_pointer_assert_disabled(void** state) {
    (void)state; // unused
    skipAssert = 1;
    assert_int_equal(hash_djb(NULL), 5381U);
    skipAssert = 0;
}

static void test_hash_djb_special_characters(void** state) {
    (void)state; // unused
    assert_int_equal(hash_djb("!@#$%^&*()"), 1465968561);
}

static void test_hash_djb_different_cases(void** state) {
    (void)state; // unused
    assert_int_not_equal(hash_djb("test"), hash_djb("TEST"));
    assert_int_not_equal(hash_djb("Test"), hash_djb("tEsT"));
}

static void test_hash_djb_long_string(void** state) {
    (void)state; // unused
    char* long_string = "This is a very long string to test the hash function for long inputs.";
    assert_int_equal(hash_djb(long_string), 2041601053);
}

static void test_hash_djb_numeric_string(void** state) {
    (void)state; // unused
    assert_int_equal(hash_djb("1234567890"), 4018482162);
}

// Test cases for hash_sdbm
static void test_hash_sdbm_basic(void** state) {
    (void)state; // unused
    assert_int_equal(hash_sdbm("test"), 1195757874);
}

static void test_hash_sdbm_empty_string(void** state) {
    (void)state; // unused
    assert_int_equal(hash_sdbm(""), 0);
}

static void test_hash_sdbm_null_pointer(void** state) {
    (void)state; // unused
    skipAssert = 0;
    expect_assert_failure(hash_sdbm(NULL));
}

static void test_hash_sdbm_null_pointer_assert_disabled(void** state) {
    (void)state; // unused
    skipAssert = 1;
    assert_int_equal(hash_sdbm(NULL), 0U);
    skipAssert = 0;
}

static void test_hash_sdbm_special_characters(void** state) {
    (void)state; // unused
    assert_int_equal(hash_sdbm("!@#$%^&*()"), 773972894);
}

static void test_hash_sdbm_different_cases(void** state) {
    (void)state; // unused
    assert_int_not_equal(hash_sdbm("test"), hash_sdbm("TEST"));
    assert_int_not_equal(hash_sdbm("Test"), hash_sdbm("tEsT"));
}

static void test_hash_sdbm_long_string(void** state) {
    (void)state; // unused
    char* long_string = "This is a very long string to test the hash function for long inputs.";
    assert_int_equal(hash_sdbm(long_string), 3092146248);
}

static void test_hash_sdbm_numeric_string(void** state) {
    (void)state; // unused
    assert_int_equal(hash_sdbm("1234567890"), 800103739);
}

int main(void) {
    const struct CMUnitTest test_hashFunctions[] = {
        // hash_FNV1A tests
        cmocka_unit_test(test_hash_FNV1A_basic),
        cmocka_unit_test(test_hash_FNV1A_empty_string),
        cmocka_unit_test(test_hash_FNV1A_null_pointer),
        cmocka_unit_test(test_hash_FNV1A_null_pointer_assert_disabled),
        cmocka_unit_test(test_hash_FNV1A_special_characters),
        cmocka_unit_test(test_hash_FNV1A_different_cases),
        cmocka_unit_test(test_hash_FNV1A_long_string),
        cmocka_unit_test(test_hash_FNV1A_numeric_string),

        // hash_djb tests
        cmocka_unit_test(test_hash_djb_basic),
        cmocka_unit_test(test_hash_djb_empty_string),
        cmocka_unit_test(test_hash_djb_null_pointer),
        cmocka_unit_test(test_hash_djb_null_pointer_assert_disabled),
        cmocka_unit_test(test_hash_djb_special_characters),
        cmocka_unit_test(test_hash_djb_different_cases),
        cmocka_unit_test(test_hash_djb_long_string),
        cmocka_unit_test(test_hash_djb_numeric_string),

        // hash_sdbm tests
        cmocka_unit_test(test_hash_sdbm_basic),
        cmocka_unit_test(test_hash_sdbm_empty_string),
        cmocka_unit_test(test_hash_sdbm_null_pointer),
        cmocka_unit_test(test_hash_sdbm_null_pointer_assert_disabled),
        cmocka_unit_test(test_hash_sdbm_special_characters),
        cmocka_unit_test(test_hash_sdbm_different_cases),
        cmocka_unit_test(test_hash_sdbm_long_string),
        cmocka_unit_test(test_hash_sdbm_numeric_string),
    };

    return cmocka_run_group_tests(test_hashFunctions, NULL, NULL);
}
