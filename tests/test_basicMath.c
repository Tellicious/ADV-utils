/* BEGIN Header */
/**
 ******************************************************************************
 * \file            test_basicMath.c
 * \author          Andrea Vivani
 * \brief           Unit tests for basicMath.c
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

#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "basicMath.h"

#include <cmocka.h>

/* Functions -----------------------------------------------------------------*/

static void test_macros(void** state) {
    (void)state; /* unused */

    // Test ABS macro
    assert_int_equal(ABS(-5), 5);
    assert_int_equal(ABS(5), 5);
    assert_int_equal(ABS(0), 0);
    assert_float_equal(ABS(-3.5f), 3.5f, 1e-5);

    // Test SIGN macro
    assert_int_equal(SIGN(-5), -1);
    assert_int_equal(SIGN(5), 1);
    assert_int_equal(SIGN(0), 1);

    // Test CONSTRAIN macro
    assert_int_equal(CONSTRAIN(5, 0, 10), 5);
    assert_int_equal(CONSTRAIN(-5, 0, 10), 0);
    assert_int_equal(CONSTRAIN(15, 0, 10), 10);
    assert_int_equal(CONSTRAIN(0, 0, 0), 0);

    // Test MAP macro
    assert_int_equal(MAP(5, 0, 10, 100, 200), 150);
    assert_int_equal(MAP(0, 0, 10, 100, 200), 100);
    assert_int_equal(MAP(10, 0, 10, 100, 200), 200);

    // Test DEADBAND macro
    assert_int_equal(DEADBAND(5, 2), 3);
    assert_int_equal(DEADBAND(-5, 2), -3);
    assert_int_equal(DEADBAND(1, 2), 0);
    assert_int_equal(DEADBAND(2, 2), 0);

    // Test MAX and MIN macros
    assert_int_equal(MAX(5, 10), 10);
    assert_int_equal(MAX(5, 5), 5);
    assert_int_equal(MIN(5, 10), 5);
    assert_int_equal(MIN(10, 10), 10);

    // Test conversion macros
    assert_float_equal(RAD2DEG(3.141592654f), 180.0f, 1e-5);
    assert_float_equal(DEG2RAD(180.0f), 3.141592654f, 1e-5);
    assert_float_equal(C2K(0.0f), 273.15f, 1e-5);
    assert_float_equal(K2C(273.15f), 0.0f, 1e-5);
    assert_float_equal(MG2MS2(1000.0f), 9.80665f, 1e-5);
    assert_float_equal(MS22MG(9.80665f), 1000.0f, 1e-5);
    assert_float_equal(RADPS2MDPS(10.f), 572957.795132f, 1e-5);
    assert_float_equal(MDPS2RADPS(572957.795132f), 10.f, 1e-5);

    // Test bit manipulation macros
    assert_true(IS_BIT_SET_ALL(0xFF, 0x0F));
    assert_false(IS_BIT_SET_ALL(0x0F, 0xF0));
    assert_true(IS_BIT_SET_ANY(0xFF, 0x02));
    assert_false(IS_BIT_SET_ANY(0x00, 0x01));
    assert_int_equal(BIT_MASK(0xFF, 0x0F), 0x0F);
    assert_int_equal(BIT_SET(0x00, 0x0F), 0x0F);
    assert_int_equal(BIT_CLEAR(0xFF, 0x0F), 0xF0);
    assert_int_equal(BIT_TOGGLE(0x01, 0x0F), 0x0E);
    assert_int_equal(BIT_TOGGLE(0x0F, 0x0F), 0x00);
    assert_int_equal(SHIFT(0x03, 2), 0x0C);
    assert_int_equal(UNSHIFT(0x0C, 2), 0x03);

    // Test bit manipulation macros in place
    uint8_t val = 0x1C;
    BIT_SET_IN_PLACE(val, 0x30);
    assert_int_equal(val, 0x3C);
    BIT_CLEAR_IN_PLACE(val, 0x0C);
    assert_int_equal(val, 0x30);
    BIT_TOGGLE_IN_PLACE(val, 0x62);
    assert_int_equal(val, 0x52);
    UNSHIFT_IN_PLACE(val, 1);
    assert_int_equal(val, 0x29);
    SHIFT_IN_PLACE(val, 2);
    assert_int_equal(val, 0xA4);
}

static void test_functions(void** state) {
    (void)state; /* unused */

    // Test fastSqrt function
    assert_float_equal(fastSqrt(4.0f), 2.0f, 1e-4);
    assert_float_equal(fastSqrt(121.0f), 11.0f, 1e-4);
    assert_float_equal(fastSqrt(0.0f), 0.0f, 1e-4);
    assert_true(isnan(fastSqrt(-4.0f))); // Check for NaN for negative input

    // Test fastInvSqrt function
    assert_float_equal(fastInvSqrt(4.0f), 0.5f, 2e-3);
    assert_float_equal(fastInvSqrt(9.0f), 1.0f / 3.0f, 2e-3);
    assert_float_equal(fastInvSqrt(1.0f), 1.0f, 2e-3);
    assert_true(isnan(fastInvSqrt(-4.0f))); // Check for NaN for negative input

    // Test fastSin function
    assert_float_equal(fastSin(3.141592654f / 2.0f), 1.0f, 1e-4);
    assert_float_equal(fastSin(-3.141592654f / 2.0f), -1.0f, 1e-4);
    assert_float_equal(fastSin(0.0f), 0.0f, 1e-4);
    assert_float_equal(fastSin(3.141592654f), 0.0f, 1e-4);

    // Test fastCos function
    assert_float_equal(fastCos(0.0f), 1.0f, 1e-4);
    assert_float_equal(fastCos(3.141592654f), -1.0f, 1e-4);
    assert_float_equal(fastCos(3.141592654f / 2.0f), 0.0f, 1e-4);
}

int main(void) {
    const struct CMUnitTest test_basicMath[] = {
        cmocka_unit_test(test_macros),
        cmocka_unit_test(test_functions),
    };

    return cmocka_run_group_tests(test_basicMath, NULL, NULL);
}
