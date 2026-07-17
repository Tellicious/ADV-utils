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
#include "ADVUtilsAssert.h"
#include "ADVUtilsMemory.h"
#include "basicMath.h"
#include "numMethods.h"

/* ==========================================Assignment============================================= */

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/* -----------------------Constructor----------------------- */
utilsStatus_t matrixInit(matrix_t* matrix, uint8_t rows, uint8_t cols) {
    matrix->rows = rows;
    matrix->cols = cols;
    /* cppcheck-suppress misra-c2012-11.5 ; deviation: generic container returns typed pointer from void* storage */
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
    matrixZeros(matrix);
    for (uint16_t i = 0; i < ((matrix->cols < matrix->rows) ? matrix->cols : matrix->rows); i++) {
        ELEMP(matrix, i, i) = 1.0f;
    }
    return;
}

/* ==========================================Operations============================================= */
/* --------------------matrix_t addition---------------------- */
void matrixAdd(const matrix_t* lhs, const matrix_t* rhs, matrix_t* result) {
    ADVUTILS_ASSERT(lhs->cols == rhs->cols);
    ADVUTILS_ASSERT(lhs->rows == rhs->rows);
    ADVUTILS_ASSERT(result->rows == lhs->rows);
    ADVUTILS_ASSERT(result->cols == lhs->cols);
    for (uint16_t i = 0; i < (lhs->cols * lhs->rows); i++) {
        result->data[i] = lhs->data[i] + rhs->data[i];
    }
    return;
}

/* --------------------Scalar addition---------------------- */
void matrixAddScalar(const matrix_t* lhs, float sc, matrix_t* result) {
    ADVUTILS_ASSERT(result->rows == lhs->rows);
    ADVUTILS_ASSERT(result->cols == lhs->cols);
    for (uint16_t i = 0; i < (lhs->cols * lhs->rows); i++) {
        result->data[i] = lhs->data[i] + sc;
    }
    return;
}

/* ------------------matrix_t subtraction-------------------- */
void matrixSub(const matrix_t* lhs, const matrix_t* rhs, matrix_t* result) {
    ADVUTILS_ASSERT(lhs->cols == rhs->cols);
    ADVUTILS_ASSERT(lhs->rows == rhs->rows);
    ADVUTILS_ASSERT(result->rows == lhs->rows);
    ADVUTILS_ASSERT(result->cols == lhs->cols);
    for (uint16_t i = 0; i < (lhs->cols * lhs->rows); i++) {
        result->data[i] = lhs->data[i] - rhs->data[i];
    }
    return;
}

/* ---------------matrix_t multiplication------------------ */
void matrixMult(const matrix_t* lhs, const matrix_t* rhs, const matrix_t* result) {
    ADVUTILS_ASSERT(lhs->cols == rhs->rows);
    ADVUTILS_ASSERT(result->rows == lhs->rows);
    ADVUTILS_ASSERT(result->cols == rhs->cols);
    /* 	uint8_t i, j, k; */
    /* 	matrixZeros(result); */
    /* 	for (i = 0; i < lhs->rows; i++) */
    /* 		for (j = 0; j < rhs->cols; j++) */
    /* 			for (k = 0; k < lhs->cols; k++) */
    /* 				ELEMP(result, i, j) += ELEMP(lhs, i, k) * ELEMP(rhs, k, j); */
    float* pIn1;                /* input data matrix pointer A */
    float* pInA = lhs->data;    /* input data matrix pointer A  */
    float* pInB = rhs->data;    /* input data matrix pointer B */
    float* pOut = result->data; /* output data matrix pointer */

    /* Run the below code for Cortex-M4 and Cortex-M3 */

    uint16_t i = 0U;
    uint16_t row = lhs->rows;
    /* The following loop performs the dot-product of each row in lhs with each column in rhs */
    /* row loop */
    do {
        /* Output pointer is set to starting address of the row being processed */
        /* cppcheck-suppress misra-c2012-18.4 ; deviation: pointer walk in performance-critical kernel (advisory) */
        float* px = pOut + i;

        /* For every row wise process, the column loop counter is to be initiated */
        uint16_t col = rhs->cols;

        /* For every row wise process, the pIn2 pointer is set
             * to the starting address of the rhs data */
        float* pIn2 = pInB;

        uint16_t j = 0U;

        /* column loop */
        do {
            /* Set the variable sum, that acts as accumulator, to zero */
            float sum = 0.0f;

            /* Initiate the pointer pIn1 to point to the starting address of the column being processed */
            pIn1 = pInA;

            /* Apply loop unrolling and compute 4 MACs simultaneously. */
            uint16_t colCnt = lhs->cols >> 2U;

            /* matrix multiplication        */
            while (colCnt > 0U) {
                /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */
                float in3 = *pIn2;
                /* cppcheck-suppress misra-c2012-18.4 ; deviation: pointer walk in performance-critical kernel (advisory) */
                pIn2 += rhs->cols;
                float in1 = pIn1[0];
                float in2 = pIn1[1];
                sum += in1 * in3;
                float in4 = *pIn2;
                /* cppcheck-suppress misra-c2012-18.4 ; deviation: pointer walk in performance-critical kernel (advisory) */
                pIn2 += rhs->cols;
                sum += in2 * in4;

                in3 = *pIn2;
                /* cppcheck-suppress misra-c2012-18.4 ; deviation: pointer walk in performance-critical kernel (advisory) */
                pIn2 += rhs->cols;
                in1 = pIn1[2];
                in2 = pIn1[3];
                sum += in1 * in3;
                in4 = *pIn2;
                /* cppcheck-suppress misra-c2012-18.4 ; deviation: pointer walk in performance-critical kernel (advisory) */
                pIn2 += rhs->cols;
                sum += in2 * in4;
                /* cppcheck-suppress[misra-c2012-18.4,misra-c2012-10.3] ; deviation: pointer walk in performance-critical kernel (advisory); pointer increment in matrix kernel (advisory) */
                pIn1 += 4U;

                /* Decrement the loop count */
                colCnt--;
            }

            /* If the columns of lhs is not a multiple of 4, compute any remaining MACs here.
                 * No loop unrolling is used. */
            colCnt = lhs->cols % 0x4u;

            while (colCnt > 0U) {
                /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */
                sum += *pIn1++ * (*pIn2);
                /* cppcheck-suppress misra-c2012-18.4 ; deviation: pointer walk in performance-critical kernel (advisory) */
                pIn2 += rhs->cols;

                /* Decrement the loop counter */
                colCnt--;
            }

            /* Store the result in the destination buffer */
            /* cppcheck-suppress misra-c2012-13.3 ; deviation: pointer post-increment in matrix kernel (advisory) */
            *px++ = sum;

            /* Update the pointer pIn2 to point to the  starting address of the next column */
            j++;
            /* cppcheck-suppress misra-c2012-18.4 ; deviation: pointer walk in performance-critical kernel (advisory) */
            pIn2 = rhs->data + j;

            /* Decrement the column loop counter */
            col--;

        } while (col > 0U);
        /* Update the pointer pInA to point to the  starting address of the next row */
        i = i + rhs->cols;
        /* cppcheck-suppress misra-c2012-18.4 ; deviation: pointer walk in performance-critical kernel (advisory) */
        pInA = pInA + lhs->cols;

        /* Decrement the row loop counter */
        row--;

    } while (row > 0U);
    return;
}

/* ------matrix_t multiplication with lhs transposed------ */
void matrixMult_lhsT(const matrix_t* lhs, const matrix_t* rhs, matrix_t* result) {
    ADVUTILS_ASSERT(lhs->rows == rhs->rows);
    ADVUTILS_ASSERT(result->rows == lhs->cols);
    ADVUTILS_ASSERT(result->cols == rhs->cols);
    matrixZeros(result);
    for (uint8_t i = 0; i < lhs->cols; i++) {
        for (uint8_t j = 0; j < rhs->cols; j++) {
            for (uint8_t k = 0; k < lhs->rows; k++) {
                ELEMP(result, i, j) += ELEMP(lhs, k, i) * ELEMP(rhs, k, j);
            }
        }
    }
    return;
}

/* ------matrix_t multiplication with rhs transposed------ */
void matrixMult_rhsT(const matrix_t* lhs, const matrix_t* rhs, matrix_t* result) {
    ADVUTILS_ASSERT(lhs->cols == rhs->cols);
    ADVUTILS_ASSERT(result->rows == lhs->rows);
    ADVUTILS_ASSERT(result->cols == rhs->rows);
    matrixZeros(result);
    for (uint8_t i = 0; i < lhs->rows; i++) {
        for (uint8_t j = 0; j < rhs->rows; j++) {
            for (uint8_t k = 0; k < lhs->cols; k++) {
                ELEMP(result, i, j) += ELEMP(lhs, i, k) * ELEMP(rhs, j, k);
            }
        }
    }
    return;
}

/* ---------------Scalar multiplication------------------ */
void matrixMultScalar(const matrix_t* lhs, float sc, matrix_t* result) {
    ADVUTILS_ASSERT(result->rows == lhs->rows);
    ADVUTILS_ASSERT(result->cols == lhs->cols);
    for (uint16_t i = 0; i < (lhs->cols * lhs->rows); i++) {
        result->data[i] = lhs->data[i] * sc;
    }
    return;
}

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION

/* --------------------Inverse LU------------------------ */
void matrixInversed(const matrix_t* lhs, matrix_t* result) {
    ADVUTILS_ASSERT(lhs->rows == lhs->cols);
    ADVUTILS_ASSERT(result->rows == lhs->rows);
    ADVUTILS_ASSERT(result->cols == lhs->cols);
    matrix_t Eye;
    (void)matrixInit(&Eye, lhs->rows, lhs->cols);
    matrixIdentity(&Eye);
    LinSolveLU(lhs, &Eye, result);
    (void)matrixDelete(&Eye);
    return;
}

/* -----------------Robust Inverse LUP------------------- */
void matrixInversed_rob(const matrix_t* lhs, matrix_t* result) {
    ADVUTILS_ASSERT(lhs->rows == lhs->cols);
    ADVUTILS_ASSERT(result->rows == lhs->rows);
    ADVUTILS_ASSERT(result->cols == lhs->cols);
    matrix_t Eye;
    (void)matrixInit(&Eye, lhs->rows, lhs->cols);
    matrixIdentity(&Eye);
    LinSolveLUP(lhs, &Eye, result);
    (void)matrixDelete(&Eye);
    return;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION

/* -----------------Static Inverse LU-------------------- */
void matrixInversedStatic(const matrix_t* lhs, matrix_t* result) {
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
void matrixInversedStatic_rob(const matrix_t* lhs, matrix_t* result) {
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
    for (uint8_t i = 0; i < lhs->rows; i++) {
        for (uint8_t j = 0; j < lhs->cols; j++) {
            ELEMP(result, j, i) = ELEMP(lhs, i, j);
        }
    }
    return;
}

/* -----------------Nomalized-------------------- */
void matrixNormalized(const matrix_t* lhs, matrix_t* result) {
    ADVUTILS_ASSERT(result->rows == lhs->rows);
    ADVUTILS_ASSERT(result->cols == lhs->cols);
    float k = 1.0f / matrixNorm(lhs);
    matrixMultScalar(lhs, k, result);
    return;
}

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION

/* -------Moore-Penrose pseudo inverse--------- */
void matrixPseudoInv(matrix_t* lhs, matrix_t* result) {
    ADVUTILS_ASSERT(result->rows == lhs->cols);
    ADVUTILS_ASSERT(result->cols == lhs->rows);
    matrix_t tran;
    matrix_t mult1;
    (void)matrixInit(&tran, lhs->cols, lhs->rows);
    (void)matrixInit(&mult1, lhs->cols, lhs->cols);
    matrixTrans(lhs, &tran);
    matrixMult(&tran, lhs, &mult1);
    LinSolveLU(&mult1, &tran, result);
    (void)matrixDelete(&tran);
    (void)matrixDelete(&mult1);
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
    matrix_t tran;
    matrix_t mult1;
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
float matrixDet(const matrix_t* matrix) {
    if (matrix->rows != matrix->cols) {
        return 0.0f;
    }
    matrix_t L;
    matrix_t U;
    matrix_t P;
    (void)matrixInit(&L, matrix->rows, matrix->rows);
    (void)matrixInit(&U, matrix->rows, matrix->rows);
    (void)matrixInit(&P, matrix->rows, 1);
    float determinant = 1.0f;

    if (LU_Cormen(matrix, &L, &U) == UTILS_STATUS_SUCCESS) {
        for (uint8_t i = 0; i < matrix->rows; i++) {
            determinant *= ELEM(U, i, i);
        }
    }

    else {
        int8_t det_f = LUP_Cormen(matrix, &L, &U, &P);
        for (uint8_t i = 0; i < matrix->rows; i++) {
            determinant *= ELEM(U, i, i);
        }
        determinant *= (float)det_f;
    }

    (void)matrixDelete(&L);
    (void)matrixDelete(&U);
    (void)matrixDelete(&P);

    return determinant;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION

/* -----------Returns the determinant---------- */
float matrixDetStatic(const matrix_t* matrix) {
    if (matrix->rows != matrix->cols) {
        return 0.0f;
    }
    float _LData[matrix->rows * matrix->rows];
    float _UData[matrix->rows * matrix->rows];
    float _PData[matrix->rows];
    matrix_t L;
    matrix_t U;
    matrix_t P;
    matrixInitStatic(&L, _LData, matrix->rows, matrix->rows);
    matrixInitStatic(&U, _UData, matrix->rows, matrix->rows);
    matrixInitStatic(&P, _PData, matrix->rows, 1);
    float determinant = 1.0f;

    if (LU_CormenStatic(matrix, &L, &U) == UTILS_STATUS_SUCCESS) {
        for (uint8_t i = 0; i < matrix->rows; i++) {
            determinant *= ELEM(U, i, i);
        }
    }

    else {
        int8_t det_f = LUP_CormenStatic(matrix, &L, &U, &P);
        for (uint8_t i = 0; i < matrix->rows; i++) {
            determinant *= ELEM(U, i, i);
        }
        determinant *= (float)det_f;
    }
    return determinant;
}

#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

/* -------------Returns the norm-------------- */
float matrixNorm(const matrix_t* matrix) {
    float result = 0.0f;
    for (uint16_t i = 0; i < (matrix->rows * matrix->cols); i++) {
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
    matrix->data = NULL;

    return UTILS_STATUS_SUCCESS;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */
