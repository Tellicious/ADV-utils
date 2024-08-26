/* BEGIN Header */
/**
 ******************************************************************************
 * \file            test_numMethods.c
 * \author          Andrea Vivani
 * \brief           Unit tests for numMethods.c
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
#include "numMethods.h"
#include "stdio.h"

#include <cmocka.h>

/* Functions -----------------------------------------------------------------*/

static void test_fwsub(void** state) {
    (void)state; /* unused */
    matrix_t A, B, result;
    float A_data[4] = {1, 0, 1, 1};
    float B_data[2] = {1, 2};
    float result_data[2];
    matrixInitStatic(&A, A_data, 2, 2);
    matrixInitStatic(&B, B_data, 2, 1);
    matrixInitStatic(&result, result_data, 2, 1);
    fwsub(&A, &B, &result);
    assert_float_equal(result.data[0], 1.0f, 1e-5);
    assert_float_equal(result.data[1], 1.0f, 1e-5);
}

static void test_fwsubPerm(void** state) {
    (void)state; /* unused */
    matrix_t A, B, P, result;
    float A_data[4] = {1, 0, 1, 1};
    float B_data[2] = {1, 2};
    float P_data[4] = {1, 0};
    float result_data[2];
    matrixInitStatic(&A, A_data, 2, 2);
    matrixInitStatic(&B, B_data, 2, 1);
    matrixInitStatic(&P, P_data, 2, 1);
    matrixInitStatic(&result, result_data, 2, 1);
    fwsubPerm(&A, &B, &P, &result);
    assert_float_equal(result.data[0], 2.0f, 1e-5);
    assert_float_equal(result.data[1], -1.0f, 1e-5);
}

static void test_bksub(void** state) {
    (void)state; /* unused */
    matrix_t A, B, result;
    float A_data[4] = {1, 1, 0, 1};
    float B_data[2] = {2, 1};
    float result_data[2];
    matrixInitStatic(&A, A_data, 2, 2);
    matrixInitStatic(&B, B_data, 2, 1);
    matrixInitStatic(&result, result_data, 2, 1);
    bksub(&A, &B, &result);
    assert_float_equal(result.data[0], 1.0f, 1e-5);
    assert_float_equal(result.data[1], 1.0f, 1e-5);
}

static void test_bksubPerm(void** state) {
    (void)state; /* unused */
    matrix_t A, B, P, result;
    float A_data[4] = {1, 1, 0, 1};
    float B_data[2] = {2, 1};
    float P_data[4] = {1, 0};
    float result_data[2];
    matrixInitStatic(&A, A_data, 2, 2);
    matrixInitStatic(&B, B_data, 2, 1);
    matrixInitStatic(&P, P_data, 2, 1);
    matrixInitStatic(&result, result_data, 2, 1);
    bksubPerm(&A, &B, &P, &result);
    assert_float_equal(result.data[0], -1.0f, 1e-5);
    assert_float_equal(result.data[1], 2.0f, 1e-5);
}

static void test_QuadProd(void** state) {
    (void)state; /* unused */
    matrix_t A, B, result;
    float A_data[4] = {1, 2, 3, 4};
    float B_data[4] = {12, 9, 14, 13};
    float result_data[4];
    matrixInitStatic(&A, A_data, 2, 2);
    matrixInitStatic(&B, B_data, 2, 2);
    matrixInitStatic(&result, result_data, 2, 2);
    QuadProd(&A, &B, &result);
    assert_float_equal(result.data[0], 110.0f, 1e-5);
    assert_float_equal(result.data[1], 260.0f, 1e-5);
    assert_float_equal(result.data[2], 250.0f, 1e-5);
    assert_float_equal(result.data[3], 592.0f, 1e-5);
}

static void test_LU_Crout(void** state) {
    (void)state; /* unused */
    matrix_t A, L, U;
    float A_data[4] = {4, 3, 6, 3};
    float L_data[4];
    float U_data[4];
    matrixInitStatic(&A, A_data, 2, 2);
    matrixInitStatic(&L, L_data, 2, 2);
    matrixInitStatic(&U, U_data, 2, 2);
    assert_int_equal(LU_Crout(&A, &L, &U), UTILS_STATUS_SUCCESS);
    assert_float_equal(L.data[0], 4.0f, 1e-5);
    assert_float_equal(L.data[1], 0.0f, 1e-5);
    assert_float_equal(L.data[2], 6.0f, 1e-5);
    assert_float_equal(L.data[3], -1.5f, 1e-5);
    assert_float_equal(U.data[0], 1.0f, 1e-5);
    assert_float_equal(U.data[1], 0.75f, 1e-5);
    assert_float_equal(U.data[2], 0.0f, 1e-5);
    assert_float_equal(U.data[3], 1.0f, 1e-5);
}

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION

static void test_LU_Cormen(void** state) {
    (void)state; /* unused */
    matrix_t A, L, U;
    float A_data[4] = {4, 3, 6, 3};
    matrixInit(&A, 2, 2);
    matrixInit(&L, 2, 2);
    matrixInit(&U, 2, 2);
    memcpy(A.data, A_data, 4 * sizeof(float));
    assert_int_equal(LU_Cormen(&A, &L, &U), UTILS_STATUS_SUCCESS);
    assert_float_equal(L.data[0], 1.0f, 1e-5);
    assert_float_equal(L.data[1], 0.0f, 1e-5);
    assert_float_equal(L.data[2], 1.5f, 1e-5);
    assert_float_equal(L.data[3], 1.0f, 1e-5);
    assert_float_equal(U.data[0], 4.0f, 1e-5);
    assert_float_equal(U.data[1], 3.0f, 1e-5);
    assert_float_equal(U.data[2], 0.0f, 1e-5);
    assert_float_equal(U.data[3], -1.5f, 1e-5);
    matrixDelete(&A);
    matrixDelete(&L);
    matrixDelete(&U);
}

static void test_LUP_Cormen(void** state) {
    (void)state; /* unused */
    matrix_t A, L, U, P;
    float A_data[9] = {2, 0, 2, 1, 1, 1, 2, 1, 3};
    matrixInit(&A, 3, 3);
    matrixInit(&L, 3, 3);
    matrixInit(&U, 3, 3);
    matrixInit(&P, 3, 1);
    memcpy(A.data, A_data, 9 * sizeof(float));
    assert_int_equal(LUP_Cormen(&A, &L, &U, &P), 1);
    assert_float_equal(L.data[0], 1.0f, 1e-5);
    assert_float_equal(L.data[1], 0.0f, 1e-5);
    assert_float_equal(L.data[2], 0.0f, 1e-5);
    assert_float_equal(L.data[3], 0.5f, 1e-5);
    assert_float_equal(L.data[4], 1.0f, 1e-5);
    assert_float_equal(L.data[5], 0.0f, 1e-5);
    assert_float_equal(L.data[6], 1.0f, 1e-5);
    assert_float_equal(L.data[7], 1.0f, 1e-5);
    assert_float_equal(L.data[8], 1.0f, 1e-5);
    assert_float_equal(U.data[0], 2.0f, 1e-5);
    assert_float_equal(U.data[1], 0.0f, 1e-5);
    assert_float_equal(U.data[2], 2.0f, 1e-5);
    assert_float_equal(U.data[3], 0.0f, 1e-5);
    assert_float_equal(U.data[4], 1.0f, 1e-5);
    assert_float_equal(U.data[5], 0.0f, 1e-5);
    assert_float_equal(U.data[6], 0.0f, 1e-5);
    assert_float_equal(U.data[7], 0.0f, 1e-5);
    assert_float_equal(U.data[8], 1.0f, 1e-5);
    matrixDelete(&A);
    matrixDelete(&L);
    matrixDelete(&U);
    matrixDelete(&P);
}

static void test_LinSolveLU(void** state) {
    (void)state; /* unused */
    matrix_t A, B, result;
    float A_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456,
                      0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    float B_data[] = {0.7547, 0.1626, 0.3404, 0.2551, 0.2760, 0.1190, 0.5853, 0.5060,
                      0.6797, 0.4984, 0.2238, 0.6991, 0.6551, 0.9597, 0.7513, 0.8909};
    matrixInit(&A, 4, 4);
    matrixInit(&B, 4, 4);
    matrixInit(&result, 4, 4);
    memcpy(A.data, A_data, 16 * sizeof(float));
    memcpy(B.data, B_data, 16 * sizeof(float));
    LinSolveLU(&A, &B, &result);
    assert_float_equal(result.data[0], -0.142065f, 1e-5);
    assert_float_equal(result.data[1], 1.492795f, 1e-5);
    assert_float_equal(result.data[2], -0.310884f, 1e-5);
    assert_float_equal(result.data[3], 1.097069f, 1e-5);
    assert_float_equal(result.data[4], -0.318367f, 1e-5);
    assert_float_equal(result.data[5], 1.098035f, 1e-5);
    assert_float_equal(result.data[6], 0.753304f, 1e-5);
    assert_float_equal(result.data[7], 1.084532f, 1e-5);
    assert_float_equal(result.data[8], -1.344099f, 1e-5);
    assert_float_equal(result.data[9], 1.503635f, 1e-5);
    assert_float_equal(result.data[10], 0.079810f, 1e-5);
    assert_float_equal(result.data[11], 1.904844f, 1e-5);
    assert_float_equal(result.data[12], 2.951679f, 1e-5);
    assert_float_equal(result.data[13], -3.205921f, 1e-5);
    assert_float_equal(result.data[14], 0.489882f, 1e-5);
    assert_float_equal(result.data[15], -2.882036f, 1e-5);
    matrixDelete(&A);
    matrixDelete(&B);
    matrixDelete(&result);
}

static void test_LinSolveLUP(void** state) {
    (void)state; /* unused */
    matrix_t A, B, result;
    float A_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456,
                      0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    float B_data[] = {0.7547, 0.1626, 0.3404, 0.2551, 0.2760, 0.1190, 0.5853, 0.5060,
                      0.6797, 0.4984, 0.2238, 0.6991, 0.6551, 0.9597, 0.7513, 0.8909};
    matrixInit(&A, 4, 4);
    matrixInit(&B, 4, 4);
    matrixInit(&result, 4, 4);
    memcpy(A.data, A_data, 16 * sizeof(float));
    memcpy(B.data, B_data, 16 * sizeof(float));
    LinSolveLUP(&A, &B, &result);
    assert_float_equal(result.data[0], -0.142065f, 1e-5);
    assert_float_equal(result.data[1], 1.492795f, 1e-5);
    assert_float_equal(result.data[2], -0.310884f, 1e-5);
    assert_float_equal(result.data[3], 1.097069f, 1e-5);
    assert_float_equal(result.data[4], -0.318367f, 1e-5);
    assert_float_equal(result.data[5], 1.098035f, 1e-5);
    assert_float_equal(result.data[6], 0.753304f, 1e-5);
    assert_float_equal(result.data[7], 1.084532f, 1e-5);
    assert_float_equal(result.data[8], -1.344099f, 1e-5);
    assert_float_equal(result.data[9], 1.503635f, 1e-5);
    assert_float_equal(result.data[10], 0.079810f, 1e-5);
    assert_float_equal(result.data[11], 1.904844f, 1e-5);
    assert_float_equal(result.data[12], 2.951679f, 1e-5);
    assert_float_equal(result.data[13], -3.205921f, 1e-5);
    assert_float_equal(result.data[14], 0.489882f, 1e-5);
    assert_float_equal(result.data[15], -2.882036f, 1e-5);
    matrixDelete(&A);
    matrixDelete(&B);
    matrixDelete(&result);
}

static void test_LinSolveGauss(void** state) {
    (void)state; /* unused */
    matrix_t A, B, result;
    float A_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456,
                      0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    float B_data[] = {0.7547, 0.1626, 0.3404, 0.2551, 0.2760, 0.1190, 0.5853, 0.5060,
                      0.6797, 0.4984, 0.2238, 0.6991, 0.6551, 0.9597, 0.7513, 0.8909};
    matrixInit(&A, 4, 4);
    matrixInit(&B, 4, 4);
    matrixInit(&result, 4, 4);
    memcpy(A.data, A_data, 16 * sizeof(float));
    memcpy(B.data, B_data, 16 * sizeof(float));
    LinSolveGauss(&A, &B, &result);
    assert_float_equal(result.data[0], -0.142065f, 1e-5);
    assert_float_equal(result.data[1], 1.492795f, 1e-5);
    assert_float_equal(result.data[2], -0.310884f, 1e-5);
    assert_float_equal(result.data[3], 1.097069f, 1e-5);
    assert_float_equal(result.data[4], -0.318367f, 1e-5);
    assert_float_equal(result.data[5], 1.098035f, 1e-5);
    assert_float_equal(result.data[6], 0.753304f, 1e-5);
    assert_float_equal(result.data[7], 1.084532f, 1e-5);
    assert_float_equal(result.data[8], -1.344099f, 1e-5);
    assert_float_equal(result.data[9], 1.503635f, 1e-5);
    assert_float_equal(result.data[10], 0.079810f, 1e-5);
    assert_float_equal(result.data[11], 1.904844f, 1e-5);
    assert_float_equal(result.data[12], 2.951679f, 1e-5);
    assert_float_equal(result.data[13], -3.205921f, 1e-5);
    assert_float_equal(result.data[14], 0.489882f, 1e-5);
    assert_float_equal(result.data[15], -2.882036f, 1e-5);
    matrixDelete(&A);
    matrixDelete(&B);
    matrixDelete(&result);
}

static void test_DARE(void** state) {
    (void)state; /* unused */
    matrix_t A, B, Q, R, result;
    float A_data[4] = {1, 1, 0, 1};
    float B_data[2] = {0, 1};
    float Q_data[4] = {1, 0, 0, 1};
    float R_data[1] = {1};
    matrixInit(&A, 2, 2);
    matrixInit(&B, 2, 1);
    matrixInit(&Q, 2, 2);
    matrixInit(&R, 1, 1);
    matrixInit(&result, 2, 2);
    memcpy(A.data, A_data, 4 * sizeof(float));
    memcpy(B.data, B_data, 2 * sizeof(float));
    memcpy(Q.data, Q_data, 4 * sizeof(float));
    memcpy(R.data, R_data, 1 * sizeof(float));
    assert_int_equal(DARE(&A, &B, &Q, &R, 2, 1e-6, &result), UTILS_STATUS_TIMEOUT);
    assert_int_equal(DARE(&A, &B, &Q, &R, 100, 1e-6, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], 2.947122f, 1e-5);
    assert_float_equal(result.data[1], 2.369205f, 1e-5);
    assert_float_equal(result.data[2], 2.369205f, 1e-5);
    assert_float_equal(result.data[3], 4.613134f, 1e-5);
    matrixDelete(&A);
    matrixDelete(&B);
    matrixDelete(&Q);
    matrixDelete(&R);
    matrixDelete(&result);
}

static void test_GaussNewton_Sens_Cal_9(void** state) {
    (void)state; /* unused */
    matrix_t Data, Data2, X0, result;
    float Data_data[] = {0.207943,  0.176336,   -10.472851, -0.032664, -9.691158,  -0.525727, 9.739063,  0.100049,
                         -0.033633, -0.014726,  9.848834,   -0.441255, -9.848631,  0.111619,  -0.447592, -0.817354,
                         1.245916,  -10.409218, -0.954637,  -0.850694, -10.362207, 1.181917,  -0.940953, -10.381795,
                         1.234348,  1.037082,   -10.323170, 0.493486,  2.144262,   9.028279,  -1.948339, 1.323566,
                         9.077987,  -1.715729,  -1.417367,  9.021542,  1.402404,   -1.214459, 9.140464,  6.862244,
                         7.007604,  -0.370883,  7.013921,   -6.699038, -0.329180,  -6.915473, -6.891760, -0.527419,
                         -7.064906, 6.897670,   -0.619903,  3.656467,  3.660516,   -9.034408, 3.640905,  -3.403118,
                         -9.039276, -3.512549,  -3.599191,  -9.047305};
    float X0_data[9] = {0, 0, 0, 1, 0, 0, 1, 0, 1};
    matrixInit(&Data, 20, 3);
    matrixInit(&X0, 9, 1);
    matrixInit(&result, 9, 1);
    memcpy(Data.data, Data_data, 60 * sizeof(float));
    memcpy(X0.data, X0_data, 9 * sizeof(float));
    assert_int_equal(GaussNewton_Sens_Cal_9(&Data, 9.81, &X0, 2, 1e-6, &result), UTILS_STATUS_TIMEOUT);
    assert_int_equal(GaussNewton_Sens_Cal_9(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -0.045825f, 1e-5);
    assert_float_equal(result.data[1], 0.078952f, 1e-5);
    assert_float_equal(result.data[2], -0.586952f, 1e-5);
    assert_float_equal(result.data[3], 1.000657f, 1e-5);
    assert_float_equal(result.data[4], 0.000065f, 1e-5);
    assert_float_equal(result.data[5], 0.000919f, 1e-5);
    assert_float_equal(result.data[6], 1.003975f, 1e-5);
    assert_float_equal(result.data[7], 0.002118f, 1e-5);
    assert_float_equal(result.data[8], 0.991289f, 1e-5);

    /* Check all errors*/
    assert_int_equal(GaussNewton_Sens_Cal_9(&Data, 9.81, &X0, 2, 1e-6, &result), UTILS_STATUS_TIMEOUT);
    Data.rows = 8;
    assert_int_equal(GaussNewton_Sens_Cal_9(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    Data.rows = 20;
    Data.cols = 2;
    assert_int_equal(GaussNewton_Sens_Cal_9(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    float Data_data2[] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
    matrixInit(&Data2, 9, 3);
    memcpy(Data2.data, Data_data2, 27 * sizeof(float));
    assert_int_equal(GaussNewton_Sens_Cal_9(&Data2, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);

    matrixDelete(&Data);
    matrixDelete(&Data2);
    matrixDelete(&X0);
    matrixDelete(&result);
}

static void test_GaussNewton_Sens_Cal_6(void** state) {
    (void)state; /* unused */
    matrix_t Data, Data2, X0, result;
    float Data_data[] = {0.207943,  0.176336,   -10.472851, -0.032664, -9.691158,  -0.525727, 9.739063,  0.100049,
                         -0.033633, -0.014726,  9.848834,   -0.441255, -9.848631,  0.111619,  -0.447592, -0.817354,
                         1.245916,  -10.409218, -0.954637,  -0.850694, -10.362207, 1.181917,  -0.940953, -10.381795,
                         1.234348,  1.037082,   -10.323170, 0.493486,  2.144262,   9.028279,  -1.948339, 1.323566,
                         9.077987,  -1.715729,  -1.417367,  9.021542,  1.402404,   -1.214459, 9.140464,  6.862244,
                         7.007604,  -0.370883,  7.013921,   -6.699038, -0.329180,  -6.915473, -6.891760, -0.527419,
                         -7.064906, 6.897670,   -0.619903,  3.656467,  3.660516,   -9.034408, 3.640905,  -3.403118,
                         -9.039276, -3.512549,  -3.599191,  -9.047305};
    float X0_data[6] = {0, 0, 0, 1, 1, 1};
    matrixInit(&Data, 20, 3);
    matrixInit(&X0, 6, 1);
    matrixInit(&result, 6, 1);
    memcpy(Data.data, Data_data, 60 * sizeof(float));
    memcpy(X0.data, X0_data, 6 * sizeof(float));
    assert_int_equal(GaussNewton_Sens_Cal_6(&Data, 9.81, &X0, 2, 1e-6, &result), UTILS_STATUS_TIMEOUT);
    assert_int_equal(GaussNewton_Sens_Cal_6(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -0.043898f, 1e-5);
    assert_float_equal(result.data[1], 0.081555f, 1e-5);
    assert_float_equal(result.data[2], -0.586624f, 1e-5);
    assert_float_equal(result.data[3], 1.000679f, 1e-5);
    assert_float_equal(result.data[4], 1.004023f, 1e-5);
    assert_float_equal(result.data[5], 0.991293f, 1e-5);

    /* Check all errors*/
    assert_int_equal(GaussNewton_Sens_Cal_6(&Data, 9.81, &X0, 2, 1e-6, &result), UTILS_STATUS_TIMEOUT);
    Data.rows = 5;
    assert_int_equal(GaussNewton_Sens_Cal_6(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    Data.rows = 20;
    Data.cols = 2;
    assert_int_equal(GaussNewton_Sens_Cal_6(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    float Data_data2[] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
    matrixInit(&Data2, 9, 3);
    memcpy(Data2.data, Data_data2, 27 * sizeof(float));
    assert_int_equal(GaussNewton_Sens_Cal_6(&Data2, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);

    matrixDelete(&Data);
    matrixDelete(&Data2);
    matrixDelete(&X0);
    matrixDelete(&result);
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION

static void test_LU_CormenStatic(void** state) {
    (void)state; /* unused */
    matrix_t A, L, U;
    float A_data[4] = {4, 3, 6, 3};
    float L_data[4];
    float U_data[4];
    matrixInitStatic(&A, A_data, 2, 2);
    matrixInitStatic(&L, L_data, 2, 2);
    matrixInitStatic(&U, U_data, 2, 2);
    assert_int_equal(LU_CormenStatic(&A, &L, &U), UTILS_STATUS_SUCCESS);
    assert_float_equal(L.data[0], 1.0f, 1e-5);
    assert_float_equal(L.data[1], 0.0f, 1e-5);
    assert_float_equal(L.data[2], 1.5f, 1e-5);
    assert_float_equal(L.data[3], 1.0f, 1e-5);
    assert_float_equal(U.data[0], 4.0f, 1e-5);
    assert_float_equal(U.data[1], 3.0f, 1e-5);
    assert_float_equal(U.data[2], 0.0f, 1e-5);
    assert_float_equal(U.data[3], -1.5f, 1e-5);
}

static void test_LUP_CormenStatic(void** state) {
    (void)state; /* unused */
    matrix_t A, L, U, P;
    float A_data[9] = {2, 0, 2, 1, 1, 1, 2, 1, 3};
    float L_data[9];
    float U_data[9];
    float P_data[3];
    matrixInitStatic(&A, A_data, 3, 3);
    matrixInitStatic(&L, L_data, 3, 3);
    matrixInitStatic(&U, U_data, 3, 3);
    matrixInitStatic(&P, P_data, 3, 1);
    assert_int_equal(LUP_CormenStatic(&A, &L, &U, &P), 1);
    assert_float_equal(L.data[0], 1.0f, 1e-5);
    assert_float_equal(L.data[1], 0.0f, 1e-5);
    assert_float_equal(L.data[2], 0.0f, 1e-5);
    assert_float_equal(L.data[3], 0.5f, 1e-5);
    assert_float_equal(L.data[4], 1.0f, 1e-5);
    assert_float_equal(L.data[5], 0.0f, 1e-5);
    assert_float_equal(L.data[6], 1.0f, 1e-5);
    assert_float_equal(L.data[7], 1.0f, 1e-5);
    assert_float_equal(L.data[8], 1.0f, 1e-5);
    assert_float_equal(U.data[0], 2.0f, 1e-5);
    assert_float_equal(U.data[1], 0.0f, 1e-5);
    assert_float_equal(U.data[2], 2.0f, 1e-5);
    assert_float_equal(U.data[3], 0.0f, 1e-5);
    assert_float_equal(U.data[4], 1.0f, 1e-5);
    assert_float_equal(U.data[5], 0.0f, 1e-5);
    assert_float_equal(U.data[6], 0.0f, 1e-5);
    assert_float_equal(U.data[7], 0.0f, 1e-5);
    assert_float_equal(U.data[8], 1.0f, 1e-5);
}

static void test_LinSolveLUStatic(void** state) {
    (void)state; /* unused */
    matrix_t A, B, result;
    float A_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456,
                      0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    float B_data[] = {0.7547, 0.1626, 0.3404, 0.2551, 0.2760, 0.1190, 0.5853, 0.5060,
                      0.6797, 0.4984, 0.2238, 0.6991, 0.6551, 0.9597, 0.7513, 0.8909};
    float result_data[16];
    matrixInitStatic(&A, A_data, 4, 4);
    matrixInitStatic(&B, B_data, 4, 4);
    matrixInitStatic(&result, result_data, 4, 4);
    LinSolveLUStatic(&A, &B, &result);
    assert_float_equal(result.data[0], -0.142065f, 1e-5);
    assert_float_equal(result.data[1], 1.492795f, 1e-5);
    assert_float_equal(result.data[2], -0.310884f, 1e-5);
    assert_float_equal(result.data[3], 1.097069f, 1e-5);
    assert_float_equal(result.data[4], -0.318367f, 1e-5);
    assert_float_equal(result.data[5], 1.098035f, 1e-5);
    assert_float_equal(result.data[6], 0.753304f, 1e-5);
    assert_float_equal(result.data[7], 1.084532f, 1e-5);
    assert_float_equal(result.data[8], -1.344099f, 1e-5);
    assert_float_equal(result.data[9], 1.503635f, 1e-5);
    assert_float_equal(result.data[10], 0.079810f, 1e-5);
    assert_float_equal(result.data[11], 1.904844f, 1e-5);
    assert_float_equal(result.data[12], 2.951679f, 1e-5);
    assert_float_equal(result.data[13], -3.205921f, 1e-5);
    assert_float_equal(result.data[14], 0.489882f, 1e-5);
    assert_float_equal(result.data[15], -2.882036f, 1e-5);
}

static void test_LinSolveLUPStatic(void** state) {
    (void)state; /* unused */
    matrix_t A, B, result;
    float A_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456,
                      0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    float B_data[] = {0.7547, 0.1626, 0.3404, 0.2551, 0.2760, 0.1190, 0.5853, 0.5060,
                      0.6797, 0.4984, 0.2238, 0.6991, 0.6551, 0.9597, 0.7513, 0.8909};
    float result_data[16];
    matrixInitStatic(&A, A_data, 4, 4);
    matrixInitStatic(&B, B_data, 4, 4);
    matrixInitStatic(&result, result_data, 4, 4);
    LinSolveLUPStatic(&A, &B, &result);
    assert_float_equal(result.data[0], -0.142065f, 1e-5);
    assert_float_equal(result.data[1], 1.492795f, 1e-5);
    assert_float_equal(result.data[2], -0.310884f, 1e-5);
    assert_float_equal(result.data[3], 1.097069f, 1e-5);
    assert_float_equal(result.data[4], -0.318367f, 1e-5);
    assert_float_equal(result.data[5], 1.098035f, 1e-5);
    assert_float_equal(result.data[6], 0.753304f, 1e-5);
    assert_float_equal(result.data[7], 1.084532f, 1e-5);
    assert_float_equal(result.data[8], -1.344099f, 1e-5);
    assert_float_equal(result.data[9], 1.503635f, 1e-5);
    assert_float_equal(result.data[10], 0.079810f, 1e-5);
    assert_float_equal(result.data[11], 1.904844f, 1e-5);
    assert_float_equal(result.data[12], 2.951679f, 1e-5);
    assert_float_equal(result.data[13], -3.205921f, 1e-5);
    assert_float_equal(result.data[14], 0.489882f, 1e-5);
    assert_float_equal(result.data[15], -2.882036f, 1e-5);
}

static void test_LinSolveGaussStatic(void** state) {
    (void)state; /* unused */
    matrix_t A, B, result;
    float A_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456,
                      0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    float B_data[] = {0.7547, 0.1626, 0.3404, 0.2551, 0.2760, 0.1190, 0.5853, 0.5060,
                      0.6797, 0.4984, 0.2238, 0.6991, 0.6551, 0.9597, 0.7513, 0.8909};
    float result_data[16];
    matrixInitStatic(&A, A_data, 4, 4);
    matrixInitStatic(&B, B_data, 4, 4);
    matrixInitStatic(&result, result_data, 4, 4);
    LinSolveGaussStatic(&A, &B, &result);
    assert_float_equal(result.data[0], -0.142065f, 1e-5);
    assert_float_equal(result.data[1], 1.492795f, 1e-5);
    assert_float_equal(result.data[2], -0.310884f, 1e-5);
    assert_float_equal(result.data[3], 1.097069f, 1e-5);
    assert_float_equal(result.data[4], -0.318367f, 1e-5);
    assert_float_equal(result.data[5], 1.098035f, 1e-5);
    assert_float_equal(result.data[6], 0.753304f, 1e-5);
    assert_float_equal(result.data[7], 1.084532f, 1e-5);
    assert_float_equal(result.data[8], -1.344099f, 1e-5);
    assert_float_equal(result.data[9], 1.503635f, 1e-5);
    assert_float_equal(result.data[10], 0.079810f, 1e-5);
    assert_float_equal(result.data[11], 1.904844f, 1e-5);
    assert_float_equal(result.data[12], 2.951679f, 1e-5);
    assert_float_equal(result.data[13], -3.205921f, 1e-5);
    assert_float_equal(result.data[14], 0.489882f, 1e-5);
    assert_float_equal(result.data[15], -2.882036f, 1e-5);
}

static void test_DAREStatic(void** state) {
    (void)state; /* unused */
    matrix_t A, B, Q, R, result;
    float A_data[4] = {1, 1, 0, 1};
    float B_data[2] = {0, 1};
    float Q_data[4] = {1, 0, 0, 1};
    float R_data[1] = {1};
    float result_data[4];
    matrixInitStatic(&A, A_data, 2, 2);
    matrixInitStatic(&B, B_data, 2, 1);
    matrixInitStatic(&Q, Q_data, 2, 2);
    matrixInitStatic(&R, R_data, 1, 1);
    matrixInitStatic(&result, result_data, 2, 2);
    assert_int_equal(DAREStatic(&A, &B, &Q, &R, 2, 1e-6, &result), UTILS_STATUS_TIMEOUT);
    assert_int_equal(DAREStatic(&A, &B, &Q, &R, 100, 1e-6, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], 2.947122f, 1e-5);
    assert_float_equal(result.data[1], 2.369205f, 1e-5);
    assert_float_equal(result.data[2], 2.369205f, 1e-5);
    assert_float_equal(result.data[3], 4.613134f, 1e-5);
}

static void test_GaussNewton_Sens_Cal_9Static(void** state) {
    (void)state; /* unused */
    matrix_t Data, Data2, X0, result;
    float Data_data[] = {0.207943,  0.176336,   -10.472851, -0.032664, -9.691158,  -0.525727, 9.739063,  0.100049,
                         -0.033633, -0.014726,  9.848834,   -0.441255, -9.848631,  0.111619,  -0.447592, -0.817354,
                         1.245916,  -10.409218, -0.954637,  -0.850694, -10.362207, 1.181917,  -0.940953, -10.381795,
                         1.234348,  1.037082,   -10.323170, 0.493486,  2.144262,   9.028279,  -1.948339, 1.323566,
                         9.077987,  -1.715729,  -1.417367,  9.021542,  1.402404,   -1.214459, 9.140464,  6.862244,
                         7.007604,  -0.370883,  7.013921,   -6.699038, -0.329180,  -6.915473, -6.891760, -0.527419,
                         -7.064906, 6.897670,   -0.619903,  3.656467,  3.660516,   -9.034408, 3.640905,  -3.403118,
                         -9.039276, -3.512549,  -3.599191,  -9.047305};
    float X0_data[9] = {0, 0, 0, 1, 0, 0, 1, 0, 1};
    float result_data[9];
    matrixInitStatic(&Data, Data_data, 20, 3);
    matrixInitStatic(&X0, X0_data, 9, 1);
    matrixInitStatic(&result, result_data, 9, 1);
    assert_int_equal(GaussNewton_Sens_Cal_9Static(&Data, 9.81, &X0, 2, 1e-6, &result), UTILS_STATUS_TIMEOUT);
    assert_int_equal(GaussNewton_Sens_Cal_9Static(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -0.045825f, 1e-5);
    assert_float_equal(result.data[1], 0.078952f, 1e-5);
    assert_float_equal(result.data[2], -0.586952f, 1e-5);
    assert_float_equal(result.data[3], 1.000657f, 1e-5);
    assert_float_equal(result.data[4], 0.000065f, 1e-5);
    assert_float_equal(result.data[5], 0.000919f, 1e-5);
    assert_float_equal(result.data[6], 1.003975f, 1e-5);
    assert_float_equal(result.data[7], 0.002118f, 1e-5);
    assert_float_equal(result.data[8], 0.991289f, 1e-5);

    /* Check all errors*/
    assert_int_equal(GaussNewton_Sens_Cal_9Static(&Data, 9.81, &X0, 2, 1e-6, &result), UTILS_STATUS_TIMEOUT);
    Data.rows = 8;
    assert_int_equal(GaussNewton_Sens_Cal_9Static(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    Data.rows = 20;
    Data.cols = 2;
    assert_int_equal(GaussNewton_Sens_Cal_9Static(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    float Data_data2[] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
    matrixInitStatic(&Data2, Data_data2, 9, 3);
    assert_int_equal(GaussNewton_Sens_Cal_9Static(&Data2, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
}

static void test_GaussNewton_Sens_Cal_6Static(void** state) {
    (void)state; /* unused */
    matrix_t Data, Data2, X0, result;
    float Data_data[] = {0.207943,  0.176336,   -10.472851, -0.032664, -9.691158,  -0.525727, 9.739063,  0.100049,
                         -0.033633, -0.014726,  9.848834,   -0.441255, -9.848631,  0.111619,  -0.447592, -0.817354,
                         1.245916,  -10.409218, -0.954637,  -0.850694, -10.362207, 1.181917,  -0.940953, -10.381795,
                         1.234348,  1.037082,   -10.323170, 0.493486,  2.144262,   9.028279,  -1.948339, 1.323566,
                         9.077987,  -1.715729,  -1.417367,  9.021542,  1.402404,   -1.214459, 9.140464,  6.862244,
                         7.007604,  -0.370883,  7.013921,   -6.699038, -0.329180,  -6.915473, -6.891760, -0.527419,
                         -7.064906, 6.897670,   -0.619903,  3.656467,  3.660516,   -9.034408, 3.640905,  -3.403118,
                         -9.039276, -3.512549,  -3.599191,  -9.047305};
    float X0_data[6] = {0, 0, 0, 1, 1, 1};
    float result_data[6];
    matrixInitStatic(&Data, Data_data, 20, 3);
    matrixInitStatic(&X0, X0_data, 6, 1);
    matrixInitStatic(&result, result_data, 6, 1);
    assert_int_equal(GaussNewton_Sens_Cal_6Static(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -0.043898f, 1e-5);
    assert_float_equal(result.data[1], 0.081555f, 1e-5);
    assert_float_equal(result.data[2], -0.586624f, 1e-5);
    assert_float_equal(result.data[3], 1.000679f, 1e-5);
    assert_float_equal(result.data[4], 1.004023f, 1e-5);
    assert_float_equal(result.data[5], 0.991293f, 1e-5);

    /* Check all errors*/
    assert_int_equal(GaussNewton_Sens_Cal_6Static(&Data, 9.81, &X0, 2, 1e-6, &result), UTILS_STATUS_TIMEOUT);
    Data.rows = 5;
    assert_int_equal(GaussNewton_Sens_Cal_6Static(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    Data.rows = 20;
    Data.cols = 2;
    assert_int_equal(GaussNewton_Sens_Cal_6Static(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    float Data_data2[] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
    matrixInitStatic(&Data2, Data_data2, 9, 3);
    assert_int_equal(GaussNewton_Sens_Cal_6Static(&Data2, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
}

#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_fwsub),
        cmocka_unit_test(test_fwsubPerm),
        cmocka_unit_test(test_bksub),
        cmocka_unit_test(test_bksubPerm),
        cmocka_unit_test(test_QuadProd),
        cmocka_unit_test(test_LU_Crout),
#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
        cmocka_unit_test(test_LU_Cormen),
        cmocka_unit_test(test_LUP_Cormen),
        cmocka_unit_test(test_LinSolveLU),
        cmocka_unit_test(test_LinSolveLUP),
        cmocka_unit_test(test_LinSolveGauss),
        cmocka_unit_test(test_DARE),
        cmocka_unit_test(test_GaussNewton_Sens_Cal_9),
        cmocka_unit_test(test_GaussNewton_Sens_Cal_6),
#endif
#ifdef ADVUTILS_USE_STATIC_ALLOCATION
        cmocka_unit_test(test_LU_CormenStatic),
        cmocka_unit_test(test_LUP_CormenStatic),
        cmocka_unit_test(test_LinSolveLUStatic),
        cmocka_unit_test(test_LinSolveLUPStatic),
        cmocka_unit_test(test_LinSolveGaussStatic),
        cmocka_unit_test(test_DAREStatic),
        cmocka_unit_test(test_GaussNewton_Sens_Cal_9Static),
        cmocka_unit_test(test_GaussNewton_Sens_Cal_6Static),
#endif
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
