/* BEGIN Header */
/**
 ******************************************************************************
 * \file            test_matrix.c
 * \author          Andrea Vivani
 * \brief           Unit tests for matrix.c
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
#include "matrix.h"

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

static void test_matrixInit(void** state) {
    (void)state; /* unused */
    matrix_t matrix;
    assert_int_equal(matrixInit(&matrix, 3, 3), UTILS_STATUS_SUCCESS);
    assert_int_equal(matrix.rows, 3);
    assert_int_equal(matrix.cols, 3);
    assert_non_null(matrix.data);
    matrixDelete(&matrix);
    /* Check null initialization */
    skipAssert = 0;
    expect_assert_failure(matrixInit(&matrix, 0, 0));
    skipAssert = 1;
    assert_int_equal(matrixInit(&matrix, 0, 0), UTILS_STATUS_ERROR);
    skipAssert = 0;
}

static void test_matrixDelete(void** state) {
    (void)state; /* unused */
    matrix_t matrix;
    assert_int_equal(matrixInit(&matrix, 3, 3), UTILS_STATUS_SUCCESS);
    assert_int_equal(matrixDelete(&matrix), UTILS_STATUS_SUCCESS);
    /* Check null deletion */
    matrix.data = NULL;
    assert_int_equal(matrixDelete(&matrix), UTILS_STATUS_ERROR);
}

static void test_matrixInitStatic(void** state) {
    (void)state; /* unused */
    matrix_t matrix;
    float buffer[9];
    matrixInitStatic(&matrix, buffer, 3, 3);
    assert_int_equal(matrix.rows, 3);
    assert_int_equal(matrix.cols, 3);
    assert_ptr_equal(matrix.data, buffer);
}

static void test_matrixIdentity(void** state) {
    (void)state; /* unused */
    matrix_t matrix;
    float buffer[9];
    matrixInitStatic(&matrix, buffer, 3, 3);
    matrixIdentity(&matrix);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == j) {
                assert_float_equal(matrixGet(&matrix, i, j), 1.0f, 1e-5);
            } else {
                assert_float_equal(matrixGet(&matrix, i, j), 0.0f, 1e-5);
            }
        }
    }
}

static void test_matrixZeros(void** state) {
    (void)state; /* unused */
    matrix_t matrix;
    float buffer[9];
    matrixInitStatic(&matrix, buffer, 3, 3);
    matrixZeros(&matrix);
    for (int i = 0; i < 9; i++) {
        assert_float_equal(matrix.data[i], 0.0f, 1e-5);
    }
}

static void test_matrixCopy(void** state) {
    (void)state; /* unused */
    matrix_t input, output;
    float input_data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    float output_data[9];
    matrixInitStatic(&input, input_data, 3, 3);
    matrixInitStatic(&output, output_data, 3, 3);
    matrixCopy(&input, &output);
    for (int i = 0; i < 9; i++) {
        assert_float_equal(output.data[i], input.data[i], 1e-5);
    }
}

static void test_matrixAdd(void** state) {
    (void)state; /* unused */
    matrix_t lhs, rhs, result;
    float lhs_data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    float rhs_data[9] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    float result_data[9];
    matrixInitStatic(&lhs, lhs_data, 3, 3);
    matrixInitStatic(&rhs, rhs_data, 3, 3);
    matrixInitStatic(&result, result_data, 3, 3);
    matrixAdd(&lhs, &rhs, &result);
    for (int i = 0; i < 9; i++) {
        assert_float_equal(result.data[i], 10.0f, 1e-5);
    }
}

static void test_matrixAddScalar(void** state) {
    (void)state; /* unused */
    matrix_t lhs, result;
    float lhs_data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    float result_data[9];
    matrixInitStatic(&lhs, lhs_data, 3, 3);
    matrixInitStatic(&result, result_data, 3, 3);
    matrixAddScalar(&lhs, 1.0f, &result);
    for (int i = 0; i < 9; i++) {
        assert_float_equal(result.data[i], lhs_data[i] + 1.0f, 1e-5);
    }
}

static void test_matrixSub(void** state) {
    (void)state; /* unused */
    matrix_t lhs, rhs, result;
    float lhs_data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    float rhs_data[9] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    float result_data[9];
    matrixInitStatic(&lhs, lhs_data, 3, 3);
    matrixInitStatic(&rhs, rhs_data, 3, 3);
    matrixInitStatic(&result, result_data, 3, 3);
    matrixSub(&lhs, &rhs, &result);
    for (int i = 0; i < 9; i++) {
        assert_float_equal(result.data[i], lhs_data[i] - rhs_data[i], 1e-5);
    }
}

static void test_matrixMult(void** state) {
    (void)state; /* unused */
    matrix_t lhs, rhs, result;
    float lhs_data[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    float rhs_data[16] = {17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
    float result_data[16];
    matrixInitStatic(&lhs, lhs_data, 4, 4);
    matrixInitStatic(&rhs, rhs_data, 4, 4);
    matrixInitStatic(&result, result_data, 4, 4);
    matrixMult(&lhs, &rhs, &result);
    assert_float_equal(result.data[0], 250.0f, 1e-5);
    assert_float_equal(result.data[1], 260.0f, 1e-5);
    assert_float_equal(result.data[2], 270.0f, 1e-5);
    assert_float_equal(result.data[3], 280.0f, 1e-5);
    assert_float_equal(result.data[4], 618.0f, 1e-5);
    assert_float_equal(result.data[5], 644.0f, 1e-5);
    assert_float_equal(result.data[6], 670.0f, 1e-5);
    assert_float_equal(result.data[7], 696.0f, 1e-5);
    assert_float_equal(result.data[8], 986.0f, 1e-5);
    assert_float_equal(result.data[9], 1028.0f, 1e-5);
    assert_float_equal(result.data[10], 1070.0f, 1e-5);
    assert_float_equal(result.data[11], 1112.0f, 1e-5);
    assert_float_equal(result.data[12], 1354.0f, 1e-5);
    assert_float_equal(result.data[13], 1412.0f, 1e-5);
    assert_float_equal(result.data[14], 1470.0f, 1e-5);
    assert_float_equal(result.data[15], 1528.0f, 1e-5);
}

static void test_matrixMult_lhsT(void** state) {
    (void)state; /* unused */
    matrix_t lhs, rhs, result;
    float lhs_data[6] = {1, 2, 3, 4, 5, 6};
    float rhs_data[6] = {7, 8, 9, 10, 11, 12};
    float result_data[4];
    matrixInitStatic(&lhs, lhs_data, 3, 2);
    matrixInitStatic(&rhs, rhs_data, 3, 2);
    matrixInitStatic(&result, result_data, 2, 2);
    matrixMult_lhsT(&lhs, &rhs, &result);
    assert_float_equal(result.data[0], 89.0f, 1e-5);
    assert_float_equal(result.data[1], 98.0f, 1e-5);
    assert_float_equal(result.data[2], 116.0f, 1e-5);
    assert_float_equal(result.data[3], 128.0f, 1e-5);
}

static void test_matrixMult_rhsT(void** state) {
    (void)state; /* unused */
    matrix_t lhs, rhs, result;
    float lhs_data[6] = {1, 2, 3, 4, 5, 6};
    float rhs_data[6] = {7, 8, 9, 10, 11, 12};
    float result_data[4];
    matrixInitStatic(&lhs, lhs_data, 2, 3);
    matrixInitStatic(&rhs, rhs_data, 2, 3);
    matrixInitStatic(&result, result_data, 2, 2);
    matrixMult_rhsT(&lhs, &rhs, &result);
    assert_float_equal(result.data[0], 50.0f, 1e-5);
    assert_float_equal(result.data[1], 68.0f, 1e-5);
    assert_float_equal(result.data[2], 122.0f, 1e-5);
    assert_float_equal(result.data[3], 167.0f, 1e-5);
}

static void test_matrixMultScalar(void** state) {
    (void)state; /* unused */
    matrix_t matrix, result;
    float matrix_data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    float result_data[9];
    matrixInitStatic(&matrix, matrix_data, 3, 3);
    matrixInitStatic(&result, result_data, 3, 3);
    matrixMultScalar(&matrix, 2.0f, &result);
    for (int i = 0; i < 9; i++) {
        assert_float_equal(result.data[i], matrix_data[i] * 2.0f, 1e-5);
    }
}

static void test_matrixTrans(void** state) {
    (void)state; /* unused */
    matrix_t matrix, result;
    float matrix_data[6] = {1, 2, 3, 4, 5, 6};
    float result_data[6];
    matrixInitStatic(&matrix, matrix_data, 2, 3);
    matrixInitStatic(&result, result_data, 3, 2);
    matrixTrans(&matrix, &result);
    assert_float_equal(result.data[0], 1.0f, 1e-5);
    assert_float_equal(result.data[1], 4.0f, 1e-5);
    assert_float_equal(result.data[2], 2.0f, 1e-5);
    assert_float_equal(result.data[3], 5.0f, 1e-5);
    assert_float_equal(result.data[4], 3.0f, 1e-5);
    assert_float_equal(result.data[5], 6.0f, 1e-5);
}

static void test_matrixSymmetric(void** state) {
    (void)state; /* unused */
    matrix_t lhs, result;
    float lhs_data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    float result_data[9];
    matrixInitStatic(&lhs, lhs_data, 3, 3);
    matrixInitStatic(&result, result_data, 3, 3);
    matrixSymmetric(&lhs, &result);
    /* result = (lhs + lhs^T) / 2 */
    assert_float_equal(result.data[0], 1.0f, 1e-5);
    assert_float_equal(result.data[1], 3.0f, 1e-5);
    assert_float_equal(result.data[2], 5.0f, 1e-5);
    assert_float_equal(result.data[3], 3.0f, 1e-5);
    assert_float_equal(result.data[4], 5.0f, 1e-5);
    assert_float_equal(result.data[5], 7.0f, 1e-5);
    assert_float_equal(result.data[6], 5.0f, 1e-5);
    assert_float_equal(result.data[7], 7.0f, 1e-5);
    assert_float_equal(result.data[8], 9.0f, 1e-5);
    /* const input must be left untouched */
    assert_float_equal(lhs.data[1], 2.0f, 1e-5);
    assert_float_equal(lhs.data[5], 6.0f, 1e-5);
    assert_float_equal(lhs.data[6], 7.0f, 1e-5);
}

static void test_matrixSymmetric_inPlace(void** state) {
    (void)state; /* unused */
    matrix_t m;
    float m_data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    matrixInitStatic(&m, m_data, 3, 3);
    matrixSymmetric(&m, &m); /* lhs and result alias the same buffer */
    assert_float_equal(m.data[0], 1.0f, 1e-5);
    assert_float_equal(m.data[1], 3.0f, 1e-5);
    assert_float_equal(m.data[2], 5.0f, 1e-5);
    assert_float_equal(m.data[3], 3.0f, 1e-5);
    assert_float_equal(m.data[4], 5.0f, 1e-5);
    assert_float_equal(m.data[5], 7.0f, 1e-5);
    assert_float_equal(m.data[6], 5.0f, 1e-5);
    assert_float_equal(m.data[7], 7.0f, 1e-5);
    assert_float_equal(m.data[8], 9.0f, 1e-5);
}

static void test_matrixNorm(void** state) {
    (void)state; /* unused */
    matrix_t matrix;
    float matrix_data[4] = {1, 2, 3, 4};
    matrixInitStatic(&matrix, matrix_data, 2, 2);
    float norm = matrixNorm(&matrix);
    assert_float_equal(norm, sqrtf(30.0f), 1e-5);
}

static void test_matrixNormalized(void** state) {
    (void)state; /* unused */
    matrix_t matrix, result;
    float matrix_data[4] = {1, 2, 3, 4};
    float result_data[4];
    matrixInitStatic(&matrix, matrix_data, 2, 2);
    matrixInitStatic(&result, result_data, 2, 2);
    matrixNormalized(&matrix, &result);
    assert_float_equal(result_data[0], 1 / sqrtf(30.0f), 1e-5);
    assert_float_equal(result_data[1], 2 / sqrtf(30.0f), 1e-5);
    assert_float_equal(result_data[2], 3 / sqrtf(30.0f), 1e-5);
    assert_float_equal(result_data[3], 4 / sqrtf(30.0f), 1e-5);
}

static void test_matrixSetAndGet(void** state) {
    (void)state; /* unused */
    matrix_t matrix;
    float buffer[4];
    matrixInitStatic(&matrix, buffer, 2, 2);
    matrixSet(&matrix, 0, 0, 1.0f);
    matrixSet(&matrix, 0, 1, 2.0f);
    matrixSet(&matrix, 1, 0, 3.0f);
    matrixSet(&matrix, 1, 1, 4.0f);
    assert_float_equal(matrixGet(&matrix, 0, 0), 1.0f, 1e-5);
    assert_float_equal(matrixGet(&matrix, 0, 1), 2.0f, 1e-5);
    assert_float_equal(matrixGet(&matrix, 1, 0), 3.0f, 1e-5);
    assert_float_equal(matrixGet(&matrix, 1, 1), 4.0f, 1e-5);
}

static void test_matrixDet(void** state) {
    (void)state; /* unused */
    matrix_t matrix;
    assert_int_equal(matrixInit(&matrix, 4, 4), UTILS_STATUS_SUCCESS);
    float matrix_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456, 0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    memcpy(matrix.data, matrix_data, 16 * sizeof(float));
    assert_float_equal(matrixDet(&matrix), -0.059641f, 1e-5);
    matrixDelete(&matrix);
    /* Check ill-conditioned matrix */
    matrix_t matrix2;
    float matrix2_data[] = {0, 7, 6, 2, 9, 5, 1, 3, 4, 3, 8, 4, 5, 6, 7, 8};
    matrixInit(&matrix2, 4, 4);
    memcpy(matrix2.data, matrix2_data, 16 * sizeof(float));
    assert_float_equal(matrixDet(&matrix2), -1782.0f, 1e-3);
    matrixDelete(&matrix2);
    /* Check rows != columns */
    matrixInit(&matrix, 4, 3);
    assert_float_equal(matrixDet(&matrix), 0, 1e-5);
    matrixDelete(&matrix);
}

static void test_matrixInversed(void** state) {
    (void)state; /* unused */
    matrix_t matrix, result;
    assert_int_equal(matrixInit(&matrix, 4, 4), UTILS_STATUS_SUCCESS);
    assert_int_equal(matrixInit(&result, 4, 4), UTILS_STATUS_SUCCESS);
    float matrix_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456, 0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    memcpy(matrix.data, matrix_data, 16 * sizeof(float));
    assert_int_equal(matrixInversed(&matrix, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -2.467338, 1e-5);
    assert_float_equal(result.data[1], -1.266793, 1e-5);
    assert_float_equal(result.data[2], 1.985099, 1e-5);
    assert_float_equal(result.data[3], 1.099676, 1e-5);
    assert_float_equal(result.data[4], -1.881585, 1e-5);
    assert_float_equal(result.data[5], 0.621034, 1e-5);
    assert_float_equal(result.data[6], 0.065793, 1e-5);
    assert_float_equal(result.data[7], 1.351763, 1e-5);
    assert_float_equal(result.data[8], -5.466175, 1e-5);
    assert_float_equal(result.data[9], 1.072583, 1e-5);
    assert_float_equal(result.data[10], 2.766578, 1e-5);
    assert_float_equal(result.data[11], 0.923137, 1e-5);
    assert_float_equal(result.data[12], 10.254800, 1e-5);
    assert_float_equal(result.data[13], 0.167199, 1e-5);
    assert_float_equal(result.data[14], -4.399540, 1e-5);
    assert_float_equal(result.data[15], -2.813918, 1e-5);
    matrixDelete(&matrix);
    matrixDelete(&result);
    /* singular matrix -> ERROR */
    float matrix2_data[9] = {1, 2, 3, 2, 4, 6, 7, 8, 10};
    matrixInit(&matrix, 3, 3);
    matrixInit(&result, 3, 3);
    memcpy(matrix.data, matrix2_data, 9 * sizeof(float));
    assert_int_equal(matrixInversed(&matrix, &result), UTILS_STATUS_ERROR);
    matrixDelete(&matrix);
    matrixDelete(&result);
}

static void test_matrixInversed_rob(void** state) {
    (void)state; /* unused */
    matrix_t matrix, result;
    assert_int_equal(matrixInit(&matrix, 4, 4), UTILS_STATUS_SUCCESS);
    assert_int_equal(matrixInit(&result, 4, 4), UTILS_STATUS_SUCCESS);
    float matrix_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456, 0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    memcpy(matrix.data, matrix_data, 16 * sizeof(float));
    assert_int_equal(matrixInversed_rob(&matrix, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -2.467338, 1e-5);
    assert_float_equal(result.data[1], -1.266793, 1e-5);
    assert_float_equal(result.data[2], 1.985099, 1e-5);
    assert_float_equal(result.data[3], 1.099676, 1e-5);
    assert_float_equal(result.data[4], -1.881585, 1e-5);
    assert_float_equal(result.data[5], 0.621034, 1e-5);
    assert_float_equal(result.data[6], 0.065793, 1e-5);
    assert_float_equal(result.data[7], 1.351763, 1e-5);
    assert_float_equal(result.data[8], -5.466175, 1e-5);
    assert_float_equal(result.data[9], 1.072583, 1e-5);
    assert_float_equal(result.data[10], 2.766578, 1e-5);
    assert_float_equal(result.data[11], 0.923137, 1e-5);
    assert_float_equal(result.data[12], 10.254800, 1e-5);
    assert_float_equal(result.data[13], 0.167199, 1e-5);
    assert_float_equal(result.data[14], -4.399540, 1e-5);
    assert_float_equal(result.data[15], -2.813918, 1e-5);
    matrixDelete(&matrix);
    matrixDelete(&result);
    /* singular matrix -> ERROR */
    float matrix2_data[9] = {1, 2, 3, 2, 4, 6, 7, 8, 10};
    matrixInit(&matrix, 3, 3);
    matrixInit(&result, 3, 3);
    memcpy(matrix.data, matrix2_data, 9 * sizeof(float));
    assert_int_equal(matrixInversed_rob(&matrix, &result), UTILS_STATUS_ERROR);
    matrixDelete(&matrix);
    matrixDelete(&result);
}

static void test_matrixInversed_SPD(void** state) {
    (void)state; /* unused */
    matrix_t matrix, result;
    assert_int_equal(matrixInit(&matrix, 4, 4), UTILS_STATUS_SUCCESS);
    assert_int_equal(matrixInit(&result, 4, 4), UTILS_STATUS_SUCCESS);
    float matrix_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456, 0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    memcpy(matrix.data, matrix_data, 16 * sizeof(float));
    assert_int_equal(matrixInversed_SPD(&matrix, &result), UTILS_STATUS_ERROR);
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < 4; j++) {
            assert_float_equal(result.data[i * 4 + j], (i == j) ? 1.0f : 0.0f, 1e-5); // Check graceful fail in case matrix is not SPD
        }
    }
    float matrix_data2[] = {2.5431, 0.8124, 0.6213, 0.4172, 0.8124, 2.2698, 0.5341, 0.7156, 0.6213, 0.5341, 2.8017, 0.3288, 0.4172, 0.7156, 0.3288, 2.0459};
    memcpy(matrix.data, matrix_data2, 16 * sizeof(float));
    assert_int_equal(matrixInversed_SPD(&matrix, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], 0.460176, 1e-5);
    assert_float_equal(result.data[1], -0.136945, 1e-5);
    assert_float_equal(result.data[2], -0.071906, 1e-5);
    assert_float_equal(result.data[3], -0.034383, 1e-5);
    assert_float_equal(result.data[4], -0.136945, 1e-5);
    assert_float_equal(result.data[5], 0.552104, 1e-5);
    assert_float_equal(result.data[6], -0.056562, 1e-5);
    assert_float_equal(result.data[7], -0.156095, 1e-5);
    assert_float_equal(result.data[8], -0.071906, 1e-5);
    assert_float_equal(result.data[9], -0.056562, 1e-5);
    assert_float_equal(result.data[10], 0.386909, 1e-5);
    assert_float_equal(result.data[11], -0.027734, 1e-5);
    assert_float_equal(result.data[12], -0.034383, 1e-5);
    assert_float_equal(result.data[13], -0.156095, 1e-5);
    assert_float_equal(result.data[14], -0.027734, 1e-5);
    assert_float_equal(result.data[15], 0.554849, 1e-5);
    matrixDelete(&matrix);
    matrixDelete(&result);
}

static void test_matrixPseudoInv(void** state) {
    (void)state; /* unused */
    matrix_t matrix, result;
    assert_int_equal(matrixInit(&matrix, 2, 2), UTILS_STATUS_SUCCESS);
    assert_int_equal(matrixInit(&result, 2, 2), UTILS_STATUS_SUCCESS);
    matrix.data[0] = 1.0f;
    matrix.data[1] = 2.0f;
    matrix.data[2] = 3.0f;
    matrix.data[3] = 4.0f;
    assert_int_equal(matrixPseudoInv(&matrix, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -2.0f, 1e-5);
    assert_float_equal(result.data[1], 1.0f, 1e-5);
    assert_float_equal(result.data[2], 1.5f, 1e-5);
    assert_float_equal(result.data[3], -0.5f, 1e-5);
    matrixDelete(&matrix);
    matrixDelete(&result);
    /* non-finite input -> ERROR */
    float matrix2_data[9] = {INFINITY, 0, 0, 0, 1, 0, 0, 0, 1};
    matrixInit(&matrix, 3, 3);
    matrixInit(&result, 3, 3);
    memcpy(matrix.data, matrix2_data, 9 * sizeof(float));
    assert_int_equal(matrixPseudoInv(&matrix, &result), UTILS_STATUS_ERROR);
    matrixDelete(&matrix);
    matrixDelete(&result);
}

static void test_matrixDetStatic(void** state) {
    (void)state; /* unused */
    matrix_t matrix;
    float matrix_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456, 0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    matrixInitStatic(&matrix, matrix_data, 4, 4);
    assert_float_equal(matrixDetStatic(&matrix), -0.059641f, 1e-5);
    /* Check ill-conditioned matrix */
    matrix_t matrix2;
    float matrix2_data[] = {0, 7, 6, 2, 9, 5, 1, 3, 4, 3, 8, 4, 5, 6, 7, 8};
    matrixInitStatic(&matrix2, matrix2_data, 4, 4);
    assert_float_equal(matrixDetStatic(&matrix2), -1782.0f, 1e-3);
    /* Check rows != columns */
    matrixInitStatic(&matrix2, matrix2_data, 1, 4);
    assert_float_equal(matrixDetStatic(&matrix2), 0, 1e-5);
}

static void test_matrixInversedStatic(void** state) {
    (void)state; /* unused */
    matrix_t matrix, result;
    float matrix_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456, 0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    float result_data[16];
    matrixInitStatic(&matrix, matrix_data, 4, 4);
    matrixInitStatic(&result, result_data, 4, 4);
    assert_int_equal(matrixInversedStatic(&matrix, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -2.467338, 1e-5);
    assert_float_equal(result.data[1], -1.266793, 1e-5);
    assert_float_equal(result.data[2], 1.985099, 1e-5);
    assert_float_equal(result.data[3], 1.099676, 1e-5);
    assert_float_equal(result.data[4], -1.881585, 1e-5);
    assert_float_equal(result.data[5], 0.621034, 1e-5);
    assert_float_equal(result.data[6], 0.065793, 1e-5);
    assert_float_equal(result.data[7], 1.351763, 1e-5);
    assert_float_equal(result.data[8], -5.466175, 1e-5);
    assert_float_equal(result.data[9], 1.072583, 1e-5);
    assert_float_equal(result.data[10], 2.766578, 1e-5);
    assert_float_equal(result.data[11], 0.923137, 1e-5);
    assert_float_equal(result.data[12], 10.254800, 1e-5);
    assert_float_equal(result.data[13], 0.167199, 1e-5);
    assert_float_equal(result.data[14], -4.399540, 1e-5);
    assert_float_equal(result.data[15], -2.813918, 1e-5);
    /* singular matrix -> ERROR */
    float matrix2_data[9] = {1, 2, 3, 2, 4, 6, 7, 8, 10};
    matrixInitStatic(&matrix, matrix2_data, 3, 3);
    matrixInitStatic(&result, result_data, 3, 3);
    assert_int_equal(matrixInversedStatic(&matrix, &result), UTILS_STATUS_ERROR);
}

static void test_matrixInversedStatic_rob(void** state) {
    (void)state; /* unused */
    matrix_t matrix, result;
    float matrix_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456, 0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    float result_data[16];
    matrixInitStatic(&matrix, matrix_data, 4, 4);
    matrixInitStatic(&result, result_data, 4, 4);
    assert_int_equal(matrixInversedStatic_rob(&matrix, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -2.467338, 1e-5);
    assert_float_equal(result.data[1], -1.266793, 1e-5);
    assert_float_equal(result.data[2], 1.985099, 1e-5);
    assert_float_equal(result.data[3], 1.099676, 1e-5);
    assert_float_equal(result.data[4], -1.881585, 1e-5);
    assert_float_equal(result.data[5], 0.621034, 1e-5);
    assert_float_equal(result.data[6], 0.065793, 1e-5);
    assert_float_equal(result.data[7], 1.351763, 1e-5);
    assert_float_equal(result.data[8], -5.466175, 1e-5);
    assert_float_equal(result.data[9], 1.072583, 1e-5);
    assert_float_equal(result.data[10], 2.766578, 1e-5);
    assert_float_equal(result.data[11], 0.923137, 1e-5);
    assert_float_equal(result.data[12], 10.254800, 1e-5);
    assert_float_equal(result.data[13], 0.167199, 1e-5);
    assert_float_equal(result.data[14], -4.399540, 1e-5);
    assert_float_equal(result.data[15], -2.813918, 1e-5);
    /* singular matrix -> ERROR */
    float matrix2_data[9] = {1, 2, 3, 2, 4, 6, 7, 8, 10};
    matrixInitStatic(&matrix, matrix2_data, 3, 3);
    matrixInitStatic(&result, result_data, 3, 3);
    assert_int_equal(matrixInversedStatic_rob(&matrix, &result), UTILS_STATUS_ERROR);
}

static void test_matrixInversedStatic_SPD(void** state) {
    (void)state; /* unused */
    matrix_t matrix, result;
    float matrix_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456, 0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    float result_data[16];
    matrixInitStatic(&matrix, matrix_data, 4, 4);
    matrixInitStatic(&result, result_data, 4, 4);
    assert_int_equal(matrixInversedStatic_SPD(&matrix, &result), UTILS_STATUS_ERROR);
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < 4; j++) {
            assert_float_equal(result.data[i * 4 + j], (i == j) ? 1.0f : 0.0f, 1e-5); // Check graceful fail in case matrix is not SPD
        }
    }
    float matrix_data2[] = {2.5431, 0.8124, 0.6213, 0.4172, 0.8124, 2.2698, 0.5341, 0.7156, 0.6213, 0.5341, 2.8017, 0.3288, 0.4172, 0.7156, 0.3288, 2.0459};
    memcpy(matrix.data, matrix_data2, 16 * sizeof(float));
    assert_int_equal(matrixInversedStatic_SPD(&matrix, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], 0.460176, 1e-5);
    assert_float_equal(result.data[1], -0.136945, 1e-5);
    assert_float_equal(result.data[2], -0.071906, 1e-5);
    assert_float_equal(result.data[3], -0.034383, 1e-5);
    assert_float_equal(result.data[4], -0.136945, 1e-5);
    assert_float_equal(result.data[5], 0.552104, 1e-5);
    assert_float_equal(result.data[6], -0.056562, 1e-5);
    assert_float_equal(result.data[7], -0.156095, 1e-5);
    assert_float_equal(result.data[8], -0.071906, 1e-5);
    assert_float_equal(result.data[9], -0.056562, 1e-5);
    assert_float_equal(result.data[10], 0.386909, 1e-5);
    assert_float_equal(result.data[11], -0.027734, 1e-5);
    assert_float_equal(result.data[12], -0.034383, 1e-5);
    assert_float_equal(result.data[13], -0.156095, 1e-5);
    assert_float_equal(result.data[14], -0.027734, 1e-5);
    assert_float_equal(result.data[15], 0.554849, 1e-5);
}

static void test_matrixPseudoInvStatic(void** state) {
    (void)state; /* unused */
    matrix_t matrix, result;
    float matrix_data[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float result_data[4];
    matrixInitStatic(&matrix, matrix_data, 2, 2);
    matrixInitStatic(&result, result_data, 2, 2);
    assert_int_equal(matrixPseudoInvStatic(&matrix, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -2.0f, 1e-5);
    assert_float_equal(result.data[1], 1.0f, 1e-5);
    assert_float_equal(result.data[2], 1.5f, 1e-5);
    assert_float_equal(result.data[3], -0.5f, 1e-5);
    /* non-finite input -> ERROR */
    float matrix2_data[9] = {INFINITY, 0, 0, 0, 1, 0, 0, 0, 1};
    matrixInitStatic(&matrix, matrix2_data, 3, 3);
    matrixInitStatic(&result, result_data, 3, 3);
    assert_int_equal(matrixPseudoInvStatic(&matrix, &result), UTILS_STATUS_ERROR);
}

int main(void) {
    const struct CMUnitTest test_matrix[] = {
        cmocka_unit_test(test_matrixInit),
        cmocka_unit_test(test_matrixDelete),
        cmocka_unit_test(test_matrixDet),
        cmocka_unit_test(test_matrixInversed),
        cmocka_unit_test(test_matrixInversed_rob),
        cmocka_unit_test(test_matrixInversed_SPD),
        cmocka_unit_test(test_matrixPseudoInv),
        cmocka_unit_test(test_matrixInitStatic),
        cmocka_unit_test(test_matrixDetStatic),
        cmocka_unit_test(test_matrixInversedStatic),
        cmocka_unit_test(test_matrixInversedStatic_rob),
        cmocka_unit_test(test_matrixInversedStatic_SPD),
        cmocka_unit_test(test_matrixPseudoInvStatic),
        cmocka_unit_test(test_matrixIdentity),
        cmocka_unit_test(test_matrixZeros),
        cmocka_unit_test(test_matrixCopy),
        cmocka_unit_test(test_matrixAdd),
        cmocka_unit_test(test_matrixAddScalar),
        cmocka_unit_test(test_matrixSub),
        cmocka_unit_test(test_matrixMult),
        cmocka_unit_test(test_matrixMult_lhsT),
        cmocka_unit_test(test_matrixMult_rhsT),
        cmocka_unit_test(test_matrixMultScalar),
        cmocka_unit_test(test_matrixTrans),
        cmocka_unit_test(test_matrixSymmetric),
        cmocka_unit_test(test_matrixSymmetric_inPlace),
        cmocka_unit_test(test_matrixNorm),
        cmocka_unit_test(test_matrixNormalized),
        cmocka_unit_test(test_matrixSetAndGet),
    };

    return cmocka_run_group_tests(test_matrix, NULL, NULL);
}
