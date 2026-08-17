/* BEGIN Header */
/**
 ******************************************************************************
 * \file            numMethods.c
 * \author          Andrea Vivani
 * \brief           Implementation of several numerical methods
 ******************************************************************************
 * \copyright
 *
 * Copyright 2016 Andrea Vivani
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

/* Configuration check -------------------------------------------------------*/
#if !defined(ADVUTILS_USE_DYNAMIC_ALLOCATION) && !defined(ADVUTILS_USE_STATIC_ALLOCATION)
#error Either ADVUTILS_USE_DYNAMIC_ALLOCATION or ADVUTILS_USE_STATIC_ALLOCATION must be set for ADVUtils to work
#endif

/* Includes ------------------------------------------------------------------*/

#include "numMethods.h"
#include "ADVUtilsAssert.h"
#include "ADVUtilsMemory.h"
#include "basicMath.h"
#include "math.h"

/* Private Typedefs ----------------------------------------------------------*/
typedef utilsStatus_t (*s_linSolveQR_fn)(const matrix_t*, const matrix_t*, matrix_t*);
typedef utilsStatus_t (*s_inverse_fn)(const matrix_t*, matrix_t*);

/* Private Function Prototypes -----------------------------------------------*/
static uint32_t s_LU_Cormen_scratchBufWords(const matrix_t* A);
static uint32_t s_LUP_Cormen_scratchBufWords(const matrix_t* A);
static uint32_t s_QR_Householder_scratchBufWords(const matrix_t* A);
static uint32_t s_LinSolveLU_scratchBufWords(const matrix_t* A);
static uint32_t s_LinSolveLUP_scratchBufWords(const matrix_t* A, const matrix_t* B);
static uint32_t s_LinSolveGauss_scratchBufWords(const matrix_t* A, const matrix_t* B);
static uint32_t s_LinSolveCholesky_scratchBufWords(const matrix_t* A, const matrix_t* B);
static uint32_t s_LinSolveQR_scratchBufWords(const matrix_t* A, const matrix_t* B);
static uint32_t s_DARE_scratchBufWords(const matrix_t* A, const matrix_t* B, const matrix_t* R);
static uint32_t s_GaussNewton_Sens_Cal_9_scratchBufWords(const matrix_t* Data);
static uint32_t s_GaussNewton_Sens_Cal_6_scratchBufWords(const matrix_t* Data);
static utilsStatus_t s_LU_Cormen_impl(const matrix_t* A, matrix_t* L, matrix_t* U, matrixBuffer_t* buf);
static int8_t s_LUP_Cormen_impl(const matrix_t* A, matrix_t* L, matrix_t* U, matrix_t* P, matrixBuffer_t* buf);
static utilsStatus_t s_QR_Householder_impl(const matrix_t* A, matrix_t* Q, matrix_t* R, matrixBuffer_t* buf);
static utilsStatus_t s_LinSolveLU_impl(const matrix_t* A, const matrix_t* B, matrix_t* result, matrixBuffer_t* buf);
static utilsStatus_t s_LinSolveLUP_impl(const matrix_t* A, const matrix_t* B, matrix_t* result, matrixBuffer_t* buf);
static utilsStatus_t s_LinSolveGauss_impl(const matrix_t* A, const matrix_t* B, matrix_t* result, matrixBuffer_t* buf);
static utilsStatus_t s_LinSolveCholesky_impl(const matrix_t* A, const matrix_t* B, matrix_t* result, matrixBuffer_t* buf);
static utilsStatus_t s_LinSolveQR_impl(const matrix_t* A, const matrix_t* B, matrix_t* result, matrixBuffer_t* buf);
static utilsStatus_t s_DARE_impl(const matrix_t* A, const matrix_t* B, const matrix_t* Q, const matrix_t* R, uint16_t nmax, float tol, matrix_t* result,
                                 matrixBuffer_t* buf, s_inverse_fn inverse);
static utilsStatus_t s_GaussNewton_Sens_Cal_9_impl(const matrix_t* Data, float k, const matrix_t* X0, uint16_t nmax, float tol, matrix_t* result,
                                                   matrixBuffer_t* buf, s_linSolveQR_fn linSolve);
static utilsStatus_t s_GaussNewton_Sens_Cal_6_impl(const matrix_t* Data, float k, const matrix_t* X0, uint16_t nmax, float tol, matrix_t* result,
                                                   matrixBuffer_t* buf, s_linSolveQR_fn linSolve);

/* ---------------------Is Matrix finite------------------------ */
/* returns 1 if every element of the matrix is finite, 0 otherwise */

static uint8_t isMatrixFinite(const matrix_t* m) {
    uint8_t finite = 1U;
    uint16_t size = (uint16_t)((uint16_t)m->rows * (uint16_t)m->cols);
    for (uint16_t i = 0U; i < size; i++) {
        if (isfinite(m->data[i]) == 0) {
            finite = 0U;
            break;
        }
    }
    return finite;
}

/* -------------------Forward substitution---------------------- */
/* assumes that the matrix A is already a lower triangular one. No check! */

void fwsub(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    for (uint8_t i = 0; i < B->cols; i++) {
        ELEMP(result, 0, i) = ELEMP(B, 0, i) / ELEMP(A, 0, 0);
        for (uint8_t j = 1; j < A->rows; j++) {
            float tmp = 0.0f;
            for (uint8_t k = 0; k < j; k++) {
                tmp += ELEMP(A, j, k) * (ELEMP(result, k, i));
            }
            ELEMP(result, j, i) = (ELEMP(B, j, i) - tmp) / ELEMP(A, j, j);
        }
    }
    return;
}

/* ---------------Forward substitution with permutation------------------- */
/* assumes that the matrix A is already a lower triangular one. No check! */

void fwsubPerm(const matrix_t* A, const matrix_t* B, const matrix_t* P, matrix_t* result) {
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    ADVUTILS_ASSERT(P->rows == A->rows);
    ADVUTILS_ASSERT(P->cols == 1U);
    for (uint8_t i = 0; i < B->cols; i++) {
        ELEMP(result, 0, i) = ELEMP(B, (uint8_t)ELEMP(P, 0, 0), i) / ELEMP(A, 0, 0);
        for (uint8_t j = 1; j < A->rows; j++) {
            float tmp = 0.0f;
            for (uint8_t k = 0; k < j; k++) {
                tmp += ELEMP(A, j, k) * ELEMP(result, k, i);
            }
            ELEMP(result, j, i) = (ELEMP(B, (uint8_t)ELEMP(P, j, 0), i) - tmp) / ELEMP(A, j, j);
        }
    }
    return;
}

/* -------------------Backward substitution---------------------- */
/* assumes that the matrix A is already an upper triangular one. No check! */

void bksub(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    for (uint8_t i = 0; i < B->cols; i++) {
        ELEMP(result, (A->cols - 1U), i) = ELEMP(B, (A->cols - 1U), i) / ELEMP(A, (A->cols - 1U), (A->cols - 1U));
        for (int16_t j = (int16_t)((int32_t)A->rows - 2); j >= 0; j--) {
            float tmp = 0.0f;
            for (uint8_t k = (uint8_t)((uint32_t)A->cols - 1U); (int16_t)k > j; k--) {
                tmp += ELEMP(A, j, k) * ELEMP(result, k, i);
            }
            ELEMP(result, j, i) = (ELEMP(B, j, i) - tmp) / ELEMP(A, j, j);
        }
    }
    return;
}

/* --------------Backward substitution with permutation----------------- */
/* assumes that the matrix A is already an upper triangular one. No check! */

void bksubPerm(const matrix_t* A, const matrix_t* B, const matrix_t* P, matrix_t* result) {
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    ADVUTILS_ASSERT(P->rows == A->rows);
    ADVUTILS_ASSERT(P->cols == 1U);
    for (uint8_t i = 0; i < B->cols; i++) {
        ELEMP(result, (A->cols - 1U), i) = ELEMP(B, (uint8_t)ELEMP(P, (A->cols - 1U), 0), i) / ELEMP(A, (A->cols - 1U), (A->cols - 1U));
        for (int16_t j = (int16_t)((int32_t)A->rows - 2); j >= 0; j--) {
            float tmp = 0.0f;
            for (uint8_t k = (uint8_t)((uint32_t)A->cols - 1U); (int16_t)k > j; k--) {
                tmp += ELEMP(A, j, k) * ELEMP(result, k, i);
            }
            ELEMP(result, j, i) = (ELEMP(B, (uint8_t)ELEMP(P, j, 0), i) - tmp) / ELEMP(A, j, j);
        }
    }
    return;
}

/* ------------------Quadratic form (sort of)---------------------- */
/* returns matrix C=A*B*(~A) */

void QuadProd(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->cols == B->rows);
    ADVUTILS_ASSERT(B->cols == A->cols);
    ADVUTILS_ASSERT(result->rows == A->rows);
    ADVUTILS_ASSERT(result->cols == A->rows);
    matrixZeros(result);
    for (uint8_t n = 0; n < A->rows; n++) {
        for (uint8_t i = 0; i < A->cols; i++) {
            float tmp = 0.0f;
            for (uint8_t j = 0; j < A->cols; j++) {
                tmp += ELEMP(A, n, j) * ELEMP(B, i, j);
            }
            for (uint8_t j = 0; j < A->rows; j++) {
                ELEMP(result, j, n) += ELEMP(A, j, i) * tmp;
            }
        }
    }
    return;
}

/* -------------------------LU factorization using Crout's Method-------------------------------- */
/* factorizes the A matrix as the product of a unit upper triangular matrix U and a lower triangular matrix L */

utilsStatus_t LU_Crout(const matrix_t* A, matrix_t* L, matrix_t* U) {
    ADVUTILS_ASSERT(A->cols == A->rows);
    ADVUTILS_ASSERT(L->rows == A->rows);
    ADVUTILS_ASSERT(L->cols == A->cols);
    ADVUTILS_ASSERT(U->rows == A->cols);
    ADVUTILS_ASSERT(U->cols == A->cols);
    matrixIdentity(U);
    matrixZeros(L);
    for (uint8_t i = 0; i < A->rows; i++) {
        for (uint8_t j = i; j < A->rows; j++) {
            float sum = 0.0f;
            for (uint8_t k = 0; k < i; k++) {
                sum += ELEMP(L, j, k) * ELEMP(U, k, i);
            }
            ELEMP(L, j, i) = ELEMP(A, j, i) - sum;
        }

        for (uint8_t j = i; j < A->rows; j++) {
            float sum = 0.0f;
            for (uint8_t k = 0; k < i; k++) {
                sum += ELEMP(L, i, k) * ELEMP(U, k, j);
            }
            if ((ELEMP(L, i, i) == 0) || (isfinite(ELEMP(L, i, i)) == 0)) {
                return UTILS_STATUS_ERROR;
            }
            ELEMP(U, i, j) = (ELEMP(A, i, j) - sum) / ELEMP(L, i, i);
        }
    }
    return UTILS_STATUS_SUCCESS;
}

/* ------------------Cholesky factorization---------------------- */
/* computes lower-triangular L such that L*(~L)=A for symmetric positive-definite A */
/* only the lower triangle of A is read; returns error on a non-positive pivot */

utilsStatus_t Cholesky(const matrix_t* A, matrix_t* L) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(L->rows == A->rows);
    ADVUTILS_ASSERT(L->cols == A->cols);
    for (uint8_t j = 0; j < A->cols; j++) {
        float sum = ELEMP(A, j, j);
        for (uint8_t k = 0; k < j; k++) {
            sum -= ELEMP(L, j, k) * ELEMP(L, j, k);
        }
        if ((sum <= 0.0f) || (isfinite(sum) == 0)) {
            return UTILS_STATUS_ERROR;
        }
        ELEMP(L, j, j) = SQRT(sum);
        for (uint8_t i = (uint8_t)(j + 1U); i < A->rows; i++) {
            float s = ELEMP(A, i, j);
            for (uint8_t k = 0; k < j; k++) {
                s -= ELEMP(L, i, k) * ELEMP(L, j, k);
            }
            ELEMP(L, i, j) = s / ELEMP(L, j, j);
            ELEMP(L, j, i) = 0.0f; /* zero strict upper triangle */
        }
    }
    return UTILS_STATUS_SUCCESS;
}

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION

/* -------------------------LU factorization using Cormen's Method-------------------------------- */
/* factorizes the A matrix as the product of a unit upper triangular matrix U and a lower triangular matrix L */

utilsStatus_t LU_Cormen(const matrix_t* A, matrix_t* L, matrix_t* U) {
    ADVUTILS_ASSERT(A->cols == A->rows);
    ADVUTILS_ASSERT(L->rows == A->rows);
    ADVUTILS_ASSERT(L->cols == A->cols);
    ADVUTILS_ASSERT(U->rows == A->cols);
    ADVUTILS_ASSERT(U->cols == A->cols);
    matrixBuffer_t buf;
    if (matrixBufferAllocate(&buf, s_LU_Cormen_scratchBufWords(A)) == UTILS_STATUS_ERROR) {
        return UTILS_STATUS_ERROR;
    }
    utilsStatus_t status = s_LU_Cormen_impl(A, L, U, &buf);
    ADVUTILS_FREE(buf.base);
    return status;
}

/* -----------------------LUP factorization using Cormen's Method------------------------------ */
/* factorizes the A matrix as the product of a upper triangular matrix U and a unit lower triangular matrix L */
/* returns the factor that has to be multiplied to the determinant of U in order to obtain the correct value */

int8_t LUP_Cormen(const matrix_t* A, matrix_t* L, matrix_t* U, matrix_t* P) {
    ADVUTILS_ASSERT(A->cols == A->rows);
    ADVUTILS_ASSERT(L->rows == A->rows);
    ADVUTILS_ASSERT(L->cols == A->cols);
    ADVUTILS_ASSERT(U->rows == A->cols);
    ADVUTILS_ASSERT(U->cols == A->cols);
    ADVUTILS_ASSERT(P->rows == A->rows);
    ADVUTILS_ASSERT(P->cols == 1U);
    matrixBuffer_t buf;
    if (matrixBufferAllocate(&buf, s_LUP_Cormen_scratchBufWords(A)) == UTILS_STATUS_ERROR) {
        return 0;
    }
    int8_t status = s_LUP_Cormen_impl(A, L, U, P, &buf);
    ADVUTILS_FREE(buf.base);
    return status;
}

/* ------------------QR factorization (Householder)---------------------- */
/* economy Householder QR of a tall matrix A (rows >= cols): A = Q*R, with Q rows-by-cols */
/* orthonormal columns and R cols-by-cols upper-triangular. Returns error if rank-deficient */

utilsStatus_t QR_Householder(const matrix_t* A, matrix_t* Q, matrix_t* R) {
    ADVUTILS_ASSERT(A->rows >= A->cols);
    ADVUTILS_ASSERT(Q->rows == A->rows);
    ADVUTILS_ASSERT(Q->cols == A->cols);
    ADVUTILS_ASSERT(R->rows == A->cols);
    ADVUTILS_ASSERT(R->cols == A->cols);
    matrixBuffer_t buf;
    if (matrixBufferAllocate(&buf, s_QR_Householder_scratchBufWords(A)) == UTILS_STATUS_ERROR) {
        return UTILS_STATUS_ERROR;
    }
    utilsStatus_t status = s_QR_Householder_impl(A, Q, R, &buf);
    ADVUTILS_FREE(buf.base);
    return status;
}

/* -----------------------Linear system solver using LU factorization--------------------------- */
/* solves the linear system A*X=B, where A is a n-by-n matrix and B an n-by-m matrix, giving the n-by-m matrix X */

utilsStatus_t LinSolveLU(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    matrixBuffer_t buf;
    if (matrixBufferAllocate(&buf, s_LinSolveLU_scratchBufWords(A)) == UTILS_STATUS_ERROR) {
        return UTILS_STATUS_ERROR;
    }
    utilsStatus_t status = s_LinSolveLU_impl(A, B, result, &buf);
    ADVUTILS_FREE(buf.base);
    return status;
}

/* ----------------------Linear system solver using LUP factorization-------------------------- */
/* solves the linear system A*X=B, where A is a n-by-n matrix and B an n-by-m matrix, giving the n-by-m matrix X */

utilsStatus_t LinSolveLUP(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    matrixBuffer_t buf;
    if (matrixBufferAllocate(&buf, s_LinSolveLUP_scratchBufWords(A, B)) == UTILS_STATUS_ERROR) {
        return UTILS_STATUS_ERROR;
    }
    utilsStatus_t status = s_LinSolveLUP_impl(A, B, result, &buf);
    ADVUTILS_FREE(buf.base);
    return status;
}

/* ------------Linear system solver using Gauss elimination with partial pivoting--------------- */
/* solves the linear system A*X=B, where A is a n-by-n matrix and B an n-by-m matrix, giving the n-by-m matrix X */

utilsStatus_t LinSolveGauss(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    matrixBuffer_t buf;
    if (matrixBufferAllocate(&buf, s_LinSolveGauss_scratchBufWords(A, B)) == UTILS_STATUS_ERROR) {
        return UTILS_STATUS_ERROR;
    }
    utilsStatus_t status = s_LinSolveGauss_impl(A, B, result, &buf);
    ADVUTILS_FREE(buf.base);
    return status;
}

/* ----------------------Linear system solver using Cholesky factorization-------------------------- */
/* solves the linear system A*X=B for symmetric positive-definite A, giving the n-by-m matrix X */

utilsStatus_t LinSolveCholesky(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    matrixBuffer_t buf;
    if (matrixBufferAllocate(&buf, s_LinSolveCholesky_scratchBufWords(A, B)) == UTILS_STATUS_ERROR) {
        return UTILS_STATUS_ERROR;
    }
    utilsStatus_t status = s_LinSolveCholesky_impl(A, B, result, &buf);
    ADVUTILS_FREE(buf.base);
    return status;
}

/* ----------------------Linear system solver using QR factorization-------------------------- */
/* solves A*X=B (least-squares when rows>cols) via economy Householder QR: X = R\(Q'*B) */

utilsStatus_t LinSolveQR(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows >= A->cols);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->rows == A->cols);
    ADVUTILS_ASSERT(result->cols == B->cols);
    matrixBuffer_t buf;
    if (matrixBufferAllocate(&buf, s_LinSolveQR_scratchBufWords(A, B)) == UTILS_STATUS_ERROR) {
        return UTILS_STATUS_ERROR;
    }
    utilsStatus_t status = s_LinSolveQR_impl(A, B, result, &buf);
    ADVUTILS_FREE(buf.base);
    return status;
}

/* -------Iterative solver for discrete-time algebraic Riccati equation--------- */
/* Solves discrete-time algebraic Riccati equation P = A'*P*A-(B'*P*A)'*inv(R+B'*P*B)*B'*P*A+Q */
utilsStatus_t DARE(const matrix_t* A, const matrix_t* B, const matrix_t* Q, const matrix_t* R, uint16_t nmax, float tol, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(R->rows == R->cols);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(R->rows == B->cols);
    ADVUTILS_ASSERT(Q->rows == A->rows);
    ADVUTILS_ASSERT(Q->rows == Q->cols);
    ADVUTILS_ASSERT(result->rows == result->cols);
    ADVUTILS_ASSERT(result->rows == A->cols);
    matrixBuffer_t buf;
    if (matrixBufferAllocate(&buf, s_DARE_scratchBufWords(A, B, R)) == UTILS_STATUS_ERROR) {
        return UTILS_STATUS_ERROR;
    }
    utilsStatus_t status = s_DARE_impl(A, B, Q, R, nmax, tol, result, &buf, &matrixInversed_rob);
    ADVUTILS_FREE(buf.base);
    return status;
}

/* ------------Gauss-Newton sensors calibration with 9 parameters--------------- */
/* approximates Data to a sphere of radius k by calculating 6 gains (s) and 3 biases (b), useful to calibrate some sensors (meas_sphere=S*(meas-B) with S symmetric) */
/* Data has n>=9 rows corresponding to the number of measures and 3 columns corresponding to the 3 axes */
/* X0 is the starting guess vector (usually [0 0 0 1 0 0 1 0 1]), nmax the maximum number of iterations (200 is generally fine, even if it usually converges within 10 iterations), and tol the stopping tolerance (1e-6 is usually more than fine) */
/*b1=out(0,0);
 b2=out(1,0);
 b3=out(2,0);
 s11=out(3,0);
 s12=out(4,0);
 s13=out(5,0);
 s22=out(6,0);
 s23=out(7,0);
 s33=out(8,0);*/

utilsStatus_t GaussNewton_Sens_Cal_9(const matrix_t* Data, float k, const matrix_t* X0, uint16_t nmax, float tol, matrix_t* result) {
    matrixBuffer_t buf;
    if (matrixBufferAllocate(&buf, s_GaussNewton_Sens_Cal_9_scratchBufWords(Data)) == UTILS_STATUS_ERROR) {
        return UTILS_STATUS_ERROR;
    }
    utilsStatus_t status = s_GaussNewton_Sens_Cal_9_impl(Data, k, X0, nmax, tol, result, &buf, &LinSolveQR);
    ADVUTILS_FREE(buf.base);
    return status;
}

/* ------------Gauss-Newton sensors calibration with 6 parameters--------------- */
/* approximates Data to a sphere of radius k by calculating 3 gains (s) and 3 biases (b), useful to calibrate some sensors (meas_sphere=S*(meas-B) with S diagonal) */
/* Data has n>=6 rows corresponding to the number of measures and 3 columns corresponding to the 3 axes */
/* X0 is the starting guess vector (usually [0 0 0 1 1 1]), nmax the maximum number of iterations (200 is generally fine, even if it usually converges within 10 iterations), and tol the stopping tolerance (1e-6 is usually more than fine) */
/*b1=out(0,0);
 b2=out(1,0);
 b3=out(2,0);
 s11=out(3,0);
 s22=out(4,0);
 s33=out(5,0);*/

utilsStatus_t GaussNewton_Sens_Cal_6(const matrix_t* Data, float k, const matrix_t* X0, uint16_t nmax, float tol, matrix_t* result) {
    matrixBuffer_t buf;
    if (matrixBufferAllocate(&buf, s_GaussNewton_Sens_Cal_6_scratchBufWords(Data)) == UTILS_STATUS_ERROR) {
        return UTILS_STATUS_ERROR;
    }
    utilsStatus_t status = s_GaussNewton_Sens_Cal_6_impl(Data, k, X0, nmax, tol, result, &buf, &LinSolveQR);
    ADVUTILS_FREE(buf.base);
    return status;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION

/* -------------------------LU factorization using Cormen's Method-------------------------------- */
/* factorizes the A matrix as the product of a unit upper triangular matrix U and a lower triangular matrix L */

utilsStatus_t LU_CormenStatic(const matrix_t* A, matrix_t* L, matrix_t* U) {
    ADVUTILS_ASSERT(A->cols == A->rows);
    ADVUTILS_ASSERT(L->rows == A->rows);
    ADVUTILS_ASSERT(L->cols == A->cols);
    ADVUTILS_ASSERT(U->rows == A->cols);
    ADVUTILS_ASSERT(U->cols == A->cols);
    uint16_t words = s_LU_Cormen_scratchBufWords(A);
    float base[words];
    matrixBuffer_t buf = {base, words, 0};
    return s_LU_Cormen_impl(A, L, U, &buf);
}

/* -----------------------LUP factorization using Cormen's Method------------------------------ */
/* factorizes the A matrix as the product of a upper triangular matrix U and a unit lower triangular matrix L */
/* returns the factor that has to be multiplied to the determinant of U in order to obtain the correct value */

int8_t LUP_CormenStatic(const matrix_t* A, matrix_t* L, matrix_t* U, matrix_t* P) {
    ADVUTILS_ASSERT(A->cols == A->rows);
    ADVUTILS_ASSERT(L->rows == A->rows);
    ADVUTILS_ASSERT(L->cols == A->cols);
    ADVUTILS_ASSERT(U->rows == A->cols);
    ADVUTILS_ASSERT(U->cols == A->cols);
    ADVUTILS_ASSERT(P->rows == A->rows);
    ADVUTILS_ASSERT(P->cols == 1U);
    uint16_t words = s_LUP_Cormen_scratchBufWords(A);
    float base[words];
    matrixBuffer_t buf = {base, words, 0};
    return s_LUP_Cormen_impl(A, L, U, P, &buf);
}

/* ------------------QR factorization (Householder) with static allocation---------------------- */
/* economy Householder QR of a tall matrix A (rows >= cols): A = Q*R, with Q rows-by-cols */
/* orthonormal columns and R cols-by-cols upper-triangular. Returns error if rank-deficient */

utilsStatus_t QR_HouseholderStatic(const matrix_t* A, matrix_t* Q, matrix_t* R) {
    ADVUTILS_ASSERT(A->rows >= A->cols);
    ADVUTILS_ASSERT(Q->rows == A->rows);
    ADVUTILS_ASSERT(Q->cols == A->cols);
    ADVUTILS_ASSERT(R->rows == A->cols);
    ADVUTILS_ASSERT(R->cols == A->cols);
    uint16_t words = s_QR_Householder_scratchBufWords(A);
    float base[words];
    matrixBuffer_t buf = {base, words, 0};
    return s_QR_Householder_impl(A, Q, R, &buf);
}

/* -----------------------Linear system solver using LU factorization--------------------------- */
/* solves the linear system A*X=B, where A is a n-by-n matrix and B an n-by-m matrix, giving the n-by-m matrix X */

utilsStatus_t LinSolveLUStatic(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    uint16_t words = s_LinSolveLU_scratchBufWords(A);
    float base[words];
    matrixBuffer_t buf = {base, words, 0};
    return s_LinSolveLU_impl(A, B, result, &buf);
}

/* ----------------------Linear system solver using LUP factorization-------------------------- */
/* solves the linear system A*X=B, where A is a n-by-n matrix and B an n-by-m matrix, giving the n-by-m matrix X */

utilsStatus_t LinSolveLUPStatic(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    uint16_t words = s_LinSolveLUP_scratchBufWords(A, B);
    float base[words];
    matrixBuffer_t buf = {base, words, 0};
    return s_LinSolveLUP_impl(A, B, result, &buf);
}

/* ------------Linear system solver using Gauss elimination with partial pivoting--------------- */
/* solves the linear system A*X=B, where A is a n-by-n matrix and B an n-by-m matrix, giving the n-by-m matrix X */

utilsStatus_t LinSolveGaussStatic(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    uint16_t words = s_LinSolveGauss_scratchBufWords(A, B);
    float base[words];
    matrixBuffer_t buf = {base, words, 0};
    return s_LinSolveGauss_impl(A, B, result, &buf);
}

/* ----------------------Linear system solver using Cholesky factorization with static allocation-------------------------- */
/* solves the linear system A*X=B for symmetric positive-definite A, giving the n-by-m matrix X */

utilsStatus_t LinSolveCholeskyStatic(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    uint16_t words = s_LinSolveCholesky_scratchBufWords(A, B);
    float base[words];
    matrixBuffer_t buf = {base, words, 0};
    return s_LinSolveCholesky_impl(A, B, result, &buf);
}

/* ----------------------Linear system solver using QR factorization with static allocation-------------------------- */
/* solves A*X=B (least-squares when rows>cols) via economy Householder QR: X = R\(Q'*B) */

utilsStatus_t LinSolveQRStatic(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows >= A->cols);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->rows == A->cols);
    ADVUTILS_ASSERT(result->cols == B->cols);
    uint16_t words = s_LinSolveQR_scratchBufWords(A, B);
    float base[words];
    matrixBuffer_t buf = {base, words, 0};
    return s_LinSolveQR_impl(A, B, result, &buf);
}

/* -------Iterative solver for discrete-time algebraic Riccati equation--------- */
/* Solves discrete-time algebraic Riccati equation P = A'*P*A-(B'*P*A)'*inv(R+B'*P*B)*B'*P*A+Q */
utilsStatus_t DAREStatic(const matrix_t* A, const matrix_t* B, const matrix_t* Q, const matrix_t* R, uint16_t nmax, float tol, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(R->rows == R->cols);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(R->rows == B->cols);
    ADVUTILS_ASSERT(Q->rows == A->rows);
    ADVUTILS_ASSERT(Q->rows == Q->cols);
    ADVUTILS_ASSERT(result->rows == result->cols);
    ADVUTILS_ASSERT(result->rows == A->cols);
    uint16_t words = s_DARE_scratchBufWords(A, B, R);
    float base[words];
    matrixBuffer_t buf = {base, words, 0};
    return s_DARE_impl(A, B, Q, R, nmax, tol, result, &buf, &matrixInversedStatic_rob);
}

/* ------------Gauss-Newton sensors calibration with 9 parameters--------------- */
/* approximates Data to a sphere of radius k by calculating 6 gains (s) and 3 biases (b), useful to calibrate some sensors (meas_sphere=S*(meas-B) with S symmetric) */
/* Data has n>=9 rows corresponding to the number of measures and 3 columns corresponding to the 3 axes */
/* X0 is the starting guess vector (usually [0 0 0 1 0 0 1 0 1]), nmax the maximum number of iterations (200 is generally fine, even if it usually converges within 10 iterations), and tol the stopping tolerance (1e-6 is usually more than fine) */
/*b1=out(0,0);
 b2=out(1,0);
 b3=out(2,0);
 s11=out(3,0);
 s12=out(4,0);
 s13=out(5,0);
 s22=out(6,0);
 s23=out(7,0);
 s33=out(8,0);*/

utilsStatus_t GaussNewton_Sens_Cal_9Static(const matrix_t* Data, float k, const matrix_t* X0, uint16_t nmax, float tol, matrix_t* result) {

    uint16_t words = s_GaussNewton_Sens_Cal_9_scratchBufWords(Data);
    float base[words];
    matrixBuffer_t buf = {base, words, 0};
    return s_GaussNewton_Sens_Cal_9_impl(Data, k, X0, nmax, tol, result, &buf, &LinSolveQRStatic);
}

/* ------------Gauss-Newton sensors calibration with 6 parameters--------------- */
/* approximates Data to a sphere of radius k by calculating 3 gains (s) and 3 biases (b), useful to calibrate some sensors (meas_sphere=S*(meas-B) with S diagonal) */
/* Data has n>=6 rows corresponding to the number of measures and 3 columns corresponding to the 3 axes */
/* X0 is the starting guess vector (usually [0 0 0 1 1 1]), nmax the maximum number of iterations (200 is generally fine, even if it usually converges within 10 iterations), and tol the stopping tolerance (1e-6 is usually more than fine) */
/*b1=out(0,0);
 b2=out(1,0);
 b3=out(2,0);
 s11=out(3,0);
 s22=out(4,0);
 s33=out(5,0);*/

utilsStatus_t GaussNewton_Sens_Cal_6Static(const matrix_t* Data, float k, const matrix_t* X0, uint16_t nmax, float tol, matrix_t* result) {

    uint16_t words = s_GaussNewton_Sens_Cal_6_scratchBufWords(Data);
    float base[words];
    matrixBuffer_t buf = {base, words, 0};
    return s_GaussNewton_Sens_Cal_6_impl(Data, k, X0, nmax, tol, result, &buf, &LinSolveQRStatic);
}

#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

/* Private Functions ---------------------------------------------------------*/

static uint32_t s_LU_Cormen_scratchBufWords(const matrix_t* A) { return (uint32_t)(A->rows) * (A->cols); }

static uint32_t s_LUP_Cormen_scratchBufWords(const matrix_t* A) { return (uint32_t)(A->rows) * (A->cols); }

static uint32_t s_QR_Householder_scratchBufWords(const matrix_t* A) {
    return ((uint32_t)(A->rows) * (A->cols)) + ((uint32_t)(A->rows) * (A->rows)) + (uint32_t)(A->rows);
}

static uint32_t s_LinSolveLU_scratchBufWords(const matrix_t* A) {
    return ((uint32_t)(A->rows) * (A->cols)) + ((uint32_t)(A->cols) * (A->cols)) + ((uint32_t)(A->rows) * (A->cols));
}

static uint32_t s_LinSolveLUP_scratchBufWords(const matrix_t* A, const matrix_t* B) {
    return ((uint32_t)(A->rows) * (A->cols)) + ((uint32_t)(A->cols) * (A->cols)) + (uint32_t)(A->rows) + ((uint32_t)(A->rows) * (B->cols))
           + ((uint32_t)(A->rows) * (A->cols));
}

static uint32_t s_LinSolveGauss_scratchBufWords(const matrix_t* A, const matrix_t* B) {
    return ((uint32_t)(A->rows) * (A->cols)) + ((uint32_t)(B->rows) * (B->cols));
}

static uint32_t s_LinSolveCholesky_scratchBufWords(const matrix_t* A, const matrix_t* B) {
    return ((uint32_t)(A->rows) * (A->cols)) + ((uint32_t)(A->cols) * (A->rows)) + ((uint32_t)(A->rows) * (B->cols));
}

static uint32_t s_LinSolveQR_scratchBufWords(const matrix_t* A, const matrix_t* B) {
    return ((uint32_t)(A->rows) * (A->cols)) + ((uint32_t)(A->cols) * (A->cols)) + ((uint32_t)(A->cols) * (B->cols)) + ((uint32_t)(A->rows) * (A->cols))
           + ((uint32_t)(A->rows) * (A->rows)) + (uint32_t)(A->rows);
}

static uint32_t s_DARE_scratchBufWords(const matrix_t* A, const matrix_t* B, const matrix_t* R) {
    return ((uint32_t)(A->rows) * (A->cols)) + ((uint32_t)(A->rows) * (A->cols)) + ((uint32_t)(B->rows) * (B->rows)) + ((uint32_t)(A->rows) * (A->cols))
           + ((uint32_t)(R->rows) * (R->cols)) + ((uint32_t)(A->rows) * (A->cols)) + ((uint32_t)(A->rows) * (A->cols));
}

static uint32_t s_GaussNewton_Sens_Cal_9_scratchBufWords(const matrix_t* Data) {
    return ((uint32_t)(Data->rows) * (uint32_t)9U) + (uint32_t)(Data->rows) + (uint32_t)9U;
}

static uint32_t s_GaussNewton_Sens_Cal_6_scratchBufWords(const matrix_t* Data) {
    return ((uint32_t)(Data->rows) * (uint32_t)6U) + (uint32_t)(Data->rows) + (uint32_t)6U;
}

/**
 * \brief           Allocation-agnostic core shared by LU_Cormen() and LU_CormenStatic().
 *
 *                  All working matrices are supplied already initialized, so this routine
 *                  performs no allocation; the wrappers own memory management.
 *
 * \param[in]       A: pointer to A matrix object
 * \param[out]      L: pointer to L matrix object
 * \param[out]      U: pointer to U matrix object
 * \param[in]       A_cp: pre-initialized scratch matrix, size [A->rows x A->cols]
 *
 * \return          utilsStatus_t as returned by LU_Cormen()
 */
static utilsStatus_t s_LU_Cormen_impl(const matrix_t* A, matrix_t* L, matrix_t* U, matrixBuffer_t* buf) {
    matrix_t A_cp;
    matrixInitInBuffer(&A_cp, buf, A->rows, A->cols);

    matrixCopy(A, &A_cp);
    matrixZeros(U);
    matrixIdentity(L);

    for (uint8_t i = 0; i < A_cp.rows; i++) {
        ELEMP(U, i, i) = ELEM(A_cp, i, i);
        if ((ELEM(A_cp, i, i) == 0) || (isfinite(ELEM(A_cp, i, i)) == 0)) {
            return UTILS_STATUS_ERROR;
        }
        float tmp = 1.0f / ELEMP(U, i, i);
        for (uint8_t j = (uint8_t)(i + 1U); j < A_cp.rows; j++) {
            ELEMP(L, j, i) = ELEM(A_cp, j, i) * tmp;
            ELEMP(U, i, j) = ELEM(A_cp, i, j);
        }
        for (uint8_t j = (uint8_t)(i + 1U); j < A_cp.rows; j++) {
            for (uint8_t k = (uint8_t)(i + 1U); k < A_cp.rows; k++) {
                ELEM(A_cp, j, k) -= ELEMP(L, j, i) * ELEMP(U, i, k);
            }
        }
    }
    return UTILS_STATUS_SUCCESS;
}

/**
 * \brief           Allocation-agnostic core shared by LUP_Cormen() and LUP_CormenStatic().
 *
 *                  All working matrices are supplied already initialized, so this routine
 *                  performs no allocation; the wrappers own memory management.
 *
 * \param[in]       A: pointer to A matrix object
 * \param[out]      L: pointer to L matrix object
 * \param[out]      U: pointer to U matrix object
 * \param[out]      P: pointer to P matrix object
 * \param[in]       A_cp: pre-initialized scratch matrix, size [A->rows x A->cols]
 *
 * \return          determinant sign factor, or 0 if singular
 */
static int8_t s_LUP_Cormen_impl(const matrix_t* A, matrix_t* L, matrix_t* U, matrix_t* P, matrixBuffer_t* buf) {
    matrix_t A_cp;
    matrixInitInBuffer(&A_cp, buf, A->rows, A->cols);

    int8_t d_mult = 1; /* determinant multiplying factor */
    matrixCopy(A, &A_cp);
    matrixZeros(U);
    matrixIdentity(L);
    /* initialization */
    for (uint8_t i = 0; i < A_cp.rows; i++) {
        ELEMP(P, i, 0) = i;
    }

    /* outer loop over diagonal pivots */
    for (uint8_t i = 0; (int32_t)i < ((int32_t)A_cp.rows - 1); i++) {

        /* inner loop to find the largest pivot */
        uint8_t pivrow = (uint8_t)i;
        float tmp = fabsf(ELEM(A_cp, i, i));
        for (uint8_t j = (uint8_t)(i + 1U); j < A_cp.rows; j++) {
            float tmp2 = fabsf(ELEM(A_cp, j, i));
            if (tmp2 > tmp) {
                tmp = tmp2;
                pivrow = (uint8_t)j;
            }
        }
        /* check for singularity */
        if ((ELEM(A_cp, pivrow, i) == 0) || (isfinite(ELEM(A_cp, pivrow, i)) == 0)) {
            return 0;
        }

        /* swap rows */
        if (pivrow != i) {
            tmp = ELEMP(P, i, 0);
            ELEMP(P, i, 0) = ELEMP(P, pivrow, 0);
            ELEMP(P, pivrow, 0) = tmp;
            d_mult *= -1;

            for (uint8_t j = 0; j < A_cp.rows; j++) {
                tmp = ELEM(A_cp, i, j);
                ELEM(A_cp, i, j) = ELEM(A_cp, pivrow, j);
                ELEM(A_cp, pivrow, j) = tmp;
            }
        }
        tmp = 1.0f / ELEM(A_cp, i, i);
        /* Gaussian elimination */
        for (uint8_t j = (uint8_t)(i + 1U); j < A_cp.rows; j++) { /* iterate down rows */
            ELEM(A_cp, j, i) *= tmp;
            for (uint8_t k = (uint8_t)(i + 1U); k < A_cp.rows; k++) { /* iterate across rows */
                ELEM(A_cp, j, k) -= ELEM(A_cp, j, i) * ELEM(A_cp, i, k);
            }
        }
    }
    for (uint8_t i = 0; i < A_cp.rows; i++) {
        ELEMP(U, i, i) = ELEM(A_cp, i, i);
        for (uint8_t j = (uint8_t)(i + 1U); j < A_cp.rows; j++) {
            ELEMP(L, j, i) = ELEM(A_cp, j, i);
            ELEMP(U, i, j) = ELEM(A_cp, i, j);
        }
    }
    return d_mult;
}

/**
 * \brief           Allocation-agnostic core shared by QR_Householder() and QR_HouseholderStatic().
 *
 *                  All working matrices are supplied already initialized, so this routine
 *                  performs no allocation; the wrappers own memory management.
 *
 * \param[in]       A: pointer to A matrix object
 * \param[out]      Q: pointer to Q matrix object
 * \param[out]      R: pointer to R matrix object
 * \param[in]       Rw: pre-initialized scratch matrix, size [m x n]
 * \param[in]       Qf: pre-initialized scratch matrix, size [m x m]
 * \param[in]       v: pre-initialized scratch matrix, size [m x 1]
 *
 * \return          utilsStatus_t as returned by QR_Householder()
 */
static utilsStatus_t s_QR_Householder_impl(const matrix_t* A, matrix_t* Q, matrix_t* R, matrixBuffer_t* buf) {
    matrix_t Rw;
    matrix_t Qf;
    matrix_t v;
    matrixInitInBuffer(&Rw, buf, (A->rows), (A->cols));
    matrixInitInBuffer(&Qf, buf, (A->rows), (A->rows));
    matrixInitInBuffer(&v, buf, (A->rows), 1);

    uint8_t m = A->rows;
    uint8_t n = A->cols;
    utilsStatus_t status = UTILS_STATUS_SUCCESS;
    matrixCopy(A, &Rw);
    matrixIdentity(&Qf);
    for (uint8_t k = 0; k < n; k++) {
        float norm = 0.0f;
        for (uint8_t i = k; i < m; i++) {
            norm += ELEM(Rw, i, k) * ELEM(Rw, i, k);
        }
        norm = SQRT(norm);
        float alpha = (ELEM(Rw, k, k) >= 0.0f) ? -norm : norm;
        ELEM(v, k, 0) = ELEM(Rw, k, k) - alpha;
        for (uint8_t i = (uint8_t)(k + 1U); i < m; i++) {
            ELEM(v, i, 0) = ELEM(Rw, i, k);
        }
        float vtv = 0.0f;
        for (uint8_t i = k; i < m; i++) {
            vtv += ELEM(v, i, 0) * ELEM(v, i, 0);
        }
        if (vtv > 1e-30f) {
            float beta = 2.0f / vtv;
            for (uint8_t j = k; j < n; j++) {
                float s = 0.0f;
                for (uint8_t i = k; i < m; i++) {
                    s += ELEM(v, i, 0) * ELEM(Rw, i, j);
                }
                s *= beta;
                for (uint8_t i = k; i < m; i++) {
                    ELEM(Rw, i, j) -= s * ELEM(v, i, 0);
                }
            }
            for (uint8_t i = 0; i < m; i++) {
                float s = 0.0f;
                for (uint8_t l = k; l < m; l++) {
                    s += ELEM(Qf, i, l) * ELEM(v, l, 0);
                }
                s *= beta;
                for (uint8_t l = k; l < m; l++) {
                    ELEM(Qf, i, l) -= s * ELEM(v, l, 0);
                }
            }
        }
    }
    float r00 = fabsf(ELEM(Rw, 0, 0));
    if ((r00 < 1e-20f) || (isfinite(r00) == 0)) {
        status = UTILS_STATUS_ERROR;
    }
    for (uint8_t k = 0; (k < n) && (status == UTILS_STATUS_SUCCESS); k++) {
        if (fabsf(ELEM(Rw, k, k)) < (1e-6f * r00)) {
            status = UTILS_STATUS_ERROR;
        }
    }
    if (status == UTILS_STATUS_SUCCESS) {
        for (uint8_t i = 0; i < m; i++) {
            for (uint8_t j = 0; j < n; j++) {
                ELEMP(Q, i, j) = ELEM(Qf, i, j);
            }
        }
        for (uint8_t i = 0; i < n; i++) {
            for (uint8_t j = 0; j < n; j++) {
                ELEMP(R, i, j) = (j >= i) ? ELEM(Rw, i, j) : 0.0f;
            }
        }
        if ((isMatrixFinite(Q) == 0U) || (isMatrixFinite(R) == 0U)) {
            status = UTILS_STATUS_ERROR;
        }
    }
    return status;
}

/**
 * \brief           Allocation-agnostic core shared by LinSolveLU() and LinSolveLUStatic().
 *
 *                  All working matrices are supplied already initialized, so this routine
 *                  performs no allocation; the wrappers own memory management.
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[out]      result: pointer to result matrix object
 * \param[in]       L: pre-initialized scratch matrix, size [A->rows x A->cols]
 * \param[in]       U: pre-initialized scratch matrix, size [A->cols x A->cols]
 * \param[in]       factor: pointer to the factorization routine to use (dynamic or static variant)
 *
 * \return          utilsStatus_t as returned by LinSolveLU()
 */
static utilsStatus_t s_LinSolveLU_impl(const matrix_t* A, const matrix_t* B, matrix_t* result, matrixBuffer_t* buf) {
    matrix_t L;
    matrix_t U;
    matrixInitInBuffer(&L, buf, A->rows, A->cols);
    matrixInitInBuffer(&U, buf, A->cols, A->cols);

    utilsStatus_t status;
    status = s_LU_Cormen_impl(A, &L, &U, buf);
    if (status == UTILS_STATUS_SUCCESS) {
        fwsub(&L, B, result);
        bksub(&U, result, result); /* hope it can work in-place */
        if (isMatrixFinite(result) == 0U) {
            status = UTILS_STATUS_ERROR;
            matrixZeros(result);
        }
    } else {
        matrixZeros(result);
    }
    return status;
}

/**
 * \brief           Allocation-agnostic core shared by LinSolveLUP() and LinSolveLUPStatic().
 *
 *                  All working matrices are supplied already initialized, so this routine
 *                  performs no allocation; the wrappers own memory management.
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[out]      result: pointer to result matrix object
 * \param[in]       L: pre-initialized scratch matrix, size [A->rows x A->cols]
 * \param[in]       U: pre-initialized scratch matrix, size [A->cols x A->cols]
 * \param[in]       P: pre-initialized scratch matrix, size [A->rows x 1]
 * \param[in]       tmp: pre-initialized scratch matrix, size [A->rows x B->cols]
 * \param[in]       factor: pointer to the factorization routine to use (dynamic or static variant)
 *
 * \return          utilsStatus_t as returned by LinSolveLUP()
 */
static utilsStatus_t s_LinSolveLUP_impl(const matrix_t* A, const matrix_t* B, matrix_t* result, matrixBuffer_t* buf) {
    matrix_t L;
    matrix_t U;
    matrix_t P;
    matrix_t tmp;
    matrixInitInBuffer(&L, buf, A->rows, A->cols);
    matrixInitInBuffer(&U, buf, A->cols, A->cols);
    matrixInitInBuffer(&P, buf, A->rows, 1);
    matrixInitInBuffer(&tmp, buf, A->rows, B->cols);

    utilsStatus_t status = UTILS_STATUS_SUCCESS;
    if (s_LUP_Cormen_impl(A, &L, &U, &P, buf) == 0) {
        status = UTILS_STATUS_ERROR; /* singular matrix */
    }
    if (status == UTILS_STATUS_SUCCESS) {
        fwsubPerm(&L, B, &P, &tmp);
        bksub(&U, &tmp, result);
        if (isMatrixFinite(result) == 0U) {
            status = UTILS_STATUS_ERROR;
            matrixZeros(result);
        }
    } else {
        matrixZeros(result);
    }
    return status;
}

/**
 * \brief           Allocation-agnostic core shared by LinSolveGauss() and LinSolveGaussStatic().
 *
 *                  All working matrices are supplied already initialized, so this routine
 *                  performs no allocation; the wrappers own memory management.
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[out]      result: pointer to result matrix object
 * \param[in]       A_cp: pre-initialized scratch matrix, size [A->rows x A->cols]
 * \param[in]       B_cp: pre-initialized scratch matrix, size [B->rows x B->cols]
 *
 * \return          utilsStatus_t as returned by LinSolveGauss()
 */
static utilsStatus_t s_LinSolveGauss_impl(const matrix_t* A, const matrix_t* B, matrix_t* result, matrixBuffer_t* buf) {
    matrix_t A_cp;
    matrix_t B_cp;
    matrixInitInBuffer(&A_cp, buf, A->rows, A->cols);
    matrixInitInBuffer(&B_cp, buf, B->rows, B->cols);

    utilsStatus_t status = UTILS_STATUS_SUCCESS;
    matrixCopy(A, &A_cp);
    matrixCopy(B, &B_cp);

    for (uint8_t i = 0; (int32_t)i < ((int32_t)A_cp.cols - 1); i++) {

        /* find pivot row, the row with biggest entry in current column */
        float tmp = fabsf(ELEM(A_cp, i, i));
        uint8_t pivrow = (uint8_t)i;
        for (uint8_t j = (uint8_t)(i + 1U); j < A_cp.cols; j++) {
            float tmp2 = fabsf(ELEM(A_cp, j, i)); /* 'Avoid using other functions inside abs()?' */
            if (tmp2 > tmp) {
                tmp = tmp2;
                pivrow = (uint8_t)j;
            }
        }

        /* check for singular or non-finite pivot */
        if ((ELEM(A_cp, pivrow, i) == 0.0f) || (isfinite(ELEM(A_cp, pivrow, i)) == 0)) {
            status = UTILS_STATUS_ERROR;
            break;
        }

        /* Execute pivot (row swap) if needed */
        if (pivrow != i) {
            /* swap row k of matrix A with pivrow */
            for (uint8_t j = i; j < A_cp.cols; j++) {
                tmp = ELEM(A_cp, i, j);
                ELEM(A_cp, i, j) = ELEM(A_cp, pivrow, j);
                ELEM(A_cp, pivrow, j) = tmp;
            }
            /* swap row k of matrix B with pivrow */
            for (uint8_t j = 0; j < B_cp.cols; j++) {
                tmp = ELEM(B_cp, i, j);
                ELEM(B_cp, i, j) = ELEM(B_cp, pivrow, j);
                ELEM(B_cp, pivrow, j) = tmp;
            }
        }

        /* Row reduction */
        tmp = 1.0f / ELEM(A_cp, i, i);                            /* invert pivot element */
        for (uint8_t j = (uint8_t)(i + 1U); j < A_cp.cols; j++) { /* along rows */
            float tmp2 = ELEM(A_cp, j, i) * tmp;
            /* Perform row reduction of A */
            for (uint8_t k = (uint8_t)(i + 1U); k < A_cp.cols; k++) { /* along columns of A */
                ELEM(A_cp, j, k) -= tmp2 * ELEM(A_cp, i, k);
            }
            /* Perform row reduction of B */
            for (uint8_t k = 0; k < B_cp.cols; k++) { /* along columns of B */
                ELEM(B_cp, j, k) -= tmp2 * ELEM(B_cp, i, k);
            }
        }
    }
    if (status == UTILS_STATUS_SUCCESS) {
        bksub(&A_cp, &B_cp, result);
        if (isMatrixFinite(result) == 0U) {
            status = UTILS_STATUS_ERROR;
            matrixZeros(result);
        }
    } else {
        matrixZeros(result);
    }
    return status;
}

/**
 * \brief           Allocation-agnostic core shared by LinSolveCholesky() and LinSolveCholeskyStatic().
 *
 *                  All working matrices are supplied already initialized, so this routine
 *                  performs no allocation; the wrappers own memory management.
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[out]      result: pointer to result matrix object
 * \param[in]       L: pre-initialized scratch matrix, size [A->rows x A->cols]
 * \param[in]       Lt: pre-initialized scratch matrix, size [A->cols x A->rows]
 * \param[in]       Y: pre-initialized scratch matrix, size [A->rows x B->cols]
 *
 * \return          utilsStatus_t as returned by LinSolveCholesky()
 */
static utilsStatus_t s_LinSolveCholesky_impl(const matrix_t* A, const matrix_t* B, matrix_t* result, matrixBuffer_t* buf) {
    matrix_t L;
    matrix_t Lt;
    matrix_t Y;
    matrixInitInBuffer(&L, buf, A->rows, A->cols);
    matrixInitInBuffer(&Lt, buf, A->cols, A->rows);
    matrixInitInBuffer(&Y, buf, A->rows, B->cols);

    utilsStatus_t status;
    status = Cholesky(A, &L);
    if (status == UTILS_STATUS_SUCCESS) {
        matrixTrans(&L, &Lt);
        fwsub(&L, B, &Y);
        bksub(&Lt, &Y, result);
        if (isMatrixFinite(result) == 0U) {
            status = UTILS_STATUS_ERROR;
            matrixZeros(result);
        }
    } else {
        matrixZeros(result); /* defined (zero) output for rank-deficient/failed solve */
    }
    return status;
}

/**
 * \brief           Allocation-agnostic core shared by LinSolveQR() and LinSolveQRStatic().
 *
 *                  All working matrices are supplied already initialized, so this routine
 *                  performs no allocation; the wrappers own memory management.
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[out]      result: pointer to result matrix object
 * \param[in]       Q: pre-initialized scratch matrix, size [A->rows x A->cols]
 * \param[in]       R: pre-initialized scratch matrix, size [A->cols x A->cols]
 * \param[in]       QtB: pre-initialized scratch matrix, size [A->cols x B->cols]
 * \param[in]       factor: pointer to the factorization routine to use (dynamic or static variant)
 *
 * \return          utilsStatus_t as returned by LinSolveQR()
 */
static utilsStatus_t s_LinSolveQR_impl(const matrix_t* A, const matrix_t* B, matrix_t* result, matrixBuffer_t* buf) {
    matrix_t Q;
    matrix_t R;
    matrix_t QtB;
    matrixInitInBuffer(&Q, buf, A->rows, A->cols);
    matrixInitInBuffer(&R, buf, A->cols, A->cols);
    matrixInitInBuffer(&QtB, buf, A->cols, B->cols);

    utilsStatus_t status;
    status = s_QR_Householder_impl(A, &Q, &R, buf);
    if (status == UTILS_STATUS_SUCCESS) {
        matrixMult_lhsT(&Q, B, &QtB);
        bksub(&R, &QtB, result);
        if (isMatrixFinite(result) == 0U) {
            status = UTILS_STATUS_ERROR;
            matrixZeros(result);
        }
    } else {
        matrixZeros(result); /* defined (zero) output for rank-deficient/failed solve */
    }
    return status;
}

/**
 * \brief           Allocation-agnostic core shared by DARE() and DAREStatic().
 *
 *                  All working matrices are supplied already initialized, so this routine
 *                  performs no allocation; the wrappers own memory management.
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[in]       Q: pointer to Q matrix object
 * \param[in]       R: pointer to R matrix object
 * \param[in]       nmax: see DARE()
 * \param[in]       tol: see DARE()
 * \param[out]      result: pointer to result matrix object
 * \param[in]       _Ak: pre-initialized scratch matrix, size [A->rows x A->cols]
 * \param[in]       _Ak1: pre-initialized scratch matrix, size [A->rows x A->cols]
 * \param[in]       _G: pre-initialized scratch matrix, size [B->rows x B->rows]
 * \param[in]       _IGP: pre-initialized scratch matrix, size [A->rows x A->cols]
 * \param[in]       tmp1: pre-initialized scratch matrix, size [R->rows x R->cols]
 * \param[in]       tmp2: pre-initialized scratch matrix, size [A->rows x A->cols]
 * \param[in]       tmp3: pre-initialized scratch matrix, size [A->rows x A->cols]
 * \param[in]       inverse: pointer to the matrix-inversion routine to use (dynamic or static variant)
 *
 * \return          utilsStatus_t as returned by DARE()
 */
static utilsStatus_t s_DARE_impl(const matrix_t* A, const matrix_t* B, const matrix_t* Q, const matrix_t* R, uint16_t nmax, float tol, matrix_t* result,
                                 matrixBuffer_t* buf, s_inverse_fn inverse) {
    matrix_t _Ak;
    matrix_t _Ak1;
    matrix_t _G;
    matrix_t _IGP;
    matrix_t tmp1;
    matrix_t tmp2;
    matrix_t tmp3;
    matrixInitInBuffer(&_Ak, buf, A->rows, A->cols);
    matrixInitInBuffer(&_Ak1, buf, A->rows, A->cols);
    matrixInitInBuffer(&_G, buf, B->rows, B->rows);
    matrixInitInBuffer(&_IGP, buf, A->rows, A->cols);
    matrixInitInBuffer(&tmp1, buf, R->rows, R->cols);
    matrixInitInBuffer(&tmp2, buf, A->rows, A->cols);
    matrixInitInBuffer(&tmp3, buf, A->rows, A->cols);

    matrixCopy(A, &_Ak);
    (void)inverse(R, &tmp1);
    QuadProd(B, &tmp1, &_G);
    matrixCopy(Q, result);

    uint16_t nIter = nmax;
    while (nIter-- > 0U) {
        /* Calculation of inverse(I+G*H); */
        matrixMult(&_G, result, &tmp2);
        for (uint8_t i = 0; i < tmp2.rows; i++) {
            ELEM(tmp2, i, i) += 1.f;
        }
        (void)inverse(&tmp2, &_IGP);
        /* Calculation of Ak1 = Ak*inverse(I+G*H)*Ak */
        matrixMult(&_Ak, &_IGP, &tmp2);
        matrixMult(&tmp2, &_Ak, &_Ak1);
        /* Calculation of Gk1 = Gk + Ak*inverse(eye(4)+Gk*H)*Gk*Ak.' */
        matrixMult(&tmp2, &_G, &tmp3);
        matrixMult_rhsT(&tmp3, &_Ak, &tmp2);
        matrixAdd(&_G, &tmp2, &_G);
        /* Calculation of H = H + Ak.'*H*inverse(eye(4)+Gk*H)*Ak */
        matrixMult_lhsT(&_Ak, result, &tmp2);
        matrixMult(&tmp2, &_IGP, &tmp3);
        matrixMult(&tmp3, &_Ak, &tmp2);
        matrixAdd(result, &tmp2, result);
        if ((matrixNorm(&tmp2) / matrixNorm(result)) < tol) {
            /* Delete temporary matrices */
            return UTILS_STATUS_SUCCESS;
        }
        matrixCopy(&_Ak1, &_Ak);
    }

    /* Delete temporary matrices */
    return UTILS_STATUS_TIMEOUT;
}

/**
 * \brief           Allocation-agnostic core shared by GaussNewton_Sens_Cal_9() and GaussNewton_Sens_Cal_9Static().
 *
 *                  All working matrices are supplied already initialized, so this routine
 *                  performs no allocation; the wrappers own memory management.
 *
 * \param[in]       Data: pointer to Data matrix object
 * \param[in]       k: see GaussNewton_Sens_Cal_9()
 * \param[in]       X0: pointer to X0 matrix object
 * \param[in]       nmax: see GaussNewton_Sens_Cal_9()
 * \param[in]       tol: see GaussNewton_Sens_Cal_9()
 * \param[out]      result: pointer to result matrix object
 * \param[in]       Jr: pre-initialized scratch matrix, size [Data->rows x 9]
 * \param[in]       res: pre-initialized scratch matrix, size [Data->rows x 1]
 * \param[in]       delta: pre-initialized scratch matrix, size [9 x 1]
 * \param[in]       linSolve: pointer to the linear-solve routine to use (dynamic or static variant)
 *
 * \return          utilsStatus_t as returned by GaussNewton_Sens_Cal_9()
 */
static utilsStatus_t s_GaussNewton_Sens_Cal_9_impl(const matrix_t* Data, float k, const matrix_t* X0, uint16_t nmax, float tol, matrix_t* result,
                                                   matrixBuffer_t* buf, s_linSolveQR_fn linSolve) {
    matrix_t Jr;
    matrix_t res;
    matrix_t delta;
    matrixInitInBuffer(&Jr, buf, Data->rows, 9);
    matrixInitInBuffer(&res, buf, Data->rows, 1);
    matrixInitInBuffer(&delta, buf, 9, 1);

    float k2;
    if ((Data->rows < 9U) || (Data->cols != 3U)) {
        return UTILS_STATUS_ERROR;
    }

    /* Set starting point if not given as input */
    if (X0 != NULL) {
        matrixCopy(X0, result);
    } else {
        matrixZeros(result);
        for (uint8_t i = 0; i < Data->rows; i++) {
            ELEMP(result, 0, 0) += matrixGet(Data, i, 0) / Data->rows;
            ELEMP(result, 1, 0) += matrixGet(Data, i, 1) / Data->rows;
            ELEMP(result, 2, 0) += matrixGet(Data, i, 2) / Data->rows;
        }
        matrixSet(result, 3, 0, 1);
        matrixSet(result, 6, 0, 1);
        matrixSet(result, 8, 0, 1);
    }

    /* Set target radius if not given as input */
    if (k != 0.0f) {
        k2 = k * k;
    } else {
        float max = matrixGet(Data, 0, 0) - matrixGet(result, 0, 0);
        float min = matrixGet(Data, 0, 0) - matrixGet(result, 0, 0);
        for (uint8_t i = 0; i < Data->rows; i++) {
            for (uint8_t j = 0; j < 3U; j++) {
                float data = matrixGet(Data, i, j) - matrixGet(result, j, 0);
                if (data > max) {
                    max = data;
                } else if (data < min) {
                    min = data;
                } else {
                    /* no action required (MISRA 15.7) */
                }
            }
        }
        k2 = 0.25f * (max - min) * (max - min);
    }

    /* Perform best-fit algorithm */
    for (uint16_t n_iter = 0; n_iter < nmax; n_iter++) {
        for (uint8_t i = 0; i < Data->rows; i++) {
            float d1 = ELEMP(Data, i, 0) - ELEMP(result, 0, 0);
            float d2 = ELEMP(Data, i, 1) - ELEMP(result, 1, 0);
            float d3 = ELEMP(Data, i, 2) - ELEMP(result, 2, 0);
            float rx1 = -2.0f * ((ELEMP(result, 3, 0) * d1) + (ELEMP(result, 4, 0) * d2) + (ELEMP(result, 5, 0) * d3));
            float rx2 = -2.0f * ((ELEMP(result, 4, 0) * d1) + (ELEMP(result, 6, 0) * d2) + (ELEMP(result, 7, 0) * d3));
            float rx3 = -2.0f * ((ELEMP(result, 5, 0) * d1) + (ELEMP(result, 7, 0) * d2) + (ELEMP(result, 8, 0) * d3));
            ELEM(Jr, i, 0) = (ELEMP(result, 3, 0) * rx1) + (ELEMP(result, 4, 0) * rx2) + (ELEMP(result, 5, 0) * rx3);
            ELEM(Jr, i, 1) = (ELEMP(result, 4, 0) * rx1) + (ELEMP(result, 6, 0) * rx2) + (ELEMP(result, 7, 0) * rx3);
            ELEM(Jr, i, 2) = (ELEMP(result, 5, 0) * rx1) + (ELEMP(result, 7, 0) * rx2) + (ELEMP(result, 8, 0) * rx3);
            ELEM(Jr, i, 3) = -d1 * rx1;
            ELEM(Jr, i, 4) = (-d2 * rx1) - (d1 * rx2);
            ELEM(Jr, i, 5) = (-d3 * rx1) - (d1 * rx3);
            ELEM(Jr, i, 6) = -d2 * rx2;
            ELEM(Jr, i, 7) = (-d3 * rx2) - (d2 * rx3);
            ELEM(Jr, i, 8) = -d3 * rx3;
            float t1 = (ELEMP(result, 3, 0) * d1) + (ELEMP(result, 4, 0) * d2) + (ELEMP(result, 5, 0) * d3);
            float t2 = (ELEMP(result, 4, 0) * d1) + (ELEMP(result, 6, 0) * d2) + (ELEMP(result, 7, 0) * d3);
            float t3 = (ELEMP(result, 5, 0) * d1) + (ELEMP(result, 7, 0) * d2) + (ELEMP(result, 8, 0) * d3);
            ELEM(res, i, 0) = (t1 * t1) + (t2 * t2) + (t3 * t3) - k2;
        }
        if (linSolve(&Jr, &res, &delta) == UTILS_STATUS_ERROR) {
            return UTILS_STATUS_ERROR;
        }
        matrixSub(result, &delta, result);
        if (matrixNorm(&delta) < tol) {
            return UTILS_STATUS_SUCCESS;
        }
    }
    return UTILS_STATUS_TIMEOUT;
}

/**
 * \brief           Allocation-agnostic core shared by GaussNewton_Sens_Cal_6() and GaussNewton_Sens_Cal_6Static().
 *
 *                  All working matrices are supplied already initialized, so this routine
 *                  performs no allocation; the wrappers own memory management.
 *
 * \param[in]       Data: pointer to Data matrix object
 * \param[in]       k: see GaussNewton_Sens_Cal_6()
 * \param[in]       X0: pointer to X0 matrix object
 * \param[in]       nmax: see GaussNewton_Sens_Cal_6()
 * \param[in]       tol: see GaussNewton_Sens_Cal_6()
 * \param[out]      result: pointer to result matrix object
 * \param[in]       Jr: pre-initialized scratch matrix, size [Data->rows x 6]
 * \param[in]       res: pre-initialized scratch matrix, size [Data->rows x 1]
 * \param[in]       delta: pre-initialized scratch matrix, size [6 x 1]
 * \param[in]       linSolve: pointer to the linear-solve routine to use (dynamic or static variant)
 *
 * \return          utilsStatus_t as returned by GaussNewton_Sens_Cal_6()
 */
static utilsStatus_t s_GaussNewton_Sens_Cal_6_impl(const matrix_t* Data, float k, const matrix_t* X0, uint16_t nmax, float tol, matrix_t* result,
                                                   matrixBuffer_t* buf, s_linSolveQR_fn linSolve) {
    matrix_t Jr;
    matrix_t res;
    matrix_t delta;
    matrixInitInBuffer(&Jr, buf, Data->rows, 6);
    matrixInitInBuffer(&res, buf, Data->rows, 1);
    matrixInitInBuffer(&delta, buf, 6, 1);

    float k2;
    if ((Data->rows < 6U) || (Data->cols != 3U)) {
        return UTILS_STATUS_ERROR;
    }

    /* Set starting point if not given as input */
    if (X0 != NULL) {
        matrixCopy(X0, result);
    } else {
        matrixZeros(result);
        for (uint8_t i = 0; i < Data->rows; i++) {
            ELEMP(result, 0, 0) += matrixGet(Data, i, 0) / Data->rows;
            ELEMP(result, 1, 0) += matrixGet(Data, i, 1) / Data->rows;
            ELEMP(result, 2, 0) += matrixGet(Data, i, 2) / Data->rows;
        }
        matrixSet(result, 3, 0, 1);
        matrixSet(result, 4, 0, 1);
        matrixSet(result, 5, 0, 1);
    }

    /* Set target radius if not given as input */
    if (k != 0.0f) {
        k2 = k * k;
    } else {
        float max = matrixGet(Data, 0, 0) - matrixGet(result, 0, 0);
        float min = matrixGet(Data, 0, 0) - matrixGet(result, 0, 0);
        for (uint8_t i = 0; i < Data->rows; i++) {
            for (uint8_t j = 0; j < 3U; j++) {
                float data = matrixGet(Data, i, j) - matrixGet(result, j, 0);
                if (data > max) {
                    max = data;
                } else if (data < min) {
                    min = data;
                } else {
                    /* no action required (MISRA 15.7) */
                }
            }
        }
        k2 = 0.25f * (max - min) * (max - min);
    }

    /* Perform best-fit algorithm */
    for (uint16_t n_iter = 0; n_iter < nmax; n_iter++) {
        for (uint8_t i = 0; i < Data->rows; i++) {
            float d1 = ELEMP(Data, i, 0) - ELEMP(result, 0, 0);
            float d2 = ELEMP(Data, i, 1) - ELEMP(result, 1, 0);
            float d3 = ELEMP(Data, i, 2) - ELEMP(result, 2, 0);
            ELEM(Jr, i, 0) = -2.0f * d1 * ELEMP(result, 3, 0) * ELEMP(result, 3, 0);
            ELEM(Jr, i, 1) = -2.0f * d2 * ELEMP(result, 4, 0) * ELEMP(result, 4, 0);
            ELEM(Jr, i, 2) = -2.0f * d3 * ELEMP(result, 5, 0) * ELEMP(result, 5, 0);
            ELEM(Jr, i, 3) = 2 * ELEMP(result, 3, 0) * d1 * d1;
            ELEM(Jr, i, 4) = 2 * ELEMP(result, 4, 0) * d2 * d2;
            ELEM(Jr, i, 5) = 2 * ELEMP(result, 5, 0) * d3 * d3;
            float t1 = ELEMP(result, 3, 0) * d1;
            float t2 = ELEMP(result, 4, 0) * d2;
            float t3 = ELEMP(result, 5, 0) * d3;
            ELEM(res, i, 0) = (t1 * t1) + (t2 * t2) + (t3 * t3) - k2;
        }
        if (linSolve(&Jr, &res, &delta) == UTILS_STATUS_ERROR) {
            return UTILS_STATUS_ERROR;
        }
        matrixSub(result, &delta, result);
        if (matrixNorm(&delta) < tol) {
            return UTILS_STATUS_SUCCESS;
        }
    }
    return UTILS_STATUS_TIMEOUT;
}
