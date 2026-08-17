/* BEGIN Header */
/**
 ******************************************************************************
 * \file            test_quaternion.c
 * \author          Andrea Vivani
 * \brief           Unit tests for quaternion.c
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
#include "basicMath.h"
#include "quaternion.h"

#include <cmocka.h>

/* Support functions ---------------------------------------------------------*/

void* ADVUtils_testCalloc(const size_t number_of_elements, const size_t size) {
    if (number_of_elements > 0) {
        return test_calloc(number_of_elements, size);
    } else {
        return NULL;
    }
}

void* ADVUtils_testMalloc(const size_t size) {
    if (size > 0) {
        return test_malloc(size);
    } else {
        return NULL;
    }
}

static uint8_t skipAssert = 0;

void ADVUtils_testAssert(const int result, const char* const expression, const char* const file, const int line) {
    if (skipAssert) {
        return;
    } else {
        mock_assert(result, expression, file, line);
    }
}

/* Functions -----------------------------------------------------------------*/

static void test_quaternionNorm(void** state) {
    (void)state; /* unused */

    quaternion_t q = {1.0f, 2.0f, 3.0f, 4.0f, {0.0f, 0.0f, 0.0f}};
    quaternionNorm(&q);

    float norm = sqrtf(1.0f + 4.0f + 9.0f + 16.0f);
    assert_float_equal(q.q0, 1.0f / norm, 1e-5);
    assert_float_equal(q.q1, 2.0f / norm, 1e-5);
    assert_float_equal(q.q2, 3.0f / norm, 1e-5);
    assert_float_equal(q.q3, 4.0f / norm, 1e-5);

    // Edge case: Zero quaternion
    quaternion_t q_zero = {0.0f, 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f}};
    quaternionNorm(&q_zero);
    assert_float_equal(q_zero.q0, 0.0f, 1e-5);
    assert_float_equal(q_zero.q1, 0.0f, 1e-5);
    assert_float_equal(q_zero.q2, 0.0f, 1e-5);
    assert_float_equal(q_zero.q3, 0.0f, 1e-5);

    // Edge case: Negative components
    quaternion_t q_neg = {-1.0f, -2.0f, -3.0f, -4.0f, {0.0f, 0.0f, 0.0f}};
    quaternionNorm(&q_neg);
    assert_float_equal(q_neg.q0, -1.0f / norm, 1e-5);
    assert_float_equal(q_neg.q1, -2.0f / norm, 1e-5);
    assert_float_equal(q_neg.q2, -3.0f / norm, 1e-5);
    assert_float_equal(q_neg.q3, -4.0f / norm, 1e-5);
}

static void test_quaternionMult(void** state) {
    (void)state; /* unused */

    quaternion_t qa = {1.0f, 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f}};
    quaternion_t qb = {0.0f, 1.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f}};
    quaternion_t qo;
    quaternionMult(&qa, &qb, &qo);

    assert_float_equal(qo.q0, 0.0f, 1e-5);
    assert_float_equal(qo.q1, 1.0f, 1e-5);
    assert_float_equal(qo.q2, 0.0f, 1e-5);
    assert_float_equal(qo.q3, 0.0f, 1e-5);

    // Edge case: Identity quaternion multiplication
    quaternion_t q_identity = {1.0f, 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f}};
    quaternionMult(&qa, &q_identity, &qo);
    assert_float_equal(qo.q0, qa.q0, 1e-5);
    assert_float_equal(qo.q1, qa.q1, 1e-5);
    assert_float_equal(qo.q2, qa.q2, 1e-5);
    assert_float_equal(qo.q3, qa.q3, 1e-5);

    // Edge case: Zero quaternion multiplication
    quaternion_t q_zero = {0.0f, 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f}};
    quaternionMult(&qa, &q_zero, &qo);
    assert_float_equal(qo.q0, 0.0f, 1e-5);
    assert_float_equal(qo.q1, 0.0f, 1e-5);
    assert_float_equal(qo.q2, 0.0f, 1e-5);
    assert_float_equal(qo.q3, 0.0f, 1e-5);
}

static void test_quaternionRotation(void** state) {
    (void)state; /* unused */

    quaternion_t qr = {1.0f, 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f}};
    quaternion_t qv = {0.0f, 1.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f}};
    quaternion_t qo;
    quaternionRotation(&qr, &qv, &qo);

    assert_float_equal(qo.q0, 0.0f, 1e-5);
    assert_float_equal(qo.q1, 1.0f, 1e-5);
    assert_float_equal(qo.q2, 0.0f, 1e-5);
    assert_float_equal(qo.q3, 0.0f, 1e-5);

    // Edge case: Rotation by 180 degrees
    quaternion_t qr_180 = {0.0f, 1.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f}}; // 180 degree rotation around the x-axis
    quaternion_t qv2 = {0.0f, 0.0f, 1.0f, 0.0f, {0.0f, 0.0f, 0.0f}};
    quaternionRotation(&qr_180, &qv2, &qo);
    assert_float_equal(qo.q1, 0.0f, 1e-5);
    assert_float_equal(qo.q2, -1.0f, 1e-5);
    assert_float_equal(qo.q3, 0.0f, 1e-5);
}

static void test_quaternionConj(void** state) {
    (void)state; /* unused */

    quaternion_t qa = {1.0f, 2.0f, 3.0f, 4.0f, {0.0f, 0.0f, 0.0f}};
    quaternion_t qo;
    quaternionConj(&qa, &qo);

    assert_float_equal(qo.q0, 1.0f, 1e-5);
    assert_float_equal(qo.q1, -2.0f, 1e-5);
    assert_float_equal(qo.q2, -3.0f, 1e-5);
    assert_float_equal(qo.q3, -4.0f, 1e-5);

    // Edge case: Zero quaternion conjugate
    quaternion_t q_zero = {0.0f, 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f}};
    quaternionConj(&q_zero, &qo);
    assert_float_equal(qo.q0, 0.0f, 1e-5);
    assert_float_equal(qo.q1, 0.0f, 1e-5);
    assert_float_equal(qo.q2, 0.0f, 1e-5);
    assert_float_equal(qo.q3, 0.0f, 1e-5);
}

static void test_quaternionToEuler(void** state) {
    (void)state; /* unused */

    quaternion_t qr = {1.0f, 0.0f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f}};
    axis3f_t ea;
    quaternionToEuler(&qr, &ea);

    assert_float_equal(ea.x, 0.0f, 1e-5);
    assert_float_equal(ea.y, 0.0f, 1e-5);
    assert_float_equal(ea.z, 0.0f, 1e-5);

    // Edge case: 90 degree rotation around the z-axis
    quaternion_t qr_90_z = {0.7071068f, 0.0f, 0.0f, 0.7071068f, {0.0f, 0.0f, 0.0f}}; // 90 degrees around z-axis
    quaternionToEuler(&qr_90_z, &ea);
    assert_float_equal(ea.x, 0.0f, 1e-5);
    assert_float_equal(ea.y, 0.0f, 1e-5);
    assert_float_equal(ea.z, constPI / 2.f, 1e-5);

    // Edge case: 92 degree rotation around the x-axis. Angles should remain unchanged
    quaternion_t qr_92_x = {0.6946f, 0.7193f, 0.0f, 0.0f, {0.0f, 0.0f, 0.0f}}; // 92 degrees around x-axis
    axis3f_t ea_pre;
    ea_pre.x = ea.x;
    ea_pre.y = ea.y;
    ea_pre.z = ea.z;
    quaternionToEuler(&qr_92_x, &ea);
    assert_float_equal(ea.x, ea_pre.x, 1e-5);
    assert_float_equal(ea.y, 0.0f, 1e-5);
    assert_float_equal(ea.z, 0.0f, 1e-5);
}

static void test_quaternionToMatrix(void** state) {
    (void)state; /* unused */
    /* 40 deg about unit axis [1,2,2]/3; active DCM values from reference */
    quaternion_t q = {0.9396926f, 0.1140067f, 0.2280134f, 0.2280134f, {0.0f, 0.0f, 0.0f}};
    float M_data[9];
    matrix_t M;
    matrixInitStatic(&M, M_data, 3, 3);
    quaternionToMatrix(&q, &M);
    float M_exp[9] = {0.792040f, -0.376535f, 0.480515f, 0.480515f, 0.870025f, -0.110282f, -0.376535f, 0.318243f, 0.870025f};
    for (uint8_t i = 0; i < 9; i++) {
        assert_float_equal(M.data[i], M_exp[i], 1e-4);
    }
    /* cross-check: active matrix applied to v equals quaternionRotation(conj(q), v) */
    quaternion_t qc;
    quaternionConj(&q, &qc);
    quaternion_t qv = {0.0f, 1.0f, -2.0f, 0.5f, {0.0f, 0.0f, 0.0f}};
    quaternion_t qo;
    quaternionRotation(&qc, &qv, &qo);
    float vx = 1.0f, vy = -2.0f, vz = 0.5f;
    float mx = (ELEM(M, 0, 0) * vx) + (ELEM(M, 0, 1) * vy) + (ELEM(M, 0, 2) * vz);
    float my = (ELEM(M, 1, 0) * vx) + (ELEM(M, 1, 1) * vy) + (ELEM(M, 1, 2) * vz);
    float mz = (ELEM(M, 2, 0) * vx) + (ELEM(M, 2, 1) * vy) + (ELEM(M, 2, 2) * vz);
    assert_float_equal(mx, qo.q1, 1e-5);
    assert_float_equal(my, qo.q2, 1e-5);
    assert_float_equal(mz, qo.q3, 1e-5);
    /* dimension assert (result not 3x3) */
    float bad_data[6];
    matrix_t bad;
    matrixInitStatic(&bad, bad_data, 2, 3);
    skipAssert = 0;
    expect_assert_failure(quaternionToMatrix(&q, &bad));
}

static void test_quaternionFromAxisAngle(void** state) {
    (void)state; /* unused */
    axis3f_t axis = {1.0f / 3.0f, 2.0f / 3.0f, 2.0f / 3.0f};
    quaternion_t q;
    quaternionFromAxisAngle(&axis, 0.6981317f, &q); /* 40 deg */
    assert_float_equal(q.q0, 0.9396926f, 1e-5);
    assert_float_equal(q.q1, 0.1140067f, 1e-5);
    assert_float_equal(q.q2, 0.2280134f, 1e-5);
    assert_float_equal(q.q3, 0.2280134f, 1e-5);
#ifdef AVOID_GIMBAL_LOCK
    assert_float_equal(q.ea_pre.x, 0.0f, 1e-5);
    assert_float_equal(q.ea_pre.y, 0.0f, 1e-5);
    assert_float_equal(q.ea_pre.z, 0.0f, 1e-5);
#endif
    /* zero angle -> identity quaternion */
    quaternion_t qi;
    quaternionFromAxisAngle(&axis, 0.0f, &qi);
    assert_float_equal(qi.q0, 1.0f, 1e-5);
    assert_float_equal(qi.q1, 0.0f, 1e-5);
    assert_float_equal(qi.q2, 0.0f, 1e-5);
    assert_float_equal(qi.q3, 0.0f, 1e-5);
}

int main(void) {
    const struct CMUnitTest test_quaternion[] = {
        cmocka_unit_test(test_quaternionNorm), cmocka_unit_test(test_quaternionMult),    cmocka_unit_test(test_quaternionRotation),
        cmocka_unit_test(test_quaternionConj), cmocka_unit_test(test_quaternionToEuler),
        cmocka_unit_test(test_quaternionToMatrix), cmocka_unit_test(test_quaternionFromAxisAngle),
    };

    return cmocka_run_group_tests(test_quaternion, NULL, NULL);
}
