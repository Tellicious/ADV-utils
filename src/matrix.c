/* BEGIN Header */
/**
 ******************************************************************************
 * \file            matrix.c
 * \author          Andrea Vivani
 * \brief           Implementation of matrix manipulation functions
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

#include "matrix.h"
#include <math.h>
#include "basicMath.h"
#include "numMethods.h"
#ifdef ADVUTILS_MEMORY_MGMT_HEADER
#if !defined(ADVUTILS_MALLOC) || !defined(ADVUTILS_CALLOC) || !defined(ADVUTILS_FREE)
#error ADVUTILS_MALLOC, ADVUTILS_CALLOC and ADVUTILS_FREE must be defined by the user!
#else
#include ADVUTILS_MEMORY_MGMT_HEADER
#endif /* !defined(ADVUTILS_MALLOC) || !defined(ADVUTILS_CALLOC) || !defined(ADVUTILS_FREE) */
#else
#include <stdlib.h>
#endif /* ADVUTILS_MEMORY_MGMT_HEADER */

/* Macros --------------------------------------------------------------------*/

#ifndef ADVUTILS_MEMORY_MGMT_HEADER
#define ADVUTILS_MALLOC malloc
#define ADVUTILS_CALLOC calloc
#define ADVUTILS_FREE   free
#endif /* ADVUTILS_MEMORY_MGMT_HEADER */

#ifndef ADVUTILS_ASSERT
#ifdef DEBUG
#define ADVUTILS_ASSERT(x)                                                                                             \
    if ((x) == 0) {                                                                                                    \
        for (;;)                                                                                                       \
            ;                                                                                                          \
    }
#else
#define ADVUTILS_ASSERT(x)
#endif /* DEBUG */
#endif /* ADVUTILS_ASSERT */

/* ==========================================Assignment============================================= */

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/* -----------------------Constructor----------------------- */
utilsStatus_t matrixInit(matrix_t* matrix, uint8_t rows, uint8_t cols) {
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->data = ADVUTILS_CALLOC(rows * cols, sizeof(float));
    ADVUTILS_ASSERT(matrix->data != NULL);
    if (matrix->data == NULL) {
        return UTILS_STATUS_ERROR;
    }

    return UTILS_STATUS_SUCCESS;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION

/* --------------------Static Constructor------------------- */
void matrixInitStatic(matrix_t* matrix, float* data, uint8_t rows, uint8_t cols) {
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->data = data;
    return;
}

#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

/* ---------------------Identity Matrix---------------------- */
void matrixIdentity(matrix_t* matrix) {
    uint16_t ii;
    matrixZeros(matrix);
    for (ii = 0; ii < ((matrix->cols < matrix->rows) ? matrix->cols : matrix->rows); ii++) {
        ELEMP(matrix, ii, ii) = 1.0f;
    }
    return;
}

/* ==========================================Operations============================================= */
/* --------------------matrix_t addition---------------------- */
void matrixAdd(matrix_t* lhs, matrix_t* rhs, matrix_t* result) {
    ADVUTILS_ASSERT(lhs->cols == rhs->cols);
    ADVUTILS_ASSERT(lhs->rows == rhs->rows);
    uint16_t ii;
    for (ii = 0; ii < (lhs->cols * lhs->rows); ii++) {
        result->data[ii] = lhs->data[ii] + rhs->data[ii];
    }
    return;
}

/* --------------------Scalar addition---------------------- */
void matrixAddScalar(matrix_t* lhs, float sc, matrix_t* result) {
    uint16_t ii;
    for (ii = 0; ii < (lhs->cols * lhs->rows); ii++) {
        result->data[ii] = lhs->data[ii] + sc;
    }
    return;
}

/* ------------------matrix_t subtraction-------------------- */
void matrixSub(matrix_t* lhs, matrix_t* rhs, matrix_t* result) {
    ADVUTILS_ASSERT(lhs->cols == rhs->cols);
    ADVUTILS_ASSERT(lhs->rows == rhs->rows);
    uint16_t ii;
    for (ii = 0; ii < (lhs->cols * lhs->rows); ii++) {
        result->data[ii] = lhs->data[ii] - rhs->data[ii];
    }
    return;
}

/* ---------------matrix_t multiplication------------------ */
void matrixMult(matrix_t* lhs, matrix_t* rhs, matrix_t* result) {
    ADVUTILS_ASSERT(lhs->cols == rhs->rows);
    ADVUTILS_ASSERT(result->rows == lhs->rows);
    ADVUTILS_ASSERT(result->cols == rhs->cols);
    /* 	uint8_t i, j, k; */
    /* 	matrixZeros(result); */
    /* 	for (i = 0; i < lhs->rows; i++) */
    /* 		for (j = 0; j < rhs->cols; j++) */
    /* 			for (k = 0; k < lhs->cols; k++) */
    /* 				ELEMP(result, i, j) += ELEMP(lhs, i, k) * ELEMP(rhs, k, j); */
    float* pIn1 = lhs->data;    /* input data matrix pointer A */
    float* pIn2 = rhs->data;    /* input data matrix pointer B */
    float* pInA = lhs->data;    /* input data matrix pointer A  */
    float* pOut = result->data; /* output data matrix pointer */
    float* px;                  /* Temporary output data matrix pointer */
    float sum;                  /* Accumulator */

    /* Run the below code for Cortex-M4 and Cortex-M3 */

    float in1, in2, in3, in4;
    uint16_t col, i = 0u, j, row = lhs->rows, colCnt; /* loop counters */

    {
        /* The following loop performs the dot-product of each row in lhs with each column in rhs */
        /* row loop */
        do {
            /* Output pointer is set to starting address of the row being processed */
            px = pOut + i;

            /* For every row wise process, the column loop counter is to be initiated */
            col = rhs->cols;

            /* For every row wise process, the pIn2 pointer is set
             * to the starting address of the rhs data */
            pIn2 = rhs->data;

            j = 0u;

            /* column loop */
            do {
                /* Set the variable sum, that acts as accumulator, to zero */
                sum = 0.0f;

                /* Initiate the pointer pIn1 to point to the starting address of the column being processed */
                pIn1 = pInA;

                /* Apply loop unrolling and compute 4 MACs simultaneously. */
                colCnt = lhs->cols >> 2u;

                /* matrix multiplication        */
                while (colCnt > 0u) {
                    /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */
                    in3 = *pIn2;
                    pIn2 += rhs->cols;
                    in1 = pIn1[0];
                    in2 = pIn1[1];
                    sum += in1 * in3;
                    in4 = *pIn2;
                    pIn2 += rhs->cols;
                    sum += in2 * in4;

                    in3 = *pIn2;
                    pIn2 += rhs->cols;
                    in1 = pIn1[2];
                    in2 = pIn1[3];
                    sum += in1 * in3;
                    in4 = *pIn2;
                    pIn2 += rhs->cols;
                    sum += in2 * in4;
                    pIn1 += 4u;

                    /* Decrement the loop count */
                    colCnt--;
                }

                /* If the columns of lhs is not a multiple of 4, compute any remaining MACs here.
                 * No loop unrolling is used. */
                colCnt = lhs->cols % 0x4u;

                while (colCnt > 0u) {
                    /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */
                    sum += *pIn1++ * (*pIn2);
                    pIn2 += rhs->cols;

                    /* Decrement the loop counter */
                    colCnt--;
                }

                /* Store the result in the destination buffer */
                *px++ = sum;

                /* Update the pointer pIn2 to point to the  starting address of the next column */
                j++;
                pIn2 = rhs->data + j;

                /* Decrement the column loop counter */
                col--;

            } while (col > 0u);
            /* Update the pointer pInA to point to the  starting address of the next row */
            i = i + rhs->cols;
            pInA = pInA + lhs->cols;

            /* Decrement the row loop counter */
            row--;

        } while (row > 0u);
    }
    return;
}

/* ------matrix_t multiplication with lhs transposed------ */
void matrixMult_lhsT(matrix_t* lhs, matrix_t* rhs, matrix_t* result) {
    ADVUTILS_ASSERT(lhs->rows == rhs->rows);
    ADVUTILS_ASSERT(result->rows == lhs->cols);
    ADVUTILS_ASSERT(result->cols == rhs->cols);
    uint8_t i, j, k;
    matrixZeros(result);
    for (i = 0; i < lhs->cols; i++) {
        for (j = 0; j < rhs->cols; j++) {
            for (k = 0; k < lhs->rows; k++) {
                ELEMP(result, i, j) += ELEMP(lhs, k, i) * ELEMP(rhs, k, j);
            }
        }
    }
    return;
}

/* ------matrix_t multiplication with rhs transposed------ */
void matrixMult_rhsT(matrix_t* lhs, matrix_t* rhs, matrix_t* result) {
    ADVUTILS_ASSERT(lhs->cols == rhs->cols);
    ADVUTILS_ASSERT(result->rows == lhs->rows);
    ADVUTILS_ASSERT(result->cols == rhs->rows);
    uint8_t i, j, k;
    matrixZeros(result);
    for (i = 0; i < lhs->rows; i++) {
        for (j = 0; j < rhs->rows; j++) {
            for (k = 0; k < lhs->cols; k++) {
                ELEMP(result, i, j) += ELEMP(lhs, i, k) * ELEMP(rhs, j, k);
            }
        }
    }
    return;
}

/* ---------------Scalar multiplication------------------ */
void matrixMultScalar(matrix_t* lhs, float sc, matrix_t* result) {
    uint16_t ii;
    for (ii = 0; ii < (lhs->cols * lhs->rows); ii++) {
        result->data[ii] = lhs->data[ii] * sc;
    }
    return;
}

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION

/* --------------------Inverse LU------------------------ */
void matrixInversed(matrix_t* lhs, matrix_t* result) {
    ADVUTILS_ASSERT(lhs->rows == lhs->cols);
    ADVUTILS_ASSERT(result->rows == lhs->rows);
    ADVUTILS_ASSERT(result->cols == lhs->cols);
    matrix_t Eye;
    matrixInit(&Eye, lhs->rows, lhs->cols);
    matrixIdentity(&Eye);
    LinSolveLU(lhs, &Eye, result);
    matrixDelete(&Eye);
    return;
}

/* -----------------Robust Inverse LUP------------------- */
void matrixInversed_rob(matrix_t* lhs, matrix_t* result) {
    ADVUTILS_ASSERT(lhs->rows == lhs->cols);
    ADVUTILS_ASSERT(result->rows == lhs->rows);
    ADVUTILS_ASSERT(result->cols == lhs->cols);
    matrix_t Eye;
    matrixInit(&Eye, lhs->rows, lhs->cols);
    matrixIdentity(&Eye);
    LinSolveLUP(lhs, &Eye, result);
    matrixDelete(&Eye);
    return;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION

/* -----------------Static Inverse LU-------------------- */
void matrixInversedStatic(matrix_t* lhs, matrix_t* result) {
    ADVUTILS_ASSERT(lhs->rows == lhs->cols);
    ADVUTILS_ASSERT(result->rows == lhs->rows);
    ADVUTILS_ASSERT(result->cols == lhs->cols);
    float _eyeData[lhs->rows * lhs->cols];
    matrix_t Eye;
    matrixInitStatic(&Eye, _eyeData, lhs->rows, lhs->cols);
    matrixIdentity(&Eye);
    LinSolveLUStatic(lhs, &Eye, result);
    return;
}

/* --------------Static Robust Inverse LUP--------------- */
void matrixInversedStatic_rob(matrix_t* lhs, matrix_t* result) {
    ADVUTILS_ASSERT(lhs->rows == lhs->cols);
    ADVUTILS_ASSERT(result->rows == lhs->rows);
    ADVUTILS_ASSERT(result->cols == lhs->cols);
    float _eyeData[lhs->rows * lhs->cols];
    matrix_t Eye;
    matrixInitStatic(&Eye, _eyeData, lhs->rows, lhs->cols);
    matrixIdentity(&Eye);
    LinSolveLUPStatic(lhs, &Eye, result);
    return;
}

#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

/* -----------------Transposed-------------------- */
void matrixTrans(matrix_t* lhs, matrix_t* result) {
    ADVUTILS_ASSERT(result->rows == lhs->cols);
    ADVUTILS_ASSERT(result->cols == lhs->rows);
    uint8_t ii, jj;
    for (ii = 0; ii < lhs->rows; ii++) {
        for (jj = 0; jj < lhs->cols; jj++) {
            ELEMP(result, jj, ii) = ELEMP(lhs, ii, jj);
        }
    }
    return;
}

/* -----------------Nomalized-------------------- */
void matrixNormalized(matrix_t* lhs, matrix_t* result) {
    float k = 1.0f / matrixNorm(lhs);
    matrixMultScalar(lhs, k, result);
    return;
}

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION

/* -------Moore-Penrose pseudo inverse--------- */
void matrixPseudoInv(matrix_t* lhs, matrix_t* result) {
    ADVUTILS_ASSERT(result->rows == lhs->cols);
    ADVUTILS_ASSERT(result->cols == lhs->rows);
    matrix_t tran, mult1;
    matrixInit(&tran, lhs->cols, lhs->rows);
    matrixInit(&mult1, lhs->cols, lhs->cols);
    matrixTrans(lhs, &tran);
    matrixMult(&tran, lhs, &mult1);
    LinSolveLU(&mult1, &tran, result);
    matrixDelete(&tran);
    matrixDelete(&mult1);
    return;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION

/* -------Moore-Penrose pseudo inverse--------- */
void matrixPseudoInvStatic(matrix_t* lhs, matrix_t* result) {
    ADVUTILS_ASSERT(result->rows == lhs->cols);
    ADVUTILS_ASSERT(result->cols == lhs->rows);
    float _tranData[lhs->cols * lhs->rows];
    float _mult1Data[lhs->cols * lhs->cols];
    matrix_t tran, mult1;
    matrixInitStatic(&tran, _tranData, lhs->cols, lhs->rows);
    matrixInitStatic(&mult1, _mult1Data, lhs->cols, lhs->cols);
    matrixTrans(lhs, &tran);
    matrixMult(&tran, lhs, &mult1);
    LinSolveLUStatic(&mult1, &tran, result);
    return;
}

#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

/* =======================================matrix_t Data========================================= */

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION

/* -----------Returns the determinant---------- */
float matrixDet(matrix_t* matrix) {
    if (matrix->rows != matrix->cols) {
        return 0.0f;
    }
    matrix_t L, U, P;
    matrixInit(&L, matrix->rows, matrix->rows);
    matrixInit(&U, matrix->rows, matrix->rows);
    matrixInit(&P, matrix->rows, 1);
    int16_t ii;
    int8_t det_f;
    float determinant = 1.0f;

    if (LU_Cormen(matrix, &L, &U)) {
        for (ii = 0; ii < matrix->rows; ii++) {
            determinant *= ELEM(U, ii, ii);
        }
    }

    else {
        det_f = LUP_Cormen(matrix, &L, &U, &P);
        if (det_f) {
            for (ii = 0; ii < matrix->rows; ii++) {
                determinant *= ELEM(U, ii, ii);
            }
            determinant *= det_f;
        } else {
            determinant = 0.0f;
        }
    }

    matrixDelete(&L);
    matrixDelete(&U);
    matrixDelete(&P);

    return determinant;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION

/* -----------Returns the determinant---------- */
float matrixDetStatic(matrix_t* matrix) {
    if (matrix->rows != matrix->cols) {
        return 0.0f;
    }
    float _LData[matrix->rows * matrix->rows];
    float _UData[matrix->rows * matrix->rows];
    float _PData[matrix->rows];
    matrix_t L, U, P;
    matrixInitStatic(&L, _LData, matrix->rows, matrix->rows);
    matrixInitStatic(&U, _UData, matrix->rows, matrix->rows);
    matrixInitStatic(&P, _PData, matrix->rows, 1);
    int16_t ii;
    int8_t det_f;
    float determinant = 1.0f;

    if (LU_CormenStatic(matrix, &L, &U)) {
        for (ii = 0; ii < matrix->rows; ii++) {
            determinant *= ELEM(U, ii, ii);
        }
    }

    else {
        det_f = LUP_CormenStatic(matrix, &L, &U, &P);
        if (det_f) {
            for (ii = 0; ii < matrix->rows; ii++) {
                determinant *= ELEM(U, ii, ii);
            }
            determinant *= det_f;
        } else {
            determinant = 0.0f;
        }
    }
    return determinant;
}

#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

/* -------------Returns the norm-------------- */
float matrixNorm(matrix_t* matrix) {
    float result = 0.0f;
    uint16_t i;
    for (i = 0; i < (matrix->rows * matrix->cols); i++) {
        result += matrix->data[i] * matrix->data[i];
    }
    result = SQRT(result);
    return result;
}

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION

/* -------------Deletes the data-------------- */
utilsStatus_t matrixDelete(matrix_t* matrix) {
    if (matrix->data == NULL) {
        return UTILS_STATUS_ERROR;
    }

    ADVUTILS_FREE(matrix->data);

    return UTILS_STATUS_SUCCESS;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */
