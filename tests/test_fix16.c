/* BEGIN Header */
/**
 ******************************************************************************
 * \file            test_fix16.c
 * \author          Andrea Vivani
 * \brief           Unit tests for fix16.c
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
#include <stddef.h>
#include <stdint.h>
#include "fix16.h"

#include <cmocka.h>

/* Functions -----------------------------------------------------------------*/

static void test_fix16_add(void** state) {
    (void)state; /* unused */
    assert_int_equal(fix16_add(fix16_fromInt(3), fix16_fromInt(4)), fix16_fromInt(7));
    assert_int_equal(fix16_add(FIX16_MAX, fix16_fromInt(1)), FIX16_MAX);  /* overflow high */
    assert_int_equal(fix16_add(FIX16_MIN, fix16_fromInt(-1)), FIX16_MIN); /* overflow low */
}

static void test_fix16_sub(void** state) {
    assert_int_equal(fix16_sub(fix16_fromInt(4), fix16_fromInt(9)), fix16_fromInt(-5));
    assert_int_equal(fix16_sub(FIX16_MAX, fix16_fromInt(-1)), FIX16_MAX); /* overflow high */
    assert_int_equal(fix16_sub(FIX16_MIN, fix16_fromInt(1)), FIX16_MIN);  /* overflow low */
}

static void test_fix16_mul(void** state) {
    (void)state;                                                                         /* unused */
    assert_int_equal(fix16_mul(fix16_fromInt(2), fix16_fromInt(3)), fix16_fromInt(6));   /* positive product */
    assert_int_equal(fix16_mul(fix16_fromInt(-2), fix16_fromInt(3)), fix16_fromInt(-6)); /* negative product */
    assert_float_equal(fix16_toFloat(fix16_mul(fix16_fromFloat(0.5f), fix16_fromFloat(0.5f))), 0.25f, 1e-4);
    assert_int_equal(fix16_mul(FIX16_MAX, fix16_fromInt(2)), FIX16_MAX);  /* overflow high */
    assert_int_equal(fix16_mul(FIX16_MAX, fix16_fromInt(-2)), FIX16_MIN); /* overflow low */
}

static void test_fix16_div(void** state) {
    (void)state;                                                                                           /* unused */
    assert_int_equal(fix16_div(fix16_fromInt(6), fix16_fromInt(2)), fix16_fromInt(3));                     /* positive quotient */
    assert_int_equal(fix16_div(fix16_fromInt(-6), fix16_fromInt(2)), fix16_fromInt(-3));                   /* negative quotient */
    assert_float_equal(fix16_toFloat(fix16_div(fix16_fromInt(1), fix16_fromInt(3))), 1.0f / 3.0f, 1e-4);   /* rounding + */
    assert_float_equal(fix16_toFloat(fix16_div(fix16_fromInt(-1), fix16_fromInt(3))), -1.0f / 3.0f, 1e-4); /* rounding - */
    assert_int_equal(fix16_div(fix16_fromInt(5), 0), FIX16_MAX);                                           /* div by zero, a>=0 */
    assert_int_equal(fix16_div(fix16_fromInt(-5), 0), FIX16_MIN);                                          /* div by zero, a<0 */
    assert_int_equal(fix16_div(0, 0), FIX16_MAX);                                                          /* div by zero, a==0 */
    assert_int_equal(fix16_div(FIX16_MAX, fix16_fromFloat(0.5f)), FIX16_MAX);                              /* overflow high */
    assert_int_equal(fix16_div(FIX16_MIN, fix16_fromFloat(0.5f)), FIX16_MIN);                              /* overflow low */
    assert_int_equal(fix16_div(fix16_fromInt(2), fix16_fromInt(3)), fix16_fromFloat(2.0f / 3.0f));         /* +1 branch: num,b same sign  */
    assert_int_equal(fix16_div(fix16_fromInt(-2), fix16_fromInt(3)), fix16_fromFloat(-2.0f / 3.0f));       /* -1 branch: num,b differ     */
    assert_int_equal(fix16_div(fix16_fromInt(2), fix16_fromInt(-3)), fix16_fromFloat(-2.0f / 3.0f));       /* -1 branch: num>0, b<0 */
    assert_int_equal(fix16_div(fix16_fromInt(-2), fix16_fromInt(-3)), fix16_fromFloat(2.0f / 3.0f));       /* +1 branch: both negative */
}

static void test_fix16_sqrt(void** state) {
    (void)state;                                                                        /* unused */
    assert_int_equal(fix16_sqrt(fix16_fromInt(4)), fix16_fromInt(2));                   /* perfect square */
    assert_float_equal(fix16_toFloat(fix16_sqrt(fix16_fromInt(2))), 1.41421356f, 1e-4); /* non-perfect */
    assert_int_equal(fix16_sqrt(0), 0);                                                 /* zero */
    assert_int_equal(fix16_sqrt(fix16_fromInt(-1)), 0);                                 /* negative -> 0 */
}

static void test_fix16_abs(void** state) {
    (void)state;                                                      /* unused */
    assert_int_equal(fix16_abs(fix16_fromInt(3)), fix16_fromInt(3));  /* positive */
    assert_int_equal(fix16_abs(fix16_fromInt(-3)), fix16_fromInt(3)); /* negative */
    assert_int_equal(fix16_abs(FIX16_MIN), FIX16_MAX);                /* MIN -> MAX */
}

static void test_fix16_saturation(void** state) {
    assert_int_equal(fix16_saturated(FIX16_MAX), 1U);
    assert_int_equal(fix16_saturated(FIX16_MIN), 1U);
    assert_int_equal(fix16_saturated(fix16_fromInt(1)), 0U);
}

static void test_fix16_conversions(void** state) {
    (void)state; /* unused */
    /* from_int: positive, negative, zero */
    assert_int_equal(fix16_fromInt(5), 5 * FIX16_ONE);
    assert_int_equal(fix16_fromInt(-5), -5 * FIX16_ONE);
    assert_int_equal(fix16_fromInt(0), 0);
    /* to_int: rounds half away, both signs */
    assert_int_equal(fix16_toInt(fix16_fromFloat(3.49f)), 3);
    assert_int_equal(fix16_toInt(fix16_fromFloat(3.51f)), 4);
    assert_int_equal(fix16_toInt(fix16_fromFloat(-3.49f)), -3);
    assert_int_equal(fix16_toInt(fix16_fromFloat(-3.51f)), -4);
    /* from_float: normal + both saturations */
    assert_int_equal(fix16_fromFloat(2.5f), fix16_fromInt(2) + (FIX16_ONE >> 1));
    assert_int_equal(fix16_fromFloat(1.0e9f), FIX16_MAX);
    assert_int_equal(fix16_fromFloat(-1.0e9f), FIX16_MIN);
    /* to_float */
    assert_float_equal(fix16_toFloat(FIX16_ONE), 1.0f, 1e-6);
    assert_float_equal(fix16_toFloat(fix16_fromFloat(-2.75f)), -2.75f, 1e-4);
}

static void test_fix16_satRoundQ32(void** state) {
    (void)state; /* unused */
    uint8_t sat = 0U;
    /* exact Q32.32 of 1.0 -> Q16.16 1.0, no saturation */
    assert_int_equal(fix16_satRoundQ32((int64_t)FIX16_ONE << FIX16_FRAC_BITS, &sat), FIX16_ONE);
    assert_int_equal(sat, 0U);
    /* half-ULP rounds away from zero, both signs */
    assert_int_equal(fix16_satRoundQ32((int64_t)1 << (FIX16_FRAC_BITS - 1), &sat), 1);
    assert_int_equal(fix16_satRoundQ32(-((int64_t)1 << (FIX16_FRAC_BITS - 1)), &sat), -1);
    /* overflow high */
    sat = 0U;
    assert_int_equal(fix16_satRoundQ32((int64_t)1 << 60, &sat), FIX16_MAX);
    assert_int_equal(sat, 1U);
    /* overflow low */
    sat = 0U;
    assert_int_equal(fix16_satRoundQ32(-((int64_t)1 << 60), &sat), FIX16_MIN);
    assert_int_equal(sat, 1U);
}

int main(void) {
    const struct CMUnitTest test_fix16[] = {
        cmocka_unit_test(test_fix16_add),        cmocka_unit_test(test_fix16_sub),         cmocka_unit_test(test_fix16_mul),
        cmocka_unit_test(test_fix16_div),        cmocka_unit_test(test_fix16_sqrt),        cmocka_unit_test(test_fix16_abs),
        cmocka_unit_test(test_fix16_saturation), cmocka_unit_test(test_fix16_conversions), cmocka_unit_test(test_fix16_satRoundQ32),
    };

    return cmocka_run_group_tests(test_fix16, NULL, NULL);
}
