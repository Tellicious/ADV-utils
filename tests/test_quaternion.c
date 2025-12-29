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

/* Functions -----------------------------------------------------------------*/

static void test_quaternionNorm(void** state) {
    (void)state; /* unused */

    quaternion_t q = {1.0f, 2.0f, 3.0f, 4.0f};
    quaternionNorm(&q);

    float norm = sqrtf(1.0f + 4.0f + 9.0f + 16.0f);
    assert_float_equal(q.q0, 1.0f / norm, 1e-5);
    assert_float_equal(q.q1, 2.0f / norm, 1e-5);
    assert_float_equal(q.q2, 3.0f / norm, 1e-5);
    assert_float_equal(q.q3, 4.0f / norm, 1e-5);

    // Edge case: Zero quaternion
    quaternion_t q_zero = {0.0f, 0.0f, 0.0f, 0.0f};
    quaternionNorm(&q_zero);
    assert_float_equal(q_zero.q0, 0.0f, 1e-5);
    assert_float_equal(q_zero.q1, 0.0f, 1e-5);
    assert_float_equal(q_zero.q2, 0.0f, 1e-5);
    assert_float_equal(q_zero.q3, 0.0f, 1e-5);

    // Edge case: Negative components
    quaternion_t q_neg = {-1.0f, -2.0f, -3.0f, -4.0f};
    quaternionNorm(&q_neg);
    assert_float_equal(q_neg.q0, -1.0f / norm, 1e-5);
    assert_float_equal(q_neg.q1, -2.0f / norm, 1e-5);
    assert_float_equal(q_neg.q2, -3.0f / norm, 1e-5);
    assert_float_equal(q_neg.q3, -4.0f / norm, 1e-5);
}

static void test_quaternionMult(void** state) {
    (void)state; /* unused */

    quaternion_t qa = {1.0f, 0.0f, 0.0f, 0.0f};
    quaternion_t qb = {0.0f, 1.0f, 0.0f, 0.0f};
    quaternion_t qo;
    quaternionMult(&qa, &qb, &qo);

    assert_float_equal(qo.q0, 0.0f, 1e-5);
    assert_float_equal(qo.q1, 1.0f, 1e-5);
    assert_float_equal(qo.q2, 0.0f, 1e-5);
    assert_float_equal(qo.q3, 0.0f, 1e-5);

    // Edge case: Identity quaternion multiplication
    quaternion_t q_identity = {1.0f, 0.0f, 0.0f, 0.0f};
    quaternionMult(&qa, &q_identity, &qo);
    assert_float_equal(qo.q0, qa.q0, 1e-5);
    assert_float_equal(qo.q1, qa.q1, 1e-5);
    assert_float_equal(qo.q2, qa.q2, 1e-5);
    assert_float_equal(qo.q3, qa.q3, 1e-5);

    // Edge case: Zero quaternion multiplication
    quaternion_t q_zero = {0.0f, 0.0f, 0.0f, 0.0f};
    quaternionMult(&qa, &q_zero, &qo);
    assert_float_equal(qo.q0, 0.0f, 1e-5);
    assert_float_equal(qo.q1, 0.0f, 1e-5);
    assert_float_equal(qo.q2, 0.0f, 1e-5);
    assert_float_equal(qo.q3, 0.0f, 1e-5);
}

static void test_quaternionRotation(void** state) {
    (void)state; /* unused */

    quaternion_t qr = {1.0f, 0.0f, 0.0f, 0.0f};
    quaternion_t qv = {0.0f, 1.0f, 0.0f, 0.0f};
    quaternion_t qo;
    quaternionRotation(&qr, &qv, &qo);

    assert_float_equal(qo.q0, 0.0f, 1e-5);
    assert_float_equal(qo.q1, 1.0f, 1e-5);
    assert_float_equal(qo.q2, 0.0f, 1e-5);
    assert_float_equal(qo.q3, 0.0f, 1e-5);

    // Edge case: Rotation by 180 degrees
    quaternion_t qr_180 = {0.0f, 1.0f, 0.0f, 0.0f}; // 180 degree rotation around the x-axis
    quaternion_t qv2 = {0.0f, 0.0f, 1.0f, 0.0f};
    quaternionRotation(&qr_180, &qv2, &qo);
    assert_float_equal(qo.q1, 0.0f, 1e-5);
    assert_float_equal(qo.q2, -1.0f, 1e-5);
    assert_float_equal(qo.q3, 0.0f, 1e-5);
}

static void test_quaternionConj(void** state) {
    (void)state; /* unused */

    quaternion_t qa = {1.0f, 2.0f, 3.0f, 4.0f};
    quaternion_t qo;
    quaternionConj(&qa, &qo);

    assert_float_equal(qo.q0, 1.0f, 1e-5);
    assert_float_equal(qo.q1, -2.0f, 1e-5);
    assert_float_equal(qo.q2, -3.0f, 1e-5);
    assert_float_equal(qo.q3, -4.0f, 1e-5);

    // Edge case: Zero quaternion conjugate
    quaternion_t q_zero = {0.0f, 0.0f, 0.0f, 0.0f};
    quaternionConj(&q_zero, &qo);
    assert_float_equal(qo.q0, 0.0f, 1e-5);
    assert_float_equal(qo.q1, 0.0f, 1e-5);
    assert_float_equal(qo.q2, 0.0f, 1e-5);
    assert_float_equal(qo.q3, 0.0f, 1e-5);
}

static void test_quaternionToEuler(void** state) {
    (void)state; /* unused */

    quaternion_t qr = {1.0f, 0.0f, 0.0f, 0.0f};
    axis3f_t ea;
    quaternionToEuler(&qr, &ea);

    assert_float_equal(ea.x, 0.0f, 1e-5);
    assert_float_equal(ea.y, 0.0f, 1e-5);
    assert_float_equal(ea.z, 0.0f, 1e-5);

    // Edge case: 90 degree rotation around the z-axis
    quaternion_t qr_90_z = {0.7071068f, 0.0f, 0.0f, 0.7071068f}; // 90 degrees around z-axis
    quaternionToEuler(&qr_90_z, &ea);
    assert_float_equal(ea.x, 0.0f, 1e-5);
    assert_float_equal(ea.y, 0.0f, 1e-5);
    assert_float_equal(ea.z, ADVUTILS_constPI / 2.f, 1e-5);

    // Edge case: 92 degree rotation around the x-axis. Angles should remain unchanged
    quaternion_t qr_92_x = {0.6946f, 0.7193f, 0.0f, 0.0f}; // 92 degrees around x-axis
    axis3f_t ea_pre;
    ea_pre.x = ea.x;
    ea_pre.y = ea.y;
    ea_pre.z = ea.z;
    quaternionToEuler(&qr_92_x, &ea);
    assert_float_equal(ea.x, ea_pre.x, 1e-5);
    assert_float_equal(ea.y, 0.0f, 1e-5);
    assert_float_equal(ea.z, 0.0f, 1e-5);
}

int main(void) {
    const struct CMUnitTest test_quaternion[] = {
        cmocka_unit_test(test_quaternionNorm), cmocka_unit_test(test_quaternionMult),    cmocka_unit_test(test_quaternionRotation),
        cmocka_unit_test(test_quaternionConj), cmocka_unit_test(test_quaternionToEuler),
    };

    return cmocka_run_group_tests(test_quaternion, NULL, NULL);
}
