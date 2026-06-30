/* BEGIN Header */
/**
 ******************************************************************************
 * \file            test_PID.c
 * \author          Andrea Vivani
 * \brief           Unit tests for PID.c
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
#include "PID.h"

#include <cmocka.h>

/* Macros --------------------------------------------------------------------*/
#define NUM_DATA 111

/* Private variables ---------------------------------------------------------*/
static float input[NUM_DATA] = {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,
                                13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13,
                                -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, 13,  13,  13,  13,  13,  13,  13,  13,  13,
                                13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  -13, -13, -13, -13, -13, -13, -13,
                                -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, 13};

/* Functions -----------------------------------------------------------------*/
static void test_PID_init(void** state) {
    (void)state; /* unused */

    PID_t pid;
    PID_init(&pid, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 1000.0f, -10.0f, 10.0f);

    assert_float_equal(pid.kp, 1.0f, 1e-5);
    assert_float_equal(pid.ki, 0.5f * 1.0f * 1000.0f * 1e-3f, 1e-5);
    assert_float_equal(pid.kd, (2 * 1.0f * 1.0f) / (2 + 1.0f * 1000.0f * 1e-3f), 1e-5);
    assert_float_equal(pid.nd, 1.0f, 1e-5);
    assert_float_equal(pid.dT, 1000.0f * 1e-3f, 1e-5);
    assert_float_equal(pid.satMin, -10.0f, 1e-5);
    assert_float_equal(pid.satMax, 10.0f, 1e-5);
    assert_float_equal(pid.kb, 0.5f * 0.5f * 1000.0f * 1e-3f, 1e-5);
    assert_float_equal(pid.kf, (2 - 1.0f * 1000.0f * 1e-3) / (2 + 1.0f * 1000.0f * 1e-3), 1e-5);
}

static void test_PID_calc(void** state) {
    (void)state; /* unused */
    PID_t pid;

    float PID_true[NUM_DATA] = {
        0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  6.26167,  6.07389,  6.27130,  6.59710,
        6.96570,  7.34857,  7.73619,  8.12540,  8.51513,  8.90504,  9.29501,  9.68500,  10.07500, 10.46500, 10.85500, 11.24500, 11.63500, 12.02500,
        12.41500, 12.80500, 13.19500, 13.58500, 13.97500, 14.36500, 14.75500, 2.62167,  3.38722,  3.38241,  3.12080,  2.77360,  2.39787,  2.01262,
        1.62421,  1.23474,  0.84491,  0.45497,  0.06499,  -0.32500, -0.71500, -1.10500, -1.49500, -1.88500, -2.27500, -2.66500, -3.05500, -3.44500,
        -3.83500, -4.22500, -4.61500, -5.00500, 7.12833,  6.36278,  6.36759,  6.62920,  6.97640,  7.35213,  7.73738,  8.12579,  8.51526,  8.90509,
        9.29503,  9.68501,  10.07500, 10.46500, 10.85500, 11.24500, 11.63500, 12.02500, 12.41500, 12.80500, 13.19500, 13.58500, 13.97500, 14.36500,
        14.75500, 2.62167,  3.38722,  3.38241,  3.12080,  2.77360,  2.39787,  2.01262,  1.62421,  1.23474,  0.84491,  0.45497,  0.06499,  -0.32500,
        -0.71500, -1.10500, -1.49500, -1.88500, -2.27500, -2.66500, -3.05500, -3.44500, -3.83500, -4.22500, -4.61500, -5.00500, 7.12833};

    PID_init(&pid, 0.4f, 0.3f, 0.01f, 10.0f, 0.0f, 100.0f, -100.0f, 100.0f);

    for (uint8_t ii = 0; ii < NUM_DATA; ii++) {
        PID_calc(&pid, input[ii], 0);
        assert_float_equal(PID_getOutput(&pid), PID_true[ii], 1e-5);
    }
}

static void test_PID_calcAeroClamp(void** state) {
    (void)state; /* unused */
    PID_t pid;

    float PID_true[NUM_DATA] = {
        0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  6.26167,  6.07389,  6.27130,  6.59710,
        6.96570,  7.34857,  7.73619,  8.12540,  8.51513,  8.90504,  9.29501,  9.68500,  10.07500, 10.46500, 10.85500, 11.24500, 11.63500, 12.02500,
        12.20000, 12.20000, 12.20000, 12.20000, 12.20000, 12.20000, 12.20000, -0.12833, 0.63722,  0.63241,  0.37080,  0.02360,  -0.35213, -0.73738,
        -1.12579, -1.51526, -1.90509, -2.29503, -2.68501, -3.07500, -3.46500, -3.85500, -4.24500, -4.63500, -5.02500, -5.41500, -5.80500, -6.19500,
        -6.58500, -6.97500, -7.36500, -7.75500, 4.37833,  3.61278,  3.61759,  3.87920,  4.22640,  4.60213,  4.98738,  5.37579,  5.76526,  6.15509,
        6.54503,  6.93501,  7.32500,  7.71500,  8.10500,  8.49500,  8.88500,  9.27500,  9.66500,  10.05500, 10.44500, 10.83500, 11.22500, 11.61500,
        12.00500, -0.12833, 0.63722,  0.63241,  0.37080,  0.02360,  -0.35213, -0.73738, -1.12579, -1.51526, -1.90509, -2.29503, -2.68501, -3.07500,
        -3.46500, -3.85500, -4.24500, -4.63500, -5.02500, -5.41500, -5.80500, -6.19500, -6.58500, -6.97500, -7.36500, -7.75500, 4.37833};

    PID_init(&pid, 0.4f, 0.3f, 0.01f, 10.0f, 0.0f, 100.0f, -7.0f, 7.0f);

    for (uint8_t ii = 0; ii < NUM_DATA; ii++) {
        utilsStatus_t retVal = PID_calcAeroClamp(&pid, input[ii], 0);
        if ((PID_getOutput(&pid) == 7.0) || (PID_getOutput(&pid) == -7.0)) {
            assert_int_equal(retVal, UTILS_STATUS_FULL);
        }
        assert_float_equal(PID_getOutput(&pid), PID_true[ii], 1e-5);
    }
}

static void test_PID_calcIntegralClamp(void** state) {
    (void)state; /* unused */
    PID_t pid;

    float PID_true[NUM_DATA] = {
        0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  6.26167,  6.07389,  6.27130,  6.59710,
        6.96570,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,
        7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  -5.17833, -4.41278, -4.41759, -4.67920, -5.02640, -5.40213, -5.78738,
        -6.17579, -6.56526, -6.95509, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000,
        -7.00000, -7.00000, -7.00000, -7.00000, 5.17833,  4.41278,  4.41759,  4.67920,  5.02640,  5.40213,  5.78738,  6.17579,  6.56526,  6.95509,
        7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,
        7.00000,  -5.17833, -4.41278, -4.41759, -4.67920, -5.02640, -5.40213, -5.78738, -6.17579, -6.56526, -6.95509, -7.00000, -7.00000, -7.00000,
        -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, 5.17833};

    PID_init(&pid, 0.4f, 0.3f, 0.01f, 10.0f, 0.0f, 100.0f, -7.0f, 7.0f);

    for (uint8_t ii = 0; ii < NUM_DATA; ii++) {
        utilsStatus_t retVal = PID_calcIntegralClamp(&pid, input[ii], 0);
        if ((PID_getOutput(&pid) == 7.0) || (PID_getOutput(&pid) == -7.0)) {
            assert_int_equal(retVal, UTILS_STATUS_FULL);
        }
        assert_float_equal(PID_getOutput(&pid), PID_true[ii], 1e-5);
    }
}

static void test_PID_calcBackCalc(void** state) {
    (void)state; /* unused */
    PID_t pid;

    float PID_true[NUM_DATA] = {
        0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  6.26167,  6.07389,  6.27130,  6.59710,
        6.96570,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,
        7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  -4.02925, -3.26370, -3.26851, -3.53012, -3.87732, -4.25305, -4.63830,
        -5.02671, -5.41618, -5.80601, -6.19595, -6.58593, -6.97592, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000,
        -7.00000, -7.00000, -7.00000, -7.00000, 4.04396,  3.27841,  3.28322,  3.54483,  3.89203,  4.26776,  4.65301,  5.04142,  5.43089,  5.82072,
        6.21066,  6.60064,  6.99063,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,
        7.00000,  -4.04376, -3.27820, -3.28302, -3.54462, -3.89182, -4.26756, -4.65280, -5.04122, -5.43069, -5.82051, -6.21045, -6.60043, -6.99043,
        -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, 4.04376};

    PID_init(&pid, 0.4f, 0.3f, 0.01f, 10.0f, 3.0f, 100.0f, -7.0f, 7.0f);

    for (uint8_t ii = 0; ii < NUM_DATA; ii++) {
        utilsStatus_t retVal = PID_calcBackCalc(&pid, input[ii], 0);
        if ((PID_getOutput(&pid) == 7.0) || (PID_getOutput(&pid) == -7.0)) {
            assert_int_equal(retVal, UTILS_STATUS_FULL);
        }
        assert_float_equal(PID_getOutput(&pid), PID_true[ii], 1e-5);
    }
}

static void test_PID_calc_derivMeasure(void** state) {
    (void)state; /* unused */
    PID_t pid;

    float PID_true[NUM_DATA] = {0.00000,  0.00000,  0.00000,  -3.13083, -3.03694, -3.13565, -3.29855, -3.48285, -3.67428, -3.86809, 1.33230, 1.52743, 1.72248,
                                1.91749,  2.11250,  2.30750,  2.50250,  2.69750,  2.89250,  3.08750,  3.28250,  3.47750,  3.67250,  3.86750, 4.06250, 4.25750,
                                4.45250,  4.64750,  11.10417, 11.11139, 11.50380, 12.02460, 12.58820, 13.16607, 13.74869, 3.54290,  3.34763, 3.15254, 2.95752,
                                2.76251,  2.56750,  2.37250,  2.17750,  1.98250,  1.78750,  1.59250,  1.39750,  1.20250,  1.00750,  0.81250, 0.61750, 0.42250,
                                0.22750,  -6.22916, -6.23639, -6.62879, -7.14960, -7.71320, -8.29107, -8.87369, 1.33211,  1.52737,  1.72246, 1.91749, 2.11250,
                                2.30750,  2.50250,  2.69750,  2.89250,  3.08750,  3.28250,  3.47750,  3.67250,  3.86750,  4.06250,  4.25750, 4.45250, 4.64750,
                                11.10417, 11.11139, 11.50380, 12.02460, 12.58820, 13.16607, 13.74869, 3.54290,  3.34763,  3.15255,  2.95752, 2.76251, 2.56751,
                                2.37250,  2.17750,  1.98250,  1.78750,  1.59251,  1.39751,  1.20251,  1.00750,  0.81250,  0.61750,  0.42250, 0.22750, -6.22916,
                                -3.10555, -3.59185, -4.01395, -4.41465, -4.80821, -5.19940, 5.20020};

    PID_init(&pid, 0.4f, 0.3f, 0.01f, 10.0f, 0.0f, 100.0f, -100.0f, 100.0f);
    PID_setDerivativeMode(&pid, PID_DERIV_ON_MEASURE);

    for (uint8_t ii = 0; ii < NUM_DATA; ii++) {
        PID_calc(&pid, input[ii], 0.5f * input[(ii + 7) % NUM_DATA]);
        assert_float_equal(PID_getOutput(&pid), PID_true[ii], 1e-5);
    }
}

static void test_PID_calcAeroClamp_derivMeasure(void** state) {
    (void)state; /* unused */
    PID_t pid;

    float PID_true[NUM_DATA] = {0.00000,  0.00000,  0.00000,  -3.13083, -3.03694, -3.13565, -3.29855, -3.48285, -3.67428, -3.86809, 1.33230, 1.52743, 1.72248,
                                1.91749,  2.11250,  2.30750,  2.50250,  2.69750,  2.89250,  3.08750,  3.28250,  3.47750,  3.67250,  3.86750, 4.06250, 4.25750,
                                4.45250,  4.64750,  11.10417, 11.11139, 11.50380, 12.02460, 12.58820, 13.16607, 13.74869, 3.54290,  3.34763, 3.15254, 2.95752,
                                2.76251,  2.56750,  2.37250,  2.17750,  1.98250,  1.78750,  1.59250,  1.39750,  1.20250,  1.00750,  0.81250, 0.61750, 0.42250,
                                0.22750,  -6.22916, -6.23639, -6.62879, -7.14960, -7.71320, -8.29107, -8.87369, 1.33211,  1.52737,  1.72246, 1.91749, 2.11250,
                                2.30750,  2.50250,  2.69750,  2.89250,  3.08750,  3.28250,  3.47750,  3.67250,  3.86750,  4.06250,  4.25750, 4.45250, 4.64750,
                                11.10417, 11.11139, 11.50380, 12.02460, 12.58820, 13.16607, 13.74869, 3.54290,  3.34763,  3.15255,  2.95752, 2.76251, 2.56751,
                                2.37250,  2.17750,  1.98250,  1.78750,  1.59251,  1.39751,  1.20251,  1.00750,  0.81250,  0.61750,  0.42250, 0.22750, -6.22916,
                                -3.10555, -3.59185, -4.01395, -4.41465, -4.80821, -5.19940, 5.20020};

    PID_init(&pid, 0.4f, 0.3f, 0.01f, 10.0f, 0.0f, 100.0f, -7.0f, 7.0f);
    PID_setDerivativeMode(&pid, PID_DERIV_ON_MEASURE);

    for (uint8_t ii = 0; ii < NUM_DATA; ii++) {
        utilsStatus_t retVal = PID_calcAeroClamp(&pid, input[ii], 0.5f * input[(ii + 7) % NUM_DATA]);
        if ((PID_getOutput(&pid) == 7.0) || (PID_getOutput(&pid) == -7.0)) {
            assert_int_equal(retVal, UTILS_STATUS_FULL);
        }
        assert_float_equal(PID_getOutput(&pid), PID_true[ii], 1e-5);
    }
}

static void test_PID_calcIntegralClamp_derivMeasure(void** state) {
    (void)state; /* unused */
    PID_t pid;

    float PID_true[NUM_DATA] = {
        0.00000,  0.00000,  0.00000,  -3.13083, -3.03694, -3.13565, -3.29855, -3.48285, -3.67428, -3.86809, 1.33230,  1.52743,  1.72248,  1.91749,
        2.11250,  2.30750,  2.50250,  2.69750,  2.89250,  3.08750,  3.28250,  3.47750,  3.67250,  3.86750,  4.06250,  4.25750,  4.45250,  4.64750,
        7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  -0.55210, -0.74737, -0.94246, -1.13749, -1.33250, -1.52750, -1.72250,
        -1.91750, -2.11250, -2.30750, -2.50250, -2.69750, -2.89250, -3.08750, -3.28250, -3.47750, -3.67250, -3.86750, -7.00000, -7.00000, -7.00000,
        -7.00000, -7.00000, -7.00000, -7.00000, 1.33210,  1.52737,  1.72246,  1.91749,  2.11250,  2.30750,  2.50250,  2.69750,  2.89250,  3.08750,
        3.28250,  3.47750,  3.67250,  3.86750,  4.06250,  4.25750,  4.45250,  4.64750,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,
        7.00000,  -0.55210, -0.74737, -0.94246, -1.13749, -1.33250, -1.52750, -1.72250, -1.91750, -2.11250, -2.30750, -2.50250, -2.69750, -2.89250,
        -3.08750, -3.28250, -3.47750, -3.67250, -3.86750, -7.00000, -6.61556, -6.90685, -6.93895, -6.94965, -6.95322, -6.95441, 3.64020};

    PID_init(&pid, 0.4f, 0.3f, 0.01f, 10.0f, 0.0f, 100.0f, -7.0f, 7.0f);
    PID_setDerivativeMode(&pid, PID_DERIV_ON_MEASURE);

    for (uint8_t ii = 0; ii < NUM_DATA; ii++) {
        utilsStatus_t retVal = PID_calcIntegralClamp(&pid, input[ii], 0.5f * input[(ii + 7) % NUM_DATA]);
        if ((PID_getOutput(&pid) == 7.0) || (PID_getOutput(&pid) == -7.0)) {
            assert_int_equal(retVal, UTILS_STATUS_FULL);
        }
        assert_float_equal(PID_getOutput(&pid), PID_true[ii], 1e-5);
    }
}

static void test_PID_calcBackCalc_derivMeasure(void** state) {
    (void)state; /* unused */
    PID_t pid;

    float PID_true[NUM_DATA] = {
        0.00000,  0.00000,  0.00000,  -3.13083, -3.03694, -3.13565, -3.29855, -3.48285, -3.67428, -3.86809, 1.33230,  1.52743,  1.72248,  1.91749,
        2.11250,  2.30750,  2.50250,  2.69750,  2.89250,  3.08750,  3.28250,  3.47750,  3.67250,  3.86750,  4.06250,  4.25750,  4.45250,  4.64750,
        7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  -1.52323, -1.71850, -1.91359, -2.10862, -2.30363, -2.49863, -2.69363,
        -2.88863, -3.08363, -3.27863, -3.47363, -3.66863, -3.86363, -4.05863, -4.25363, -4.44863, -4.64363, -4.83863, -7.00000, -7.00000, -7.00000,
        -7.00000, -7.00000, -7.00000, -7.00000, 1.50749,  1.70276,  1.89785,  2.09288,  2.28789,  2.48289,  2.67789,  2.87289,  3.06789,  3.26289,
        3.45789,  3.65289,  3.84789,  4.04289,  4.23789,  4.43289,  4.62789,  4.82289,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,  7.00000,
        7.00000,  -1.50879, -1.70405, -1.89914, -2.09417, -2.28918, -2.48418, -2.67919, -2.87419, -3.06919, -3.26419, -3.45919, -3.65419, -3.84919,
        -4.04419, -4.23919, -4.43419, -4.62919, -4.82419, -7.00000, -6.96074, -7.00000, -7.00000, -7.00000, -7.00000, -7.00000, 2.45678};

    PID_init(&pid, 0.4f, 0.3f, 0.01f, 10.0f, 3.0f, 100.0f, -7.0f, 7.0f);
    PID_setDerivativeMode(&pid, PID_DERIV_ON_MEASURE);

    for (uint8_t ii = 0; ii < NUM_DATA; ii++) {
        utilsStatus_t retVal = PID_calcBackCalc(&pid, input[ii], 0.5f * input[(ii + 7) % NUM_DATA]);
        if ((PID_getOutput(&pid) == 7.0) || (PID_getOutput(&pid) == -7.0)) {
            assert_int_equal(retVal, UTILS_STATUS_FULL);
        }
        assert_float_equal(PID_getOutput(&pid), PID_true[ii], 1e-5);
    }
}

static void test_PID_setGet(void** state) {
    (void)state; /* unused */
    PID_t pid;
    PID_init(&pid, 11.1f, 12.2f, 13.3f, 100, 14.4f, 100, -7, 7);
    /* Proportional */
    assert_float_equal(PID_getKp(&pid), 11.1f, 1e-5);
    PID_setKp(&pid, 2.3);
    assert_float_equal(pid.kp, 2.3f, 1e-5);
    /* Integral*/
    assert_float_equal(PID_getKi(&pid), 12.2f, 1e-5);
    PID_setKi(&pid, 15.5f);
    assert_float_equal(pid.ki, 0.5f * 15.5f * 100 * 1e-3, 1e-5);
    PID_setIntegralValue(&pid, 135.4f);
    assert_float_equal(pid.DuI, 135.4f, 1e-5);
    /* Derivative */
    assert_float_equal(PID_getKd(&pid), 13.3f, 1e-5);
    PID_setKd(&pid, 4.4f, 3.2f);
    assert_float_equal(pid.kd, (2 * 4.4f * 3.2f) / (2 + 3.2f * 100.0f * 1e-3), 1e-5);
    assert_float_equal(pid.kf, (2 - 3.2f * 100.0f * 1e-3) / (2 + 3.2f * 100.0f * 1e-3), 1e-5);
    /* Derivative mode */
    assert_int_equal(PID_getDerivativeMode(&pid), PID_DERIV_ON_ERROR);
    PID_setDerivativeMode(&pid, PID_DERIV_ON_MEASURE);
    assert_int_equal(pid.derivMode, PID_DERIV_ON_MEASURE);
    /* Saturation */
    PID_setIntegralSaturation(&pid, -12, 12);
    assert_float_equal(pid.satMax, 12, 1e-5);
    assert_float_equal(pid.satMin, -12, 1e-5);
    /* Back-calculation */
    PID_setKb(&pid, 2.6);
    assert_float_equal(pid.kb, 0.5 * 2.6 * 100 * 1e-3, 1e-5);
    /* Reset */
    pid.DuD = 123;
    pid.DuI = 344;
    PID_reset(&pid);
    assert_float_equal(pid.DuD, 0, 1e-5);
    assert_float_equal(pid.DuI, 0, 1e-5);
}

int main(void) {
    const struct CMUnitTest test_PID[] = {
        cmocka_unit_test(test_PID_init),
        cmocka_unit_test(test_PID_calc),
        cmocka_unit_test(test_PID_calcAeroClamp),
        cmocka_unit_test(test_PID_calcIntegralClamp),
        cmocka_unit_test(test_PID_calcBackCalc),
        cmocka_unit_test(test_PID_calc_derivMeasure),
        cmocka_unit_test(test_PID_calcAeroClamp_derivMeasure),
        cmocka_unit_test(test_PID_calcIntegralClamp_derivMeasure),
        cmocka_unit_test(test_PID_calcBackCalc_derivMeasure),
        cmocka_unit_test(test_PID_setGet),
    };

    return cmocka_run_group_tests(test_PID, NULL, NULL);
}
