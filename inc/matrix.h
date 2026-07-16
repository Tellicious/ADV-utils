/* BEGIN Header */
/**
 ******************************************************************************
 * \file            matrix.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MATRIX_H__
#define __MATRIX_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <string.h>
#include "commonTypes.h"

/* Macros --------------------------------------------------------------------*/

#define ELEMP(m, i, j) (m->data[(i) * m->cols + (j)])
#define ELEM(m, i, j)  (m.data[(i) * m.cols + (j)])

/* Typedefs ------------------------------------------------------------------*/

/**
 * Matrix struct
 */
typedef struct {
    float* data;
    uint8_t rows, cols;
} matrix_t;

/* Function prototypes -------------------------------------------------------*/

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Create a new matrix with dynamic memory allocation
 *
 * \param[in]       matrix: pointer to matrix object
 * \param[in]       rows: number of rows
 * \param[in]       cols: number of columns
 *
 * \return          UTILS_STATUS_SUCCESS if matrix was initialized, UTILS_STATUS_ERROR if data was not allocated correctly
 */
utilsStatus_t matrixInit(matrix_t* matrix, uint8_t rows, uint8_t cols);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION
/**
 * \brief           Create a new matrix with static data
 *
 * \param[in]       matrix: pointer to matrix object
 * \param[in]       data: pointer to data array of size rows * cols
 * \param[in]       rows: number of rows
 * \param[in]       cols: number of columns
 */
void matrixInitStatic(matrix_t* matrix, float* data, uint8_t rows, uint8_t cols);
#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

/**
 * \brief           Set the matrix as an identity matrix
 *
 * \param[in]       matrix: pointer to matrix object
 */
void matrixIdentity(matrix_t* matrix);

/**
 * \brief           Fill the matrix with zero
 *
 * \param[in]       matrix: pointer to matrix object
 */
static inline void matrixZeros(matrix_t* matrix) { memset(matrix->data, 0x00, matrix->cols * matrix->rows * sizeof(float)); }

/**
 * \brief           Copy data from input matrix to output matrix
 *
 * \param[in]       input: pointer to input matrix object
 * \param[out]      output: pointer to output matrix object
 */
static inline void matrixCopy(const matrix_t* input, matrix_t* output) { memcpy(output->data, input->data, output->cols * output->rows * sizeof(float)); }

/**
 * \brief           Matrix addition
 *
 * \param[in]       lhs: pointer to left-hand side matrix object
 * \param[in]       rhs: pointer to right-hand side matrix object
 * \param[out]      result: pointer to result matrix object
 */
void matrixAdd(const matrix_t* lhs, const matrix_t* rhs, matrix_t* result);

/**
 * \brief           Matrix addition with scalar
 *
 * \param[in]       lhs: pointer to left-hand side matrix object
 * \param[in]       sc: scalar value
 * \param[out]      result: pointer to result matrix object
 */
void matrixAddScalar(const matrix_t* lhs, float sc, matrix_t* result);

/**
 * \brief           Matrix subtraction
 *
 * \param[in]       lhs: pointer to left-hand side matrix object
 * \param[in]       rhs: pointer to right-hand side matrix object
 * \param[out]      result: pointer to result matrix object
 */
void matrixSub(const matrix_t* lhs, const matrix_t* rhs, matrix_t* result);

/**
 * \brief           Matrix multiplication
 *
 * \param[in]       lhs: pointer to left-hand side matrix object
 * \param[in]       rhs: pointer to right-hand side matrix object
 * \param[out]      result: pointer to result matrix object
 */
void matrixMult(const matrix_t* lhs, const matrix_t* rhs, const matrix_t* result);

/**
 * \brief           Matrix multiplication with transposed left-hand side matrix
 *
 * \param[in]       lhs: pointer to left-hand side matrix object
 * \param[in]       rhs: pointer to right-hand side matrix object
 * \param[out]      result: pointer to result matrix object
 */
void matrixMult_lhsT(const matrix_t* lhs, const matrix_t* rhs, matrix_t* result);

/**
 * \brief           Matrix multiplication with transposed right-hand side matrix
 *
 * \param[in]       lhs: pointer to left-hand side matrix object
 * \param[in]       rhs: pointer to right-hand side matrix object
 * \param[out]      result: pointer to result matrix object
 */
void matrixMult_rhsT(const matrix_t* lhs, const matrix_t* rhs, matrix_t* result);

/**
 * \brief           Matrix multiplication with scalar
 *
 * \param[in]       lhs: pointer to left-hand side matrix object
 * \param[in]       sc: scalar value
 * \param[out]      result: pointer to result matrix object
 */
void matrixMultScalar(const matrix_t* lhs, float sc, matrix_t* result);

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Matrix inversion
 *
 * \param[in]       lhs: pointer to left-hand side matrix object
 * \param[out]      result: pointer to result matrix object
 */
void matrixInversed(matrix_t* lhs, matrix_t* result);

/**
 * \brief           Robust matrix inversion performed with LUP decomposition
 *
 * \param[in]       lhs: pointer to left-hand side matrix object
 * \param[out]      result: pointer to result matrix object
 */
void matrixInversed_rob(matrix_t* lhs, matrix_t* result);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION
/**
 * \brief           Matrix inversion with static allocation
 *
 * \param[in]       lhs: pointer to left-hand side matrix object
 * \param[out]      result: pointer to result matrix object
 */
void matrixInversedStatic(matrix_t* lhs, matrix_t* result);

/**
 * \brief           Robust matrix inversion performed with LUP decomposition and static allocation
 *
 * \param[in]       lhs: pointer to left-hand side matrix object
 * \param[out]      result: pointer to result matrix object
 */
void matrixInversedStatic_rob(matrix_t* lhs, matrix_t* result);
#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

/**
 * \brief           Matrix transposition
 *
 * \param[in]       lhs: pointer to left-hand side matrix object
 * \param[out]      result: pointer to result matrix object
 */
void matrixTrans(matrix_t* lhs, matrix_t* result);

/**
 * \brief           Matrix normalization
 *
 * \param[in]       lhs: pointer to left-hand side matrix object
 * \param[out]      result: pointer to result matrix object
 */
void matrixNormalized(const matrix_t* lhs, matrix_t* result);

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Matrix Moore-Penrose pseudo-inverse
 *
 * \param[in]       lhs: pointer to left-hand side matrix object
 * \param[out]      result: pointer to result matrix object
 */
void matrixPseudoInv(matrix_t* lhs, matrix_t* result);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION
/**
 * \brief           Matrix Moore-Penrose pseudo-inverse with static allocation
 *
 * \param[in]       lhs: pointer to left-hand side matrix object
 * \param[out]      result: pointer to result matrix object
 */
void matrixPseudoInvStatic(matrix_t* lhs, matrix_t* result);
#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

/**
 * \brief           Set a single element inside matrix
 *
 * \param[in]       matrix: pointer to matrix object
 * \param[in]       i: row
 * \param[in]       j: column
 * \param[in]       value: value
 */
static inline void matrixSet(matrix_t* matrix, uint8_t i, uint8_t j, float value) { ELEMP(matrix, i, j) = value; }

/**
 * \brief           Get a single element inside matrix
 *
 * \param[in]       matrix: pointer to matrix object
 * \param[in]       i: row
 * \param[in]       j: column
 *
 * \return          matrix element
 */
static inline float matrixGet(const matrix_t* matrix, uint8_t i, uint8_t j) { return ELEMP(matrix, i, j); }

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Calculate matrix determinant
 *
 * \param[in]       matrix: pointer to matrix object
 *
 * \return          determinant
 */
float matrixDet(matrix_t* matrix);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION
/**
 * \brief           Calculate matrix determinant
 *
 * \param[in]       matrix: pointer to matrix object
 *
 * \return          determinant
 */
float matrixDetStatic(matrix_t* matrix);
#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

/**
 * \brief           Calculate matrix norm
 *
 * \param[in]       matrix: pointer to matrix object
 *
 * \return          norm
 */
float matrixNorm(const matrix_t* matrix);

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Delete matrix
 *
 * \param[in]       matrix: pointer to matrix object
 *
 * \return          UTILS_STATUS_SUCCESS if matrix data is deleted, UTILS_STATUS_ERROR if data was already deleted
 */
utilsStatus_t matrixDelete(matrix_t* matrix);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef __cplusplus
}
#endif

#endif /* __MATRIX_H__ */
