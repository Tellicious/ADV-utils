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
#include "math.h"

/* -------------------Forward substitution---------------------- */
/* assumes that the matrix A is already a lower triangular one. No check! */

void fwsub(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    for (uint8_t i = 0; i < B->cols; i++) {
        ELEMP(result, 0, i) = ELEMP(B, 0, i) / ELEMP(A, 0, 0);
        for (uint8_t j = 1; j < A->rows; j++) {
            float tmp = 0.0;
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
    ADVUTILS_ASSERT(P->cols == 1);
    for (uint8_t i = 0; i < B->cols; i++) {
        ELEMP(result, 0, i) = ELEMP(B, (uint8_t)ELEMP(P, 0, 0), i) / ELEMP(A, 0, 0);
        for (uint8_t j = 1; j < A->rows; j++) {
            float tmp = 0.0;
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
            float tmp = 0.0;
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
    ADVUTILS_ASSERT(P->cols == 1);
    for (uint8_t i = 0; i < B->cols; i++) {
        ELEMP(result, (A->cols - 1U), i) = ELEMP(B, (uint8_t)ELEMP(P, (A->cols - 1U), 0), i) / ELEMP(A, (A->cols - 1U), (A->cols - 1U));
        for (int16_t j = (int16_t)((int32_t)A->rows - 2); j >= 0; j--) {
            float tmp = 0.0;
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
            float tmp = 0.0;
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
            if (ELEMP(L, i, i) == 0) {
                return UTILS_STATUS_ERROR;
            }
            ELEMP(U, i, j) = (ELEMP(A, i, j) - sum) / ELEMP(L, i, i);
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
    matrix_t A_cp;
    (void)matrixInit(&A_cp, A->rows, A->cols);
    matrixCopy(A, &A_cp);
    matrixZeros(U);
    matrixIdentity(L);

    for (uint8_t i = 0; i < A_cp.rows; i++) {
        ELEMP(U, i, i) = ELEM(A_cp, i, i);
        if (ELEM(A_cp, i, i) == 0) {
            (void)matrixDelete(&A_cp);
            return UTILS_STATUS_ERROR;
        }
        float tmp = 1.0 / ELEMP(U, i, i);
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
    (void)matrixDelete(&A_cp);
    return UTILS_STATUS_SUCCESS;
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
    ADVUTILS_ASSERT(P->cols == 1);
    int8_t d_mult = 1; /* determinant multiplying factor */
    matrix_t A_cp;
    (void)matrixInit(&A_cp, A->rows, A->cols);
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
        if (ELEM(A_cp, pivrow, i) == 0) {
            (void)matrixDelete(&A_cp);
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
        tmp = 1.0 / ELEM(A_cp, i, i);
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
    (void)matrixDelete(&A_cp);
    return d_mult;
}

/* -----------------------Linear system solver using LU factorization--------------------------- */
/* solves the linear system A*X=B, where A is a n-by-n matrix and B an n-by-m matrix, giving the n-by-m matrix X */

void LinSolveLU(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    matrix_t L;
    matrix_t U;
    (void)matrixInit(&L, A->rows, A->cols);
    (void)matrixInit(&U, A->cols, A->cols);
    /* matrix_t *tmp1 = matrixInit(A->rows, B->cols); */
    (void)LU_Cormen(A, &L, &U);
    /* fwsub(L, B, tmp1); */
    /* bksub(U, tmp1, result); */
    fwsub(&L, B, result);
    bksub(&U, result, result); /* hope it can work in-place */
    (void)matrixDelete(&L);
    (void)matrixDelete(&U);
    return;
}

/* ----------------------Linear system solver using LUP factorization-------------------------- */
/* solves the linear system A*X=B, where A is a n-by-n matrix and B an n-by-m matrix, giving the n-by-m matrix X */

void LinSolveLUP(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    matrix_t L;
    matrix_t U;
    matrix_t P;
    matrix_t tmp;
    (void)matrixInit(&L, A->rows, A->cols);
    (void)matrixInit(&U, A->cols, A->cols);
    (void)matrixInit(&P, A->rows, 1);
    (void)matrixInit(&tmp, A->rows, B->cols);

    (void)LUP_Cormen(A, &L, &U, &P);
    fwsubPerm(&L, B, &P, &tmp);
    bksub(&U, &tmp, result);
    (void)matrixDelete(&L);
    (void)matrixDelete(&U);
    (void)matrixDelete(&P);
    (void)matrixDelete(&tmp);
    return;
}

/* ------------Linear system solver using Gauss elimination with partial pivoting--------------- */
/* solves the linear system A*X=B, where A is a n-by-n matrix and B an n-by-m matrix, giving the n-by-m matrix X */

void LinSolveGauss(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    matrix_t A_cp;
    matrix_t B_cp;
    (void)matrixInit(&A_cp, A->rows, A->cols);
    (void)matrixInit(&B_cp, B->rows, B->cols);
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

        /* check for singular Matrix */
        if (ELEM(A_cp, pivrow, i) == 0.0) {
            matrixZeros(result);
            (void)matrixDelete(&A_cp);
            (void)matrixDelete(&B_cp);
            return;
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
        tmp = 1.0 / ELEM(A_cp, i, i);                             /* invert pivot element */
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
    bksub(&A_cp, &B_cp, result);
    (void)matrixDelete(&A_cp);
    (void)matrixDelete(&B_cp);
    return;
}

/* -------Iterative solver for discrete-time algebraic Riccati equation--------- */
/* Solves discrete-time algebraic Riccati equation P = A'*P*A-(B'*P*A)'*inv(R+B'*P*B)*B'*P*A+Q */
utilsStatus_t DARE(const matrix_t* A, const matrix_t* B, const matrix_t* Q, const matrix_t* R, uint16_t nmax, float tol, matrix_t* result) {
    matrix_t _Ak;
    matrix_t _G;
    matrix_t _IGP;
    matrix_t _Ak1;
    matrix_t tmp1;
    matrix_t tmp2;
    matrix_t tmp3;

    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(R->rows == R->cols);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(R->rows == B->cols);
    ADVUTILS_ASSERT(Q->rows == A->rows);
    ADVUTILS_ASSERT(Q->rows == Q->cols);
    ADVUTILS_ASSERT(result->rows == result->cols);
    ADVUTILS_ASSERT(result->rows == A->cols);

    (void)matrixInit(&_Ak, A->rows, A->cols);
    (void)matrixInit(&_Ak1, A->rows, A->cols);
    (void)matrixInit(&_G, B->rows, B->rows);
    (void)matrixInit(&_IGP, A->rows, A->cols);
    (void)matrixInit(&tmp1, R->rows, R->cols);
    (void)matrixInit(&tmp2, A->rows, A->cols);
    (void)matrixInit(&tmp3, A->rows, A->cols);

    matrixCopy(A, &_Ak);
    matrixInversed(R, &tmp1);
    QuadProd(B, &tmp1, &_G);
    matrixCopy(Q, result);

    uint16_t nIter = nmax;
    while (nIter-- > 0U) {
        /* Calculation of inverse(I+G*H); */
        matrixMult(&_G, result, &tmp2);
        for (uint8_t i = 0; i < tmp2.rows; i++) {
            ELEM(tmp2, i, i) += 1.f;
        }
        matrixInversed(&tmp2, &_IGP);
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
            (void)matrixDelete(&_Ak);
            (void)matrixDelete(&_Ak1);
            (void)matrixDelete(&_G);
            (void)matrixDelete(&_IGP);
            (void)matrixDelete(&tmp2);
            (void)matrixDelete(&tmp1);
            (void)matrixDelete(&tmp3);
            return UTILS_STATUS_SUCCESS;
        }
        matrixCopy(&_Ak1, &_Ak);
    }

    /* Delete temporary matrices */
    (void)matrixDelete(&_Ak);
    (void)matrixDelete(&_Ak1);
    (void)matrixDelete(&_G);
    (void)matrixDelete(&_IGP);
    (void)matrixDelete(&tmp2);
    (void)matrixDelete(&tmp1);
    (void)matrixDelete(&tmp3);

    return UTILS_STATUS_TIMEOUT;
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
    float d1 = 0.0f;
    float d2 = 0.0f;
    float d3 = 0.0f;
    float k2;
    matrix_t Jr;
    matrix_t res;
    matrix_t delta;
    matrix_t tmp1;
    (void)matrixInit(&Jr, Data->rows, 9);
    (void)matrixInit(&res, Data->rows, 1);
    (void)matrixInit(&delta, 9, 1);
    (void)matrixInit(&tmp1, 9, Data->rows);

    if ((Data->rows < 9U) || (Data->cols != 3U)) {
        (void)matrixDelete(&Jr);
        (void)matrixDelete(&res);
        (void)matrixDelete(&delta);
        (void)matrixDelete(&tmp1);
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
            d1 = ELEMP(Data, i, 0) - ELEMP(result, 0, 0);
            d2 = ELEMP(Data, i, 1) - ELEMP(result, 1, 0);
            d3 = ELEMP(Data, i, 2) - ELEMP(result, 2, 0);
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
        matrixPseudoInv(&Jr, &tmp1);
        matrixMult(&tmp1, &res, &delta);
        matrixSub(result, &delta, result);
        if (matrixNorm(&delta) < tol) {
            (void)matrixDelete(&Jr);
            (void)matrixDelete(&res);
            (void)matrixDelete(&delta);
            (void)matrixDelete(&tmp1);
            return UTILS_STATUS_SUCCESS;
        } else if (isnan(d1) || isnan(d2) || isnan(d3)) {
            (void)matrixDelete(&Jr);
            (void)matrixDelete(&res);
            (void)matrixDelete(&delta);
            (void)matrixDelete(&tmp1);
            return UTILS_STATUS_ERROR;
        } else {
            /* no action required (MISRA 15.7) */
        }
    }
    (void)matrixDelete(&Jr);
    (void)matrixDelete(&res);
    (void)matrixDelete(&delta);
    (void)matrixDelete(&tmp1);
    return UTILS_STATUS_TIMEOUT;
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
    float d1 = 0.0f;
    float d2 = 0.0f;
    float d3 = 0.0f;
    float k2;

    matrix_t Jr;
    matrix_t res;
    matrix_t delta;
    matrix_t tmp1;
    (void)matrixInit(&Jr, Data->rows, 6);
    (void)matrixInit(&res, Data->rows, 1);
    (void)matrixInit(&delta, 6, 1);
    (void)matrixInit(&tmp1, 6, Data->rows);

    if ((Data->rows < 6U) || (Data->cols != 3U)) {
        (void)matrixDelete(&Jr);
        (void)matrixDelete(&res);
        (void)matrixDelete(&delta);
        (void)matrixDelete(&tmp1);
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
            d1 = ELEMP(Data, i, 0) - ELEMP(result, 0, 0);
            d2 = ELEMP(Data, i, 1) - ELEMP(result, 1, 0);
            d3 = ELEMP(Data, i, 2) - ELEMP(result, 2, 0);
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
        matrixPseudoInv(&Jr, &tmp1);
        matrixMult(&tmp1, &res, &delta);
        matrixSub(result, &delta, result);
        if (matrixNorm(&delta) < tol) {
            (void)matrixDelete(&Jr);
            (void)matrixDelete(&res);
            (void)matrixDelete(&delta);
            (void)matrixDelete(&tmp1);
            return UTILS_STATUS_SUCCESS;
        } else if (isnan(d1) || isnan(d2) || isnan(d3)) {
            (void)matrixDelete(&Jr);
            (void)matrixDelete(&res);
            (void)matrixDelete(&delta);
            (void)matrixDelete(&tmp1);
            return UTILS_STATUS_ERROR;
        } else {
            /* no action required (MISRA 15.7) */
        }
    }
    (void)matrixDelete(&Jr);
    (void)matrixDelete(&res);
    (void)matrixDelete(&delta);
    (void)matrixDelete(&tmp1);
    return UTILS_STATUS_TIMEOUT;
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
    float _A_cp_Data[A->rows * A->cols];
    matrix_t A_cp;
    matrixInitStatic(&A_cp, _A_cp_Data, A->rows, A->cols);
    matrixCopy(A, &A_cp);
    matrixZeros(U);
    matrixIdentity(L);

    for (uint8_t i = 0; i < A_cp.rows; i++) {
        ELEMP(U, i, i) = ELEM(A_cp, i, i);
        if (ELEM(A_cp, i, i) == 0) {
            return UTILS_STATUS_ERROR;
        }
        float tmp = 1.0 / ELEMP(U, i, i);
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
    ADVUTILS_ASSERT(P->cols == 1);
    int8_t d_mult = 1; /* determinant multiplying factor */
    float _A_cp_Data[A->rows * A->cols];
    matrix_t A_cp;
    matrixInitStatic(&A_cp, _A_cp_Data, A->rows, A->cols);
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
        uint8_t pivrow = i;
        float tmp = fabsf(ELEM(A_cp, i, i));
        for (uint8_t j = (uint8_t)(i + 1U); j < A_cp.rows; j++) {
            float tmp2 = fabsf(ELEM(A_cp, j, i));
            if (tmp2 > tmp) {
                tmp = tmp2;
                pivrow = j;
            }
        }
        /* check for singularity */
        if (ELEM(A_cp, pivrow, i) == 0) {
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
        tmp = 1.0 / ELEM(A_cp, i, i);
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

/* -----------------------Linear system solver using LU factorization--------------------------- */
/* solves the linear system A*X=B, where A is a n-by-n matrix and B an n-by-m matrix, giving the n-by-m matrix X */

void LinSolveLUStatic(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    float _LData[A->rows * A->cols];
    float _UData[A->cols * A->cols];
    matrix_t L;
    matrix_t U;
    matrixInitStatic(&L, _LData, A->rows, A->cols);
    matrixInitStatic(&U, _UData, A->cols, A->cols);
    /* matrix_t *tmp1 = matrixInit(A->rows, B->cols); */
    (void)LU_CormenStatic(A, &L, &U);
    /* fwsub(L, B, tmp1); */
    /* bksub(U, tmp1, result); */
    fwsub(&L, B, result);
    bksub(&U, result, result); /* hope it can work in-place */
    return;
}

/* ----------------------Linear system solver using LUP factorization-------------------------- */
/* solves the linear system A*X=B, where A is a n-by-n matrix and B an n-by-m matrix, giving the n-by-m matrix X */

void LinSolveLUPStatic(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);
    float _LData[A->rows * A->cols];
    float _UData[A->cols * A->cols];
    float _PData[A->rows];
    float _tmpData[A->rows * B->cols];
    matrix_t L;
    matrix_t U;
    matrix_t P;
    matrix_t tmp;
    matrixInitStatic(&L, _LData, A->rows, A->cols);
    matrixInitStatic(&U, _UData, A->cols, A->cols);
    matrixInitStatic(&P, _PData, A->rows, 1);
    matrixInitStatic(&tmp, _tmpData, A->rows, B->cols);

    (void)LUP_CormenStatic(A, &L, &U, &P);
    fwsubPerm(&L, B, &P, &tmp);
    bksub(&U, &tmp, result);
    return;
}

/* ------------Linear system solver using Gauss elimination with partial pivoting--------------- */
/* solves the linear system A*X=B, where A is a n-by-n matrix and B an n-by-m matrix, giving the n-by-m matrix X */

void LinSolveGaussStatic(const matrix_t* A, const matrix_t* B, matrix_t* result) {
    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(A->cols == result->rows);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(result->cols == B->cols);

    float _A_cp_Data[A->rows * A->cols];
    float _B_cp_Data[B->rows * B->cols];
    matrix_t A_cp;
    matrix_t B_cp;
    matrixInitStatic(&A_cp, _A_cp_Data, A->rows, A->cols);
    matrixInitStatic(&B_cp, _B_cp_Data, B->rows, B->cols);
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

        /* check for singular Matrix */
        if (ELEM(A_cp, pivrow, i) == 0.0) {
            matrixZeros(result);
            return;
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
        tmp = 1.0 / ELEM(A_cp, i, i);                             /* invert pivot element */
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
    bksub(&A_cp, &B_cp, result);
    return;
}

/* -------Iterative solver for discrete-time algebraic Riccati equation--------- */
/* Solves discrete-time algebraic Riccati equation P = A'*P*A-(B'*P*A)'*inv(R+B'*P*B)*B'*P*A+Q */
utilsStatus_t DAREStatic(const matrix_t* A, const matrix_t* B, const matrix_t* Q, const matrix_t* R, uint16_t nmax, float tol, matrix_t* result) {
    matrix_t _Ak;
    matrix_t _G;
    matrix_t _IGP;
    matrix_t _Ak1;
    matrix_t tmp1;
    matrix_t tmp2;
    matrix_t tmp3;

    ADVUTILS_ASSERT(A->rows == A->cols);
    ADVUTILS_ASSERT(R->rows == R->cols);
    ADVUTILS_ASSERT(A->rows == B->rows);
    ADVUTILS_ASSERT(R->rows == B->cols);
    ADVUTILS_ASSERT(Q->rows == A->rows);
    ADVUTILS_ASSERT(Q->rows == Q->cols);
    ADVUTILS_ASSERT(result->rows == result->cols);
    ADVUTILS_ASSERT(result->rows == A->cols);

    float _AkData[A->rows * A->cols];
    float _Ak1Data[A->rows * A->cols];
    float _GData[B->rows * B->rows];
    float _IGPData[A->rows * A->cols];
    float _tmp1Data[R->rows * R->cols];
    float _tmp2Data[A->rows * A->cols];
    float _tmp3Data[A->rows * A->cols];

    matrixInitStatic(&_Ak, _AkData, A->rows, A->cols);
    matrixInitStatic(&_Ak1, _Ak1Data, A->rows, A->cols);
    matrixInitStatic(&_G, _GData, B->rows, B->rows);
    matrixInitStatic(&_IGP, _IGPData, A->rows, A->cols);
    matrixInitStatic(&tmp1, _tmp1Data, R->rows, R->cols);
    matrixInitStatic(&tmp2, _tmp2Data, A->rows, A->cols);
    matrixInitStatic(&tmp3, _tmp3Data, A->rows, A->cols);

    matrixCopy(A, &_Ak);
    matrixInversedStatic(R, &tmp1);
    QuadProd(B, &tmp1, &_G);
    matrixCopy(Q, result);

    uint16_t nIter = nmax;
    while (nIter-- > 0U) {
        /* Calculation of inverse(I+G*H); */
        matrixMult(&_G, result, &tmp2);
        for (uint8_t i = 0; i < tmp2.rows; i++) {
            ELEM(tmp2, i, i) += 1.f;
        }
        matrixInversedStatic(&tmp2, &_IGP);
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
            return UTILS_STATUS_SUCCESS;
        }
        matrixCopy(&_Ak1, &_Ak);
    }
    return UTILS_STATUS_TIMEOUT;
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
    float d1 = 0.0f;
    float d2 = 0.0f;
    float d3 = 0.0f;
    float k2;
    float _JrData[Data->rows * 9U];
    float _resData[Data->rows];
    float _deltaData[9];
    float _tmp1Data[9U * Data->rows];
    matrix_t Jr;
    matrix_t res;
    matrix_t delta;
    matrix_t tmp1;
    matrixInitStatic(&Jr, _JrData, Data->rows, 9);
    matrixInitStatic(&res, _resData, Data->rows, 1);
    matrixInitStatic(&delta, _deltaData, 9, 1);
    matrixInitStatic(&tmp1, _tmp1Data, 9, Data->rows);

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
            d1 = ELEMP(Data, i, 0) - ELEMP(result, 0, 0);
            d2 = ELEMP(Data, i, 1) - ELEMP(result, 1, 0);
            d3 = ELEMP(Data, i, 2) - ELEMP(result, 2, 0);
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
        matrixPseudoInvStatic(&Jr, &tmp1);
        matrixMult(&tmp1, &res, &delta);
        matrixSub(result, &delta, result);
        if (matrixNorm(&delta) < tol) {
            return UTILS_STATUS_SUCCESS;
        } else if (isnan(d1) || isnan(d2) || isnan(d3)) {
            return UTILS_STATUS_ERROR;
        } else {
            /* no action required (MISRA 15.7) */
        }
    }
    return UTILS_STATUS_TIMEOUT;
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
    float d1 = 0.0f;
    float d2 = 0.0f;
    float d3 = 0.0f;
    float k2;

    float _JrData[Data->rows * 6U];
    float _resData[Data->rows];
    float _deltaData[6];
    float _tmp1Data[6U * Data->rows];
    matrix_t Jr;
    matrix_t res;
    matrix_t delta;
    matrix_t tmp1;
    matrixInitStatic(&Jr, _JrData, Data->rows, 6);
    matrixInitStatic(&res, _resData, Data->rows, 1);
    matrixInitStatic(&delta, _deltaData, 6, 1);
    matrixInitStatic(&tmp1, _tmp1Data, 6, Data->rows);

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
            d1 = ELEMP(Data, i, 0) - ELEMP(result, 0, 0);
            d2 = ELEMP(Data, i, 1) - ELEMP(result, 1, 0);
            d3 = ELEMP(Data, i, 2) - ELEMP(result, 2, 0);
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
        matrixPseudoInvStatic(&Jr, &tmp1);
        matrixMult(&tmp1, &res, &delta);
        matrixSub(result, &delta, result);
        if (matrixNorm(&delta) < tol) {
            return UTILS_STATUS_SUCCESS;
        } else if (isnan(d1) || isnan(d2) || isnan(d3)) {
            return UTILS_STATUS_ERROR;
        } else {
            /* no action required (MISRA 15.7) */
        }
    }
    return UTILS_STATUS_TIMEOUT;
}

#endif /* ADVUTILS_USE_STATIC_ALLOCATION */
