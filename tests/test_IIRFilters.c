/* BEGIN Header */
/**
 ******************************************************************************
 * \file            testIIRFilters.c
 * \author          Andrea Vivani
 * \brief           Unit tests for IIRFilters.c
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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "IIRFilters.h"

#include <cmocka.h>

/* Macros --------------------------------------------------------------------*/
#define NUM_DATA 111

/* Private variables ---------------------------------------------------------*/
static float input[NUM_DATA] = {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,
                                13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13,
                                -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, 13,  13,  13,  13,  13,  13,  13,  13,  13,
                                13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  -13, -13, -13, -13, -13, -13, -13,
                                -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, 13};

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

static void test_IIRFilterInit(void** state) {
    (void)state; /* unused */
    IIRFilterGeneric_t filter;
    IIRFilterInit(&filter, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0);
    assert_float_equal(filter.n0, 1.0, 1e-5);
    assert_float_equal(filter.n1, 2.0, 1e-5);
    assert_float_equal(filter.n2, 3.0, 1e-5);
    assert_float_equal(filter.n3, 4.0, 1e-5);
    assert_float_equal(filter.d1, 5.0, 1e-5);
    assert_float_equal(filter.d2, 6.0, 1e-5);
    assert_float_equal(filter.d3, 7.0, 1e-5);
    assert_float_equal(filter.i1, 0.0, 1e-5);
    assert_float_equal(filter.i2, 0.0, 1e-5);
    assert_float_equal(filter.i3, 0.0, 1e-5);
    assert_float_equal(filter.o1, 0.0, 1e-5);
    assert_float_equal(filter.o2, 0.0, 1e-5);
    assert_float_equal(filter.o3, 0.0, 1e-5);
}

static void test_IIRFilterInitLP(void** state) {
    (void)state; /* unused */
    IIRFilterGeneric_t filter;
    IIRFilterInitLP(&filter, 100.0, 1.0);
    assert_float_equal(filter.n0, 6.74552e-2f, 1e-7); //b1
    assert_float_equal(filter.n1, 1.34911e-1, 1e-6);  //b2
    assert_float_equal(filter.n2, 6.74552e-2, 1e-7);  //b3
    assert_float_equal(filter.n3, 0.0, 1e-6);         //b4
    assert_float_equal(filter.d1, -1.14298, 1e-6);    //a2
    assert_float_equal(filter.d2, 4.12801e-1, 1e-6);  //a3
    assert_float_equal(filter.d3, 0.0, 1e-6);         //a4
}

static void test_IIRFilterInitHP(void** state) {
    (void)state; /* unused */
    IIRFilterGeneric_t filter;
    IIRFilterInitHP(&filter, 100.0, 1.0);
    assert_float_equal(filter.n0, 6.38946e-1, 1e-6);
    assert_float_equal(filter.n1, -1.277891, 1e-6);
    assert_float_equal(filter.n2, 6.38946e-1, 1e-6);
    assert_float_equal(filter.n3, 0.0, 1e-6);
    assert_float_equal(filter.d1, -1.14298, 1e-6);
    assert_float_equal(filter.d2, 4.12802e-1, 1e-6);
    assert_float_equal(filter.d3, 0.0, 1e-6);
}

static void test_IIRFilterInitBP(void** state) {
    (void)state; /* unused */
    IIRFilterGeneric_t filter;
    IIRFilterInitBP(&filter, 1000.0, 500.0, 0.1);
    assert_float_equal(filter.n0, 1.28120e-1, 1e-6);
    assert_float_equal(filter.n1, 0.0, 1e-6);
    assert_float_equal(filter.n2, -1.28120e-1, 1e-6);
    assert_float_equal(filter.n3, 0.0, 1e-6);
    assert_float_equal(filter.d1, -1.41073, 1e-5);
    assert_float_equal(filter.d2, 7.43761e-1, 1e-6);
    assert_float_equal(filter.d3, 0.0, 1e-6);
}

static void test_IIRFilterInitBS(void** state) {
    (void)state; /* unused */
    IIRFilterGeneric_t filter;
    IIRFilterInitBS(&filter, 1000.0, 500.0, 0.1);
    assert_float_equal(filter.n0, 8.71880e-1, 1e-6);
    assert_float_equal(filter.n1, -1.41073, 1e-5);
    assert_float_equal(filter.n2, 8.71880e-1, 1e-6);
    assert_float_equal(filter.n3, 0.0, 1e-6);
    assert_float_equal(filter.d1, -1.41073, 1e-5);
    assert_float_equal(filter.d2, 7.43761e-1, 1e-6);
    assert_float_equal(filter.d3, 0.0, 1e-6);
}

static void test_IIRFilterProcess(void** state) {
    (void)state; /* unused */
    IIRFilterGeneric_t filter;

    float filt_true[NUM_DATA] = {
        0.00000,   0.00000,   0.00000,   0.00000,   0.00000,   0.00000,   0.00000,   0.00000,   0.00000,   0.00000,   5.08737,   13.38217,  14.40819,
        12.40480,  12.94420,  13.13717,  12.96024,  12.98783,  13.01228,  12.99784,  12.99839,  13.00102,  12.99994,  12.99982,  13.00008,  13.00001,
        12.99998,  13.00001,  13.00000,  13.00000,  13.00000,  13.00000,  13.00000,  13.00000,  13.00000,  2.82527,   -13.76435, -15.81639, -11.80960,
        -12.88839, -13.27434, -12.92048, -12.97567, -13.02456, -12.99569, -12.99678, -13.00203, -12.99988, -12.99965, -13.00015, -13.00001, -12.99997,
        -13.00001, -13.00000, -13.00000, -13.00000, -13.00000, -13.00000, -13.00000, -13.00000, -2.82527,  13.76435,  15.81639,  11.80960,  12.88839,
        13.27434,  12.92048,  12.97567,  13.02456,  12.99569,  12.99678,  13.00203,  12.99988,  12.99965,  13.00015,  13.00001,  12.99997,  13.00001,
        13.00000,  13.00000,  13.00000,  13.00000,  13.00000,  13.00000,  13.00000,  2.82527,   -13.76435, -15.81639, -11.80960, -12.88839, -13.27434,
        -12.92048, -12.97567, -13.02456, -12.99569, -12.99678, -13.00203, -12.99988, -12.99965, -13.00015, -13.00001, -12.99997, -13.00001, -13.00000,
        -13.00000, -13.00000, -13.00000, -13.00000, -13.00000, -13.00000, -2.82527};

    IIRFilterInitLP(&filter, 3.0, 100);

    for (uint8_t ii = 0; ii < NUM_DATA; ii++) {
        assert_float_equal(IIRFilterProcess(&filter, input[ii]), filt_true[ii], 1e-5);
    }
}

static void test_IIRFilterReset(void** state) {
    (void)state; /* unused */
    IIRFilterGeneric_t filter;
    IIRFilterInit(&filter, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0);
    IIRFilterReset(&filter);
    assert_float_equal(filter.i1, 0.0, 1e-5);
    assert_float_equal(filter.i2, 0.0, 1e-5);
    assert_float_equal(filter.i3, 0.0, 1e-5);
    assert_float_equal(filter.o1, 0.0, 1e-5);
    assert_float_equal(filter.o2, 0.0, 1e-5);
    assert_float_equal(filter.o3, 0.0, 1e-5);

    IIRFilterSetValue(&filter, 10.0);
    assert_float_equal(filter.i1, 10.0, 1e-5);
    assert_float_equal(filter.i2, 10.0, 1e-5);
    assert_float_equal(filter.i3, 10.0, 1e-5);
    assert_float_equal(filter.o1, 10.0, 1e-5);
    assert_float_equal(filter.o2, 10.0, 1e-5);
    assert_float_equal(filter.o3, 10.0, 1e-5);
}

static void test_IIRFilterDerivative(void** state) {
    (void)state; /* unused */
    IIRFilterDerivative_t filter;

    float diff_true[NUM_DATA] = {
        0.00000,  0.00000,    0.00000,   0.00000,   0.00000,   0.00000,  0.00000,  0.00000,    0.00000,   0.00000,   86.66667, 28.88889, 9.62963,  3.20988,
        1.06996,  0.35665,    0.11888,   0.03963,   0.01321,   0.00440,  0.00147,  0.00049,    0.00016,   0.00005,   0.00002,  0.00001,  0.00000,  0.00000,
        0.00000,  0.00000,    0.00000,   0.00000,   0.00000,   0.00000,  0.00000,  -173.33333, -57.77778, -19.25926, -6.41975, -2.13992, -0.71331, -0.23777,
        -0.07926, -0.02642,   -0.00881,  -0.00294,  -0.00098,  -0.00033, -0.00011, -0.00004,   -0.00001,  -0.00000,  -0.00000, -0.00000, -0.00000, -0.00000,
        -0.00000, -0.00000,   -0.00000,  -0.00000,  173.33333, 57.77778, 19.25926, 6.41975,    2.13992,   0.71331,   0.23777,  0.07926,  0.02642,  0.00881,
        0.00294,  0.00098,    0.00033,   0.00011,   0.00004,   0.00001,  0.00000,  0.00000,    0.00000,   0.00000,   0.00000,  0.00000,  0.00000,  0.00000,
        0.00000,  -173.33333, -57.77778, -19.25926, -6.41975,  -2.13992, -0.71331, -0.23777,   -0.07926,  -0.02642,  -0.00881, -0.00294, -0.00098, -0.00033,
        -0.00011, -0.00004,   -0.00001,  -0.00000,  -0.00000,  -0.00000, -0.00000, -0.00000,   -0.00000,  -0.00000,  -0.00000, -0.00000, 173.33333};

    IIRFilterDerivativeInit(&filter, 10.0, 100);

    for (uint8_t ii = 0; ii < NUM_DATA; ii++) {
        assert_float_equal(IIRFilterDerivativeProcess(&filter, input[ii]), diff_true[ii], 1e-5);
    }

    IIRFilterDerivativeSetValue(&filter, 2.0);
    assert_float_equal(filter.i1, 2.0, 1e-5);
    assert_float_equal(filter.output, 2.0, 1e-5);

    IIRFilterDerivativeReset(&filter);
    assert_float_equal(filter.i1, 0.0, 1e-5);
    assert_float_equal(filter.output, 0.0, 1e-5);
}

static void test_IIRFilterIntegrator(void** state) {
    (void)state; /* unused */
    IIRFilterIntegrator_t filter;

    float int_true[NUM_DATA] = {
        0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.65000,  1.95000,  3.25000,  4.55000,
        5.85000,  7.15000,  8.45000,  9.75000,  11.05000, 12.35000, 13.65000, 14.95000, 16.25000, 17.55000, 18.85000, 20.15000, 21.45000, 22.75000,
        24.05000, 25.35000, 26.65000, 27.95000, 29.25000, 30.55000, 31.85000, 31.85000, 30.55000, 29.25000, 27.95000, 26.65000, 25.35000, 24.05000,
        22.75000, 21.45000, 20.15000, 18.85000, 17.55000, 16.25000, 14.95000, 13.65000, 12.35000, 11.05000, 9.75000,  8.45000,  7.15000,  5.85000,
        4.55000,  3.25000,  1.95000,  0.65000,  0.65000,  1.95000,  3.25000,  4.55000,  5.85000,  7.15000,  8.45000,  9.75000,  11.05000, 12.35000,
        13.65000, 14.95000, 16.25000, 17.55000, 18.85000, 20.15000, 21.45000, 22.75000, 24.05000, 25.35000, 26.65000, 27.95000, 29.25000, 30.55000,
        31.85000, 31.85000, 30.55000, 29.25000, 27.95000, 26.65000, 25.35000, 24.05000, 22.75000, 21.45000, 20.15000, 18.85000, 17.55000, 16.25000,
        14.95000, 13.65000, 12.35000, 11.05000, 9.75000,  8.45000,  7.15000,  5.85000,  4.55000,  3.25000,  1.95000,  0.65000,  0.65000};

    IIRFilterIntegratorInit(&filter, 100);

    for (uint8_t ii = 0; ii < NUM_DATA; ii++) {
        assert_float_equal(IIRFilterIntegratorProcess(&filter, input[ii]), int_true[ii], 1e-5);
    }

    IIRFilterIntegratorSetValue(&filter, 5.0);
    assert_float_equal(filter.i1, 5.0, 1e-5);
    assert_float_equal(filter.output, 5.0, 1e-5);

    IIRFilterIntegratorReset(&filter);
    assert_float_equal(filter.i1, 0.0, 1e-5);
    assert_float_equal(filter.output, 0.0, 1e-5);
}

static void test_IIRFilterLeakyIntegrator(void** state) {
    (void)state; /* unused */
    IIRFilterLeakyIntegrator_t filter;
    const float dT_ms = 100.0f;
    const float tau_ms = 1000.0f;
    const float expected_n0 = (dT_ms * 1e-3f * tau_ms * 1e-3f) / (2.0f * tau_ms * 1e-3f + dT_ms * 1e-3f);
    const float expected_leak = (2.0f * tau_ms * 1e-3f - dT_ms * 1e-3f) / (2.0f * tau_ms * 1e-3f + dT_ms * 1e-3f);
    float leak_true[NUM_DATA] = {
        0.00000,   0.00000,   0.00000,   0.00000,   0.00000,  0.00000,  0.00000,  0.00000,  0.00000,   0.00000,   0.61905,   1.79819,   2.86503,  3.83026,
        4.70357,   5.49371,   6.20859,   6.85539,   7.44059,  7.97006,  8.44910,  8.88252,  9.27466,   9.62946,   9.95046,   10.24089,  10.50366, 10.74141,
        10.95651,  11.15113,  11.32721,  11.48653,  11.63067, 11.76108, 11.87907, 10.74773, 8.48604,   6.43975,   4.58835,   2.91327,   1.39772,  0.02651,
        -1.21411,  -2.33658,  -3.35214,  -4.27099,  -5.10232, -5.85448, -6.53501, -7.15072, -7.70779,  -8.21181,  -8.66783,  -9.08042,  -9.45371, -9.79145,
        -10.09703, -10.37350, -10.62365, -10.84997, -9.81664, -7.64362, -5.67756, -3.89875, -2.28934,  -0.83322,  0.48423,   1.67621,   2.75467,  3.73041,
        4.61323,   5.41197,   6.13464,   6.78848,   7.38006,  7.91529,  8.39955,  8.83769,  9.23410,   9.59275,   9.91725,   10.21085,  10.47648, 10.71682,
        10.93426,  9.89290,   7.71263,   5.74000,   3.95524,  2.34045,  0.87946,  -0.44240, -1.63836,  -2.72042,  -3.69943,  -4.58520,  -5.38661, -6.11169,
        -6.76772,  -7.36127,  -7.89829,  -8.38417,  -8.82377, -9.22151, -9.58137, -9.90695, -10.20153, -10.46805, -10.70919, -10.92736, -9.88666,
    };

    IIRFilterLeakyIntegratorInit(&filter, dT_ms, tau_ms);
    assert_float_equal(filter.n0, expected_n0, 1e-6);
    assert_float_equal(filter.leak, expected_leak, 1e-6);
    assert_float_equal(filter.i1, 0.0f, 1e-5);
    assert_float_equal(filter.output, 0.0f, 1e-5);

    for (uint8_t ii = 0; ii < NUM_DATA; ii++) {
        assert_float_equal(IIRFilterLeakyIntegratorProcess(&filter, input[ii]), leak_true[ii], 1e-5);
    }

    IIRFilterLeakyIntegratorSetValue(&filter, 2.0f);
    assert_float_equal(filter.i1, 2.0f, 1e-5);
    assert_float_equal(filter.output, 2.0f, 1e-5);

    IIRFilterLeakyIntegratorReset(&filter);
    assert_float_equal(filter.i1, 0.0f, 1e-5);
    assert_float_equal(filter.output, 0.0f, 1e-5);
}

int main(void) {
    const struct CMUnitTest test_IIRFilters[] = {
        cmocka_unit_test(test_IIRFilterInit),       cmocka_unit_test(test_IIRFilterInitLP),
        cmocka_unit_test(test_IIRFilterInitHP),     cmocka_unit_test(test_IIRFilterInitBP),
        cmocka_unit_test(test_IIRFilterInitBS),     cmocka_unit_test(test_IIRFilterProcess),
        cmocka_unit_test(test_IIRFilterReset),      cmocka_unit_test(test_IIRFilterDerivative),
        cmocka_unit_test(test_IIRFilterIntegrator), cmocka_unit_test(test_IIRFilterLeakyIntegrator),
    };

    return cmocka_run_group_tests(test_IIRFilters, NULL, NULL);
}
