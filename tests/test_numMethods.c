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

#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "numMethods.h"
#include "stdio.h"

#include <cmocka.h>

/* Support functions ---------------------------------------------------------*/

static uint8_t allocFail = 0;

/* Result test */

void* ADVUtils_testCalloc(const size_t number_of_elements, const size_t size) {
    if (allocFail == 1) {
        return NULL;
    } else if (number_of_elements > 0) {
        return test_calloc(number_of_elements, size);
    } else {
        return NULL;
    }
}

void* ADVUtils_testMalloc(const size_t size) {
    if (allocFail == 1) {
        return NULL;
    } else if (size > 0) {
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
    /* Check ill-conditioned matrix */
    float A2_data[] = {0, 7, 6, 2};
    memcpy(A_data, A2_data, 4 * sizeof(float));
    assert_int_equal(LU_Crout(&A, &L, &U), UTILS_STATUS_ERROR);
    { /* finite-value guard: non-finite pivot -> ERROR */
        float infd[9] = {INFINITY, 0, 0, 0, 1, 0, 0, 0, 1};
        float lg[9], ug[9];
        matrix_t Ainf, Lg, Ug;
        matrixInitStatic(&Ainf, infd, 3, 3);
        matrixInitStatic(&Lg, lg, 3, 3);
        matrixInitStatic(&Ug, ug, 3, 3);
        assert_int_equal(LU_Crout(&Ainf, &Lg, &Ug), UTILS_STATUS_ERROR);
    }
}

static void test_Cholesky(void** state) {
    (void)state; /* unused */
    matrix_t A, L;
    float A_data[9] = {4, 12, -16, 12, 37, -43, -16, -43, 98};
    float L_data[9];
    matrixInitStatic(&A, A_data, 3, 3);
    matrixInitStatic(&L, L_data, 3, 3);
    assert_int_equal(Cholesky(&A, &L), UTILS_STATUS_SUCCESS);
    float L_exp[9] = {2, 0, 0, 6, 1, 0, -8, 5, 3};
    for (uint8_t i = 0; i < 9; i++) {
        assert_float_equal(L.data[i], L_exp[i], 1e-5);
    }
    /* non-SPD matrix -> error */
    matrix_t N, L2;
    float N_data[9] = {1, 2, 3, 2, 1, 4, 3, 4, 1};
    float L2_data[9];
    matrixInitStatic(&N, N_data, 3, 3);
    matrixInitStatic(&L2, L2_data, 3, 3);
    assert_int_equal(Cholesky(&N, &L2), UTILS_STATUS_ERROR);
    /* dimension mismatch -> assert */
    matrix_t ANS;
    float ANS_data[6] = {1, 0, 0, 1, 0, 0};
    matrixInitStatic(&ANS, ANS_data, 3, 2);
    skipAssert = 0;
    expect_assert_failure(Cholesky(&ANS, &L));
    { /* finite-value guard: non-finite pivot -> ERROR */
        float infd[9] = {INFINITY, 0, 0, 0, 1, 0, 0, 0, 1};
        float lg[9];
        matrix_t Ainf, Lg;
        matrixInitStatic(&Ainf, infd, 3, 3);
        matrixInitStatic(&Lg, lg, 3, 3);
        assert_int_equal(Cholesky(&Ainf, &Lg), UTILS_STATUS_ERROR);
    }
}

static void test_LU_Cormen(void** state) {
    (void)state; /* unused */
    matrix_t A, L, U;
    float A_data[4] = {4, 3, 6, 3};
    float L_data[4];
    float U_data[4];
    matrixInitStatic(&A, A_data, 2, 2);
    matrixInitStatic(&L, L_data, 2, 2);
    matrixInitStatic(&U, U_data, 2, 2);
    /* Allocation error */
    allocFail = 1;
    assert_int_equal(LU_Cormen(&A, &L, &U), UTILS_STATUS_ERROR);
    allocFail = 0;
    /* Result test */
    assert_int_equal(LU_Cormen(&A, &L, &U), UTILS_STATUS_SUCCESS);
    assert_float_equal(L.data[0], 1.0f, 1e-5);
    assert_float_equal(L.data[1], 0.0f, 1e-5);
    assert_float_equal(L.data[2], 1.5f, 1e-5);
    assert_float_equal(L.data[3], 1.0f, 1e-5);
    assert_float_equal(U.data[0], 4.0f, 1e-5);
    assert_float_equal(U.data[1], 3.0f, 1e-5);
    assert_float_equal(U.data[2], 0.0f, 1e-5);
    assert_float_equal(U.data[3], -1.5f, 1e-5);
    matrixZeros(&L);
    matrixZeros(&U);
    assert_int_equal(LU_CormenStatic(&A, &L, &U), UTILS_STATUS_SUCCESS);
    assert_float_equal(L.data[0], 1.0f, 1e-5);
    assert_float_equal(L.data[1], 0.0f, 1e-5);
    assert_float_equal(L.data[2], 1.5f, 1e-5);
    assert_float_equal(L.data[3], 1.0f, 1e-5);
    assert_float_equal(U.data[0], 4.0f, 1e-5);
    assert_float_equal(U.data[1], 3.0f, 1e-5);
    assert_float_equal(U.data[2], 0.0f, 1e-5);
    assert_float_equal(U.data[3], -1.5f, 1e-5);
    matrixZeros(&L);
    matrixZeros(&U);

    /* Check ill-conditioned matrix */
    float A2_data[] = {0, 7, 6, 2};
    memcpy(A_data, A2_data, 4 * sizeof(float));
    assert_int_equal(LU_Cormen(&A, &L, &U), UTILS_STATUS_ERROR);
    assert_int_equal(LU_CormenStatic(&A, &L, &U), UTILS_STATUS_ERROR);
    { /* finite-value guard: non-finite pivot -> ERROR */
        float infd[9] = {INFINITY, 0, 0, 0, 1, 0, 0, 0, 1};
        float lg[9], ug[9];
        matrix_t Ainf, Lg, Ug;
        matrixInitStatic(&Ainf, infd, 3, 3);
        matrixInitStatic(&Lg, lg, 3, 3);
        matrixInitStatic(&Ug, ug, 3, 3);
        assert_int_equal(LU_Cormen(&Ainf, &Lg, &Ug), UTILS_STATUS_ERROR);
        assert_int_equal(LU_CormenStatic(&Ainf, &Lg, &Ug), UTILS_STATUS_ERROR);
    }
}

static void test_LUP_Cormen(void** state) {
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
    /* Allocation error */
    allocFail = 1;
    assert_int_equal(LUP_Cormen(&A, &L, &U, &P), 0);
    allocFail = 0;
    /* Result test */
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
    matrixZeros(&L);
    matrixZeros(&U);
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
    matrixZeros(&L);
    matrixZeros(&U);
    /* Check ill-conditioned matrix */
    float A2_data[] = {0, 0, 6, 2, 1, 1, 2, 1, 3};
    memcpy(A_data, A2_data, 9 * sizeof(float));
    assert_int_equal(LUP_Cormen(&A, &L, &U, &P), 0);
    assert_int_equal(LUP_CormenStatic(&A, &L, &U, &P), 0);
}

static void test_QR_Householder(void** state) {
    (void)state; /* unused */
    matrix_t A, Q, R, QR, QtQ;
    float A_data[12] = {1, 2, 3, 4, 5, 6, 7, 8, 10, 1, 0, 1};
    float Q_data[12], R_data[9], QR_data[12], QtQ_data[9];
    matrixInitStatic(&A, A_data, 4, 3);
    matrixInitStatic(&Q, Q_data, 4, 3);
    matrixInitStatic(&R, R_data, 3, 3);
    matrixInitStatic(&QR, QR_data, 4, 3);
    matrixInitStatic(&QtQ, QtQ_data, 3, 3);
    /* Allocation error */
    allocFail = 1;
    assert_int_equal(QR_Householder(&A, &Q, &R), UTILS_STATUS_ERROR);
    allocFail = 0;
    /* Result test */
    assert_int_equal(QR_Householder(&A, &Q, &R), UTILS_STATUS_SUCCESS);
    matrixMult(&Q, &R, &QR);
    for (uint8_t i = 0; i < 12; i++) {
        assert_float_equal(QR.data[i], A_data[i], 1e-5);
    }
    matrixZeros(&Q);
    matrixZeros(&R);
    matrixZeros(&QR);
    assert_int_equal(QR_HouseholderStatic(&A, &Q, &R), UTILS_STATUS_SUCCESS);
    matrixMult(&Q, &R, &QR);
    for (uint8_t i = 0; i < 12; i++) {
        assert_float_equal(QR.data[i], A_data[i], 1e-5);
    }
    matrixMult_lhsT(&Q, &Q, &QtQ);
    float I3[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
    for (uint8_t i = 0; i < 9; i++) {
        assert_float_equal(QtQ.data[i], I3[i], 1e-5);
    }
    /* LAPACK sign convention: element-wise match to np.linalg.qr (loose tol pins signs) */
    assert_float_equal(R.data[0], -8.185353f, 1e-5);
    assert_float_equal(R.data[4], 1.481226f, 1e-5);
    assert_float_equal(R.data[8], 0.996593f, 1e-5);
    assert_float_equal(Q.data[0], -0.122169f, 1e-5);
    assert_float_equal(Q.data[1], 0.564277f, 1e-5);
    /* rank-deficient -> error */
    matrix_t D, Qd, Rd;
    float D_data[6] = {1, 2, 2, 4, 3, 6};
    float Qd_data[6], Rd_data[4];
    matrixInitStatic(&D, D_data, 3, 2);
    matrixInitStatic(&Qd, Qd_data, 3, 2);
    matrixInitStatic(&Rd, Rd_data, 2, 2);
    assert_int_equal(QR_Householder(&D, &Qd, &Rd), UTILS_STATUS_ERROR);
    assert_int_equal(QR_HouseholderStatic(&D, &Qd, &Rd), UTILS_STATUS_ERROR);
    float D_data2[6] = {0, 1, 0, 2, 0, 3};
    memcpy(D.data, D_data2, 6 * sizeof(float));
    assert_int_equal(QR_Householder(&D, &Qd, &Rd), UTILS_STATUS_ERROR);
    assert_int_equal(QR_HouseholderStatic(&D, &Qd, &Rd), UTILS_STATUS_ERROR);
    /* dimension assert (rows < cols) */
    matrix_t W, Qw, Rw;
    float W_data[6] = {1, 2, 3, 4, 5, 6};
    float Qw_data[6], Rw_data[9];
    matrixInitStatic(&W, W_data, 2, 3);
    matrixInitStatic(&Qw, Qw_data, 2, 3);
    matrixInitStatic(&Rw, Rw_data, 3, 3);
    skipAssert = 0;
    expect_assert_failure(QR_Householder(&W, &Qw, &Rw));
    expect_assert_failure(QR_HouseholderStatic(&W, &Qw, &Rw));
    /* finite-value guard: non-finite and column-overflow -> ERROR */
    float tinf[6] = {INFINITY, 2, 3, 4, 5, 7};
    float tov[6] = {1, 1e20f, 0, 1e20f, 0, 1e20f};
    float qg[6], rg[4];
    matrix_t Tinf, Tov, Qg, Rg;
    matrixInitStatic(&Tinf, tinf, 3, 2);
    matrixInitStatic(&Tov, tov, 3, 2);
    matrixInitStatic(&Qg, qg, 3, 2);
    matrixInitStatic(&Rg, rg, 2, 2);
    assert_int_equal(QR_Householder(&Tinf, &Qg, &Rg), UTILS_STATUS_ERROR);
    assert_int_equal(QR_HouseholderStatic(&Tinf, &Qg, &Rg), UTILS_STATUS_ERROR);
    assert_int_equal(QR_Householder(&Tov, &Qg, &Rg), UTILS_STATUS_ERROR);
    assert_int_equal(QR_HouseholderStatic(&Tov, &Qg, &Rg), UTILS_STATUS_ERROR);
}

static void test_LinSolveLU(void** state) {
    (void)state; /* unused */
    matrix_t A, B, result;
    float A_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456, 0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    float B_data[] = {0.7547, 0.1626, 0.3404, 0.2551, 0.2760, 0.1190, 0.5853, 0.5060, 0.6797, 0.4984, 0.2238, 0.6991, 0.6551, 0.9597, 0.7513, 0.8909};
    float result_data[16];
    matrixInitStatic(&A, A_data, 4, 4);
    matrixInitStatic(&B, B_data, 4, 4);
    matrixInitStatic(&result, result_data, 4, 4);
    /* Allocation error */
    allocFail = 1;
    assert_int_equal(LinSolveLU(&A, &B, &result), UTILS_STATUS_ERROR);
    allocFail = 0;
    /* Result test */
    assert_int_equal(LinSolveLU(&A, &B, &result), UTILS_STATUS_SUCCESS);
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
    matrixZeros(&result);
    assert_int_equal(LinSolveLUStatic(&A, &B, &result), UTILS_STATUS_SUCCESS);
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
    matrixZeros(&result);
    /* singular matrix -> ERROR */
    float A2_data[9] = {1, 2, 3, 2, 4, 6, 7, 8, 10};
    float B2_data[3] = {1, 2, 3};
    matrixInitStatic(&A, A2_data, 3, 3);
    matrixInitStatic(&B, B2_data, 3, 1);
    matrixInitStatic(&result, result_data, 3, 1);
    assert_int_equal(LinSolveLU(&A, &B, &result), UTILS_STATUS_ERROR);
    assert_int_equal(LinSolveLUStatic(&A, &B, &result), UTILS_STATUS_ERROR);
    /* overflow --> ERROR */
    float A3_data[4] = {1e-30f, 0.0f, 0.0f, 1e-30f};
    float B3_data[2] = {1e20f, 1e20f};
    matrixInitStatic(&A, A3_data, 2, 2);
    matrixInitStatic(&B, B3_data, 2, 1);
    matrixInitStatic(&result, result_data, 2, 1);
    assert_int_equal(LinSolveLU(&A, &B, &result), UTILS_STATUS_ERROR);
    assert_int_equal(LinSolveLUStatic(&A, &B, &result), UTILS_STATUS_ERROR);
}

static void test_LinSolveLUP(void** state) {
    (void)state; /* unused */
    matrix_t A, B, result;
    float A_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456, 0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    float B_data[] = {0.7547, 0.1626, 0.3404, 0.2551, 0.2760, 0.1190, 0.5853, 0.5060, 0.6797, 0.4984, 0.2238, 0.6991, 0.6551, 0.9597, 0.7513, 0.8909};
    float result_data[16];
    matrixInitStatic(&A, A_data, 4, 4);
    matrixInitStatic(&B, B_data, 4, 4);
    matrixInitStatic(&result, result_data, 4, 4);
    /* Allocation error */
    allocFail = 1;
    assert_int_equal(LinSolveLUP(&A, &B, &result), UTILS_STATUS_ERROR);
    allocFail = 0;
    /* Result test */
    assert_int_equal(LinSolveLUP(&A, &B, &result), UTILS_STATUS_SUCCESS);
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
    matrixZeros(&result);
    assert_int_equal(LinSolveLUPStatic(&A, &B, &result), UTILS_STATUS_SUCCESS);
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
    matrixZeros(&result);
    /* singular matrix -> ERROR */
    float A2_data[9] = {1, 2, 3, 2, 4, 6, 7, 8, 10};
    float B2_data[3] = {1, 2, 3};
    matrixInitStatic(&A, A2_data, 3, 3);
    matrixInitStatic(&B, B2_data, 3, 1);
    matrixInitStatic(&result, result_data, 3, 1);
    assert_int_equal(LinSolveLUP(&A, &B, &result), UTILS_STATUS_ERROR);
    assert_int_equal(LinSolveLUPStatic(&A, &B, &result), UTILS_STATUS_ERROR);
    /* singular matrix with a zero pivot column -> LUP_CormenStatic returns 0 -> ERROR */
    float A3_data[9] = {0.0f, 1.0f, 2.0f, 0.0f, 3.0f, 4.0f, 0.0f, 5.0f, 6.0f};
    float B3_data[3] = {1.0f, 2.0f, 3.0f};
    matrixInitStatic(&A, A3_data, 3, 3);
    matrixInitStatic(&B, B3_data, 3, 1);
    matrixInitStatic(&result, result_data, 3, 1);
    assert_int_equal(LinSolveLUP(&A, &B, &result), UTILS_STATUS_ERROR);
    assert_int_equal(LinSolveLUPStatic(&A, &B, &result), UTILS_STATUS_ERROR);
}

static void test_LinSolveGauss(void** state) {
    (void)state; /* unused */
    matrix_t A, B, result;
    float A_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456, 0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    float B_data[] = {0.7547, 0.1626, 0.3404, 0.2551, 0.2760, 0.1190, 0.5853, 0.5060, 0.6797, 0.4984, 0.2238, 0.6991, 0.6551, 0.9597, 0.7513, 0.8909};
    float result_data[16];
    matrixInitStatic(&A, A_data, 4, 4);
    matrixInitStatic(&B, B_data, 4, 4);
    matrixInitStatic(&result, result_data, 4, 4);
    /* Allocation error */
    allocFail = 1;
    assert_int_equal(LinSolveGauss(&A, &B, &result), UTILS_STATUS_ERROR);
    allocFail = 0;
    /* Result test */
    assert_int_equal(LinSolveGauss(&A, &B, &result), UTILS_STATUS_SUCCESS);
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
    matrixZeros(&result);
    assert_int_equal(LinSolveGaussStatic(&A, &B, &result), UTILS_STATUS_SUCCESS);
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
    matrixZeros(&result);
    /* Check return 0 if matrix is singular */
    float A2_data[] = {0, 0, 6, 2, 1, 1, 2, 1, 3};
    float B2_data[] = {12, 7, 9};
    matrixInitStatic(&A, A2_data, 3, 3);
    matrixInitStatic(&B, B2_data, 3, 1);
    matrixInitStatic(&result, result_data, 3, 1);
    LinSolveGauss(&A, &B, &result);
    assert_float_equal(result.data[0], 0.f, 1e-5);
    assert_float_equal(result.data[1], 0.f, 1e-5);
    assert_float_equal(result.data[2], 0.f, 1e-5);
    LinSolveGaussStatic(&A, &B, &result);
    assert_float_equal(result.data[0], 0.f, 1e-5);
    assert_float_equal(result.data[1], 0.f, 1e-5);
    assert_float_equal(result.data[2], 0.f, 1e-5);
    /* singular matrix -> ERROR */
    float A3_data[9] = {1, 2, 3, 2, 4, 6, 7, 8, 10};
    float B3_data[3] = {1, 2, 3};
    matrixInitStatic(&A, A3_data, 3, 3);
    matrixInitStatic(&B, B3_data, 3, 1);
    matrixInitStatic(&result, result_data, 3, 1);
    assert_int_equal(LinSolveGauss(&A, &B, &result), UTILS_STATUS_ERROR);
    assert_int_equal(LinSolveGaussStatic(&A, &B, &result), UTILS_STATUS_ERROR);
}

static void test_LinSolveCholesky(void** state) {
    (void)state; /* unused */
    matrix_t A, B, result;
    float A_data[9] = {4, 12, -16, 12, 37, -43, -16, -43, 98};
    float B_data[6] = {1, 2, 2, 1, 3, 0};
    float result_data[6];
    matrixInitStatic(&A, A_data, 3, 3);
    matrixInitStatic(&B, B_data, 3, 2);
    matrixInitStatic(&result, result_data, 3, 2);
    float X_exp[6] = {28.583333f, 85.166667f, -7.666667f, -23.333333f, 1.333333f, 3.666667f};
    /* Allocation error */
    allocFail = 1;
    assert_int_equal(LinSolveCholesky(&A, &B, &result), UTILS_STATUS_ERROR);
    allocFail = 0;
    /* Result test */
    assert_int_equal(LinSolveCholesky(&A, &B, &result), UTILS_STATUS_SUCCESS);
    for (uint8_t i = 0; i < 6; i++) {
        assert_float_equal(result.data[i], X_exp[i], 1e-5);
    }
    matrixZeros(&result);
    assert_int_equal(LinSolveCholeskyStatic(&A, &B, &result), UTILS_STATUS_SUCCESS);
    for (uint8_t i = 0; i < 6; i++) {
        assert_float_equal(result.data[i], X_exp[i], 1e-5);
    }
    matrixZeros(&result);
    /* non-SPD A -> error propagated */
    matrix_t N;
    float N_data[9] = {1, 2, 3, 2, 1, 4, 3, 4, 1};
    matrixInitStatic(&N, N_data, 3, 3);
    assert_int_equal(LinSolveCholesky(&N, &B, &result), UTILS_STATUS_ERROR);
    assert_int_equal(LinSolveCholeskyStatic(&N, &B, &result), UTILS_STATUS_ERROR);
    /* finite-value guard: non-finite result -> ERROR */
    float spd[9] = {4, 12, -16, 12, 37, -43, -16, -43, 98};
    float bnan[3] = {NAN, 1.0f, 2.0f};
    matrixInitStatic(&A, spd, 3, 3);
    matrixInitStatic(&B, bnan, 3, 1);
    matrixInitStatic(&result, result_data, 3, 1);
    assert_int_equal(LinSolveCholesky(&A, &B, &result), UTILS_STATUS_ERROR);
    assert_int_equal(LinSolveCholeskyStatic(&A, &B, &result), UTILS_STATUS_ERROR);
}

static void test_LinSolveQR(void** state) {
    (void)state; /* unused */
    matrix_t A, B, result;
    float A_data[] = {0.5432, 0.3171, 0.3816, 0.4898, 0.0462, 0.4358, 0.6651, 0.4456, 0.8235, 0.1324, 0.7952, 0.6463, 0.6948, 0.9745, 0.1869, 0.4456};
    float B_data[] = {0.7547, 0.1626, 0.3404, 0.2551, 0.2760, 0.1190, 0.5853, 0.5060, 0.6797, 0.4984, 0.2238, 0.6991, 0.6551, 0.9597, 0.7513, 0.8909};
    float result_data[16];
    matrixInitStatic(&A, A_data, 4, 4);
    matrixInitStatic(&B, B_data, 4, 4);
    matrixInitStatic(&result, result_data, 4, 4);
    memcpy(A.data, A_data, 16 * sizeof(float));
    memcpy(B.data, B_data, 16 * sizeof(float));
    /* Allocation error */
    allocFail = 1;
    assert_int_equal(LinSolveQR(&A, &B, &result), UTILS_STATUS_ERROR);
    allocFail = 0;
    /* Result test */
    assert_int_equal(LinSolveQR(&A, &B, &result), UTILS_STATUS_SUCCESS);
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
    matrixZeros(&result);
    assert_int_equal(LinSolveQRStatic(&A, &B, &result), UTILS_STATUS_SUCCESS);
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
    matrixZeros(&result);
    /* tall least-squares 4x3 */
    float At_data[12] = {1, 2, 3, 4, 5, 6, 7, 8, 10, 1, 0, 1};
    float Bt_data[8] = {1, 2, 0, 1, 3, 0, 1, 1};
    matrixInitStatic(&A, At_data, 4, 3);
    matrixInitStatic(&B, Bt_data, 4, 2);
    matrixInitStatic(&result, result_data, 3, 2);
    float result_exp[6] = {0.287671f, -1.260274f, -1.260274f, -1.383562f, 1.041096f, 2.034247f};
    assert_int_equal(LinSolveQR(&A, &B, &result), UTILS_STATUS_SUCCESS);
    for (uint8_t i = 0; i < 6; i++) {
        assert_float_equal(result.data[i], result_exp[i], 1e-5);
    }
    matrixZeros(&result);
    assert_int_equal(LinSolveQRStatic(&A, &B, &result), UTILS_STATUS_SUCCESS);
    for (uint8_t i = 0; i < 6; i++) {
        assert_float_equal(result.data[i], result_exp[i], 1e-5);
    }
    matrixZeros(&result);
    /* rank-deficient -> error */
    float Ad_data[6] = {1, 2, 2, 4, 3, 6};
    float Bd_data[3] = {1, 2, 3};
    matrixInitStatic(&A, Ad_data, 3, 2);
    matrixInitStatic(&B, Bd_data, 3, 1);
    matrixInitStatic(&result, result_data, 2, 1);
    assert_int_equal(LinSolveQR(&A, &B, &result), UTILS_STATUS_ERROR);
    assert_int_equal(LinSolveQRStatic(&A, &B, &result), UTILS_STATUS_ERROR);
    /* finite-value guard: non-finite result -> ERROR */
    float tall[6] = {1, 2, 3, 4, 5, 7};
    float bnan[3] = {NAN, 2.0f, 3.0f};
    matrixInitStatic(&A, tall, 3, 2);
    matrixInitStatic(&B, bnan, 3, 1);
    matrixInitStatic(&result, result_data, 2, 1);
    assert_int_equal(LinSolveQR(&A, &B, &result), UTILS_STATUS_ERROR);
    assert_int_equal(LinSolveQRStatic(&A, &B, &result), UTILS_STATUS_ERROR);
}

static void test_DARE(void** state) {
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
    assert_int_equal(DARE(&A, &B, &Q, &R, 2, 1e-6, &result), UTILS_STATUS_TIMEOUT);
    assert_int_equal(DAREStatic(&A, &B, &Q, &R, 2, 1e-6, &result), UTILS_STATUS_TIMEOUT);
    /* Allocation error */
    allocFail = 1;
    assert_int_equal(DARE(&A, &B, &Q, &R, 100, 1e-6, &result), UTILS_STATUS_ERROR);
    allocFail = 0;
    /* Result test */
    assert_int_equal(DARE(&A, &B, &Q, &R, 100, 1e-6, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], 2.947122f, 1e-5);
    assert_float_equal(result.data[1], 2.369205f, 1e-5);
    assert_float_equal(result.data[2], 2.369205f, 1e-5);
    assert_float_equal(result.data[3], 4.613134f, 1e-5);
    matrixZeros(&result);
    assert_int_equal(DAREStatic(&A, &B, &Q, &R, 100, 1e-6, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], 2.947122f, 1e-5);
    assert_float_equal(result.data[1], 2.369205f, 1e-5);
    assert_float_equal(result.data[2], 2.369205f, 1e-5);
    assert_float_equal(result.data[3], 4.613134f, 1e-5);
}

static void test_GaussNewton_Sens_Cal_9(void** state) {
    (void)state; /* unused */
    matrix_t Data, Data2, X0, result;
    float Data_data[] = {0.207943,  0.176336,  -10.472851, -0.032664, -9.691158, -0.525727,  9.739063,  0.100049,  -0.033633,  -0.014726, 9.848834,  -0.441255,
                         -9.848631, 0.111619,  -0.447592,  -0.817354, 1.245916,  -10.409218, -0.954637, -0.850694, -10.362207, 1.181917,  -0.940953, -10.381795,
                         1.234348,  1.037082,  -10.323170, 0.493486,  2.144262,  9.028279,   -1.948339, 1.323566,  9.077987,   -1.715729, -1.417367, 9.021542,
                         1.402404,  -1.214459, 9.140464,   6.862244,  7.007604,  -0.370883,  7.013921,  -6.699038, -0.329180,  -6.915473, -6.891760, -0.527419,
                         -7.064906, 6.897670,  -0.619903,  3.656467,  3.660516,  -9.034408,  3.640905,  -3.403118, -9.039276,  -3.512549, -3.599191, -9.047305};
    float X0_data[9] = {0, 0, 0, 1, 0, 0, 1, 0, 1};
    float result_data[9];
    matrixInitStatic(&Data, Data_data, 20, 3);
    matrixInitStatic(&X0, X0_data, 9, 1);
    matrixInitStatic(&result, result_data, 9, 1);
    /* Result test */
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
    matrixZeros(&result);
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
    matrixZeros(&result);

    /* Check without X0 and without radius*/
    assert_int_equal(GaussNewton_Sens_Cal_9(&Data, 0, NULL, 600, 1e-6, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -0.045825f, 1e-5);
    assert_float_equal(result.data[1], 0.078952f, 1e-5);
    assert_float_equal(result.data[2], -0.586952f, 1e-5);
    assert_float_equal(result.data[3], 1.092682, 1e-5);
    assert_float_equal(result.data[4], 0.000065f, 1e-5);
    assert_float_equal(result.data[5], 0.001003f, 1e-5);
    assert_float_equal(result.data[6], 1.096306f, 1e-5);
    assert_float_equal(result.data[7], 0.002313f, 1e-5);
    assert_float_equal(result.data[8], 1.082453f, 1e-5);
    matrixZeros(&result);
    assert_int_equal(GaussNewton_Sens_Cal_9Static(&Data, 0, NULL, 600, 1e-6, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -0.045825f, 1e-5);
    assert_float_equal(result.data[1], 0.078952f, 1e-5);
    assert_float_equal(result.data[2], -0.586952f, 1e-5);
    assert_float_equal(result.data[3], 1.092682, 1e-5);
    assert_float_equal(result.data[4], 0.000065f, 1e-5);
    assert_float_equal(result.data[5], 0.001003f, 1e-5);
    assert_float_equal(result.data[6], 1.096306f, 1e-5);
    assert_float_equal(result.data[7], 0.002313f, 1e-5);
    assert_float_equal(result.data[8], 1.082453f, 1e-5);
    matrixZeros(&result);

    /* Check all errors*/
    allocFail = 1;
    assert_int_equal(GaussNewton_Sens_Cal_9(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    allocFail = 0;
    assert_int_equal(GaussNewton_Sens_Cal_9(&Data, 9.81, &X0, 2, 1e-6, &result), UTILS_STATUS_TIMEOUT);
    assert_int_equal(GaussNewton_Sens_Cal_9Static(&Data, 9.81, &X0, 2, 1e-6, &result), UTILS_STATUS_TIMEOUT);
    Data.rows = 8;
    assert_int_equal(GaussNewton_Sens_Cal_9(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    assert_int_equal(GaussNewton_Sens_Cal_9Static(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    Data.rows = 20;
    Data.cols = 2;
    assert_int_equal(GaussNewton_Sens_Cal_9(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    assert_int_equal(GaussNewton_Sens_Cal_9Static(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    float Data_data2[] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
    matrixInitStatic(&Data2, Data_data2, 9, 3);
    assert_int_equal(GaussNewton_Sens_Cal_9(&Data2, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    assert_int_equal(GaussNewton_Sens_Cal_9Static(&Data2, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
}

static void test_GaussNewton_Sens_Cal_6(void** state) {
    (void)state; /* unused */
    matrix_t Data, Data2, X0, result;
    float Data_data[] = {0.207943,  0.176336,  -10.472851, -0.032664, -9.691158, -0.525727,  9.739063,  0.100049,  -0.033633,  -0.014726, 9.848834,  -0.441255,
                         -9.848631, 0.111619,  -0.447592,  -0.817354, 1.245916,  -10.409218, -0.954637, -0.850694, -10.362207, 1.181917,  -0.940953, -10.381795,
                         1.234348,  1.037082,  -10.323170, 0.493486,  2.144262,  9.028279,   -1.948339, 1.323566,  9.077987,   -1.715729, -1.417367, 9.021542,
                         1.402404,  -1.214459, 9.140464,   6.862244,  7.007604,  -0.370883,  7.013921,  -6.699038, -0.329180,  -6.915473, -6.891760, -0.527419,
                         -7.064906, 6.897670,  -0.619903,  3.656467,  3.660516,  -9.034408,  3.640905,  -3.403118, -9.039276,  -3.512549, -3.599191, -9.047305};
    float X0_data[6] = {0, 0, 0, 1, 1, 1};
    float result_data[6];
    matrixInitStatic(&Data, Data_data, 20, 3);
    matrixInitStatic(&X0, X0_data, 6, 1);
    matrixInitStatic(&result, result_data, 6, 1);
    /* Result test */
    assert_int_equal(GaussNewton_Sens_Cal_6(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -0.043898f, 1e-5);
    assert_float_equal(result.data[1], 0.081555f, 1e-5);
    assert_float_equal(result.data[2], -0.586624f, 1e-5);
    assert_float_equal(result.data[3], 1.000679f, 1e-5);
    assert_float_equal(result.data[4], 1.004023f, 1e-5);
    assert_float_equal(result.data[5], 0.991293f, 1e-5);
    matrixZeros(&result);
    assert_int_equal(GaussNewton_Sens_Cal_6Static(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -0.043898f, 1e-5);
    assert_float_equal(result.data[1], 0.081555f, 1e-5);
    assert_float_equal(result.data[2], -0.586624f, 1e-5);
    assert_float_equal(result.data[3], 1.000679f, 1e-5);
    assert_float_equal(result.data[4], 1.004023f, 1e-5);
    assert_float_equal(result.data[5], 0.991293f, 1e-5);
    matrixZeros(&result);

    /* Check without X0 and without radius*/
    assert_int_equal(GaussNewton_Sens_Cal_6(&Data, 0, NULL, 600, 1e-6, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -0.043898f, 1e-5);
    assert_float_equal(result.data[1], 0.081555f, 1e-5);
    assert_float_equal(result.data[2], -0.586624f, 1e-5);
    assert_float_equal(result.data[3], 1.092707f, 1e-5);
    assert_float_equal(result.data[4], 1.096358f, 1e-5);
    assert_float_equal(result.data[5], 1.082458f, 1e-5);
    matrixZeros(&result);
    assert_int_equal(GaussNewton_Sens_Cal_6Static(&Data, 0, NULL, 600, 1e-6, &result), UTILS_STATUS_SUCCESS);
    assert_float_equal(result.data[0], -0.043898f, 1e-5);
    assert_float_equal(result.data[1], 0.081555f, 1e-5);
    assert_float_equal(result.data[2], -0.586624f, 1e-5);
    assert_float_equal(result.data[3], 1.092707f, 1e-5);
    assert_float_equal(result.data[4], 1.096358f, 1e-5);
    assert_float_equal(result.data[5], 1.082458f, 1e-5);
    matrixZeros(&result);

    /* Check all errors*/
    allocFail = 1;
    assert_int_equal(GaussNewton_Sens_Cal_6(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    allocFail = 0;
    assert_int_equal(GaussNewton_Sens_Cal_6(&Data, 9.81, &X0, 2, 1e-6, &result), UTILS_STATUS_TIMEOUT);
    assert_int_equal(GaussNewton_Sens_Cal_6Static(&Data, 9.81, &X0, 2, 1e-6, &result), UTILS_STATUS_TIMEOUT);
    Data.rows = 5;
    assert_int_equal(GaussNewton_Sens_Cal_6(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    assert_int_equal(GaussNewton_Sens_Cal_6Static(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    Data.rows = 20;
    Data.cols = 2;
    assert_int_equal(GaussNewton_Sens_Cal_6(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    assert_int_equal(GaussNewton_Sens_Cal_6Static(&Data, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    float Data_data2[] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
    matrixInitStatic(&Data2, Data_data2, 9, 3);
    assert_int_equal(GaussNewton_Sens_Cal_6(&Data2, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
    assert_int_equal(GaussNewton_Sens_Cal_6Static(&Data2, 9.81, &X0, 600, 1e-6, &result), UTILS_STATUS_ERROR);
}

int main(void) {
    const struct CMUnitTest test_numMethods[] = {
        cmocka_unit_test(test_fwsub),
        cmocka_unit_test(test_fwsubPerm),
        cmocka_unit_test(test_bksub),
        cmocka_unit_test(test_bksubPerm),
        cmocka_unit_test(test_QuadProd),
        cmocka_unit_test(test_LU_Crout),
        cmocka_unit_test(test_Cholesky),
        cmocka_unit_test(test_LU_Cormen),
        cmocka_unit_test(test_LUP_Cormen),
        cmocka_unit_test(test_QR_Householder),
        cmocka_unit_test(test_LinSolveLU),
        cmocka_unit_test(test_LinSolveLUP),
        cmocka_unit_test(test_LinSolveGauss),
        cmocka_unit_test(test_LinSolveCholesky),
        cmocka_unit_test(test_LinSolveQR),
        cmocka_unit_test(test_DARE),
        cmocka_unit_test(test_GaussNewton_Sens_Cal_9),
        cmocka_unit_test(test_GaussNewton_Sens_Cal_6),
    };

    return cmocka_run_group_tests(test_numMethods, NULL, NULL);
}
