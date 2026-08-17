/* BEGIN Header */
/**
 ******************************************************************************
 * \file            test_vector3f.c
 * \author          Andrea Vivani
 * \brief           Unit tests for vector3f.c
 ******************************************************************************
 * \copyright
 *
 * Copyright 2026 Andrea Vivani
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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "vector3f.h"

#include <cmocka.h>

/* Functions -----------------------------------------------------------------*/

static void test_vector3fAdd(void** state) {
    (void)state; /* unused */
    axis3f_t a = {1.0f, 2.0f, 3.0f};
    axis3f_t b = {4.0f, 5.0f, 6.0f};
    axis3f_t out;
    vector3fAdd(&a, &b, &out);
    assert_float_equal(out.x, 5.0f, 1e-5);
    assert_float_equal(out.y, 7.0f, 1e-5);
    assert_float_equal(out.z, 9.0f, 1e-5);
}

static void test_vector3fSub(void** state) {
    (void)state; /* unused */
    axis3f_t a = {1.0f, 2.0f, 3.0f};
    axis3f_t b = {4.0f, 5.0f, 6.0f};
    axis3f_t out;
    vector3fSub(&a, &b, &out);
    assert_float_equal(out.x, -3.0f, 1e-5);
    assert_float_equal(out.y, -3.0f, 1e-5);
    assert_float_equal(out.z, -3.0f, 1e-5);
}

static void test_vector3fScale(void** state) {
    (void)state; /* unused */
    axis3f_t a = {1.0f, 2.0f, 3.0f};
    axis3f_t out;
    vector3fScale(&a, 2.0f, &out);
    assert_float_equal(out.x, 2.0f, 1e-5);
    assert_float_equal(out.y, 4.0f, 1e-5);
    assert_float_equal(out.z, 6.0f, 1e-5);
}

static void test_vector3fDot(void** state) {
    (void)state; /* unused */
    axis3f_t a = {1.0f, 2.0f, 3.0f};
    axis3f_t b = {4.0f, 5.0f, 6.0f};
    assert_float_equal(vector3fDot(&a, &b), 32.0f, 1e-5);
}

static void test_vector3fNorm(void** state) {
    (void)state; /* unused */
    axis3f_t a = {1.0f, 2.0f, 3.0f};
    assert_float_equal(vector3fNorm(&a), 3.7416574f, 1e-5);
}

static void test_vector3fNormalize(void** state) {
    (void)state; /* unused */
    axis3f_t a = {1.0f, 2.0f, 3.0f};
    axis3f_t out;
    vector3fNormalize(&a, &out);
    assert_float_equal(out.x, 0.2672612f, 1e-5);
    assert_float_equal(out.y, 0.5345225f, 1e-5);
    assert_float_equal(out.z, 0.8017837f, 1e-5);
    /* zero-norm input -> zero vector */
    axis3f_t zero = {0.0f, 0.0f, 0.0f};
    vector3fNormalize(&zero, &out);
    assert_float_equal(out.x, 0.0f, 1e-5);
    assert_float_equal(out.y, 0.0f, 1e-5);
    assert_float_equal(out.z, 0.0f, 1e-5);
}

static void test_vector3fCross(void** state) {
    (void)state; /* unused */
    axis3f_t a = {1.0f, 2.0f, 3.0f};
    axis3f_t b = {4.0f, 5.0f, 6.0f};
    axis3f_t out;
    vector3fCross(&a, &b, &out);
    assert_float_equal(out.x, -3.0f, 1e-5);
    assert_float_equal(out.y, 6.0f, 1e-5);
    assert_float_equal(out.z, -3.0f, 1e-5);
    /* in-place (out aliases a) */
    axis3f_t c = {1.0f, 2.0f, 3.0f};
    vector3fCross(&c, &b, &c);
    assert_float_equal(c.x, -3.0f, 1e-5);
    assert_float_equal(c.y, 6.0f, 1e-5);
    assert_float_equal(c.z, -3.0f, 1e-5);
}

int main(void) {
    const struct CMUnitTest test_vector3f[] = {
        cmocka_unit_test(test_vector3fAdd),  cmocka_unit_test(test_vector3fSub),       cmocka_unit_test(test_vector3fScale), cmocka_unit_test(test_vector3fDot),
        cmocka_unit_test(test_vector3fNorm), cmocka_unit_test(test_vector3fNormalize), cmocka_unit_test(test_vector3fCross),
    };

    return cmocka_run_group_tests(test_vector3f, NULL, NULL);
}
