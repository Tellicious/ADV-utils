/* BEGIN Header */
/**
 ******************************************************************************
 * \file            numMethods.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ADVUTILS_NUMMETHODS_H
#define ADVUTILS_NUMMETHODS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "matrix.h"

/**
 * \defgroup numMethods Numerical methods
 * \brief Numerical methods
 * @{
 */

/* Function prototypes -------------------------------------------------------*/

/**
 * \brief           Solve an Ax = B system with forward substitution
 *
 * \attention       Assumes that the matrix A is already a lower triangular one. No check is performed within function!
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[out]      result: pointer to result matrix object
 */
void fwsub(const matrix_t* A, const matrix_t* B, matrix_t* result);

/**
 * \brief           Solve an Ax = PB system with forward substitution (with permutation)
 *
 * \attention       Assumes that the matrix A is already a lower triangular one. No check is performed within function!
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[in]       P: pointer to P matrix object, containing the indexes of the permuted rows of B in a column vector
 * \param[out]      result: pointer to result matrix object
 */
void fwsubPerm(const matrix_t* A, const matrix_t* B, const matrix_t* P, matrix_t* result);

/**
 * \brief           Solve AX = B system with backward substitution
 *
 * \attention       Assumes that the matrix A is already an upper triangular one. No check is performed within function!
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[out]      result: pointer to result matrix object
 */
void bksub(const matrix_t* A, const matrix_t* B, matrix_t* result);

/**
 * \brief           Solve AX = B system with backward substitution (with permutation)
 *
 * \attention       Assumes that the matrix A is already an upper triangular one. No check is performed within function!
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[in]       P: pointer to P matrix object, containing the indexes of the permuted rows of B in a column vector
 * \param[out]      result: pointer to result matrix object
 */
void bksubPerm(const matrix_t* A, const matrix_t* B, const matrix_t* P, matrix_t* result);

/**
 * \brief           Calculate sort of quadratic form result=A*B*(~A)
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[out]      result: pointer to result matrix object
 */
void QuadProd(const matrix_t* A, const matrix_t* B, matrix_t* result);

/**
 * \brief           Calculate L (lower triangular) and U (upper triangular) matrices such that A = LU with Crout's Method
 *
 * \param[in]       A: pointer to A matrix object
 * \param[out]      L: pointer to L matrix object
 * \param[out]      U: pointer to U matrix object
 *
 * \retval          UTILS_STATUS_SUCCESS success
 * \retval          UTILS_STATUS_ERROR on a zero pivot, or if a non-finite (NaN/Inf) pivot is encountered
 */
utilsStatus_t LU_Crout(const matrix_t* A, matrix_t* L, matrix_t* U);

/**
 * \brief           Cholesky factorization of a symmetric positive-definite matrix
 *
 * \param[in]       A: pointer to A matrix object (only the lower triangle is read)
 * \param[out]      L: pointer to lower-triangular result such that L*(~L) = A
 *
 * \attention       A must be square and symmetric positive-definite; only its lower triangle is used. L must be distinct from A (not in-place)
 *
 * \retval          UTILS_STATUS_SUCCESS if the factorization succeeds
 * \retval          UTILS_STATUS_ERROR if A is not positive-definite, or if a non-finite (NaN/Inf) pivot is encountered
 */
utilsStatus_t Cholesky(const matrix_t* A, matrix_t* L);

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Calculate L (lower triangular) and U (upper triangular) matrices such that A = LU with Cormen's Method
 *
 * \param[in]       A: pointer to A matrix object
 * \param[out]      L: pointer to L matrix object
 * \param[out]      U: pointer to U matrix object
 *
 * \retval          UTILS_STATUS_SUCCESS success
 * \retval          UTILS_STATUS_ERROR on a zero pivot, or if a non-finite (NaN/Inf) pivot is encountered
 */
utilsStatus_t LU_Cormen(const matrix_t* A, matrix_t* L, matrix_t* U);

/**
 * \brief           Calculate L (lower triangular), U (upper triangular) and P (permutation) matrices such that A = LUP with Cormen's Method
 *
 * \param[in]       A: pointer to A matrix object
 * \param[out]      L: pointer to L matrix object
 * \param[out]      U: pointer to U matrix object
 * \param[in]       P: pointer to P matrix object
 *
 * \return          factor to be multiplied by determinant of U to obtain determinant of A, or 0 if A is singular (a zero or non-finite pivot is found)
 */
int8_t LUP_Cormen(const matrix_t* A, matrix_t* L, matrix_t* U, matrix_t* P);

/**
 * \brief           Economy Householder QR factorization A = Q*R for a tall matrix
 *
 * \attention       A must have rows >= cols. Q and R must be distinct from A
 *
 * \param[in]       A: pointer to A matrix object (rows >= cols)
 * \param[out]      Q: pointer to Q matrix object (rows-by-cols, orthonormal columns)
 * \param[out]      R: pointer to R matrix object (cols-by-cols, upper-triangular)
 *
 * \retval          UTILS_STATUS_SUCCESS if the factorization succeeds
 * \retval          UTILS_STATUS_ERROR if A is rank-deficient (a near-zero pivot is found), or if the factorization is non-finite (NaN/Inf)
 */
utilsStatus_t QR_Householder(const matrix_t* A, matrix_t* Q, matrix_t* R);

/**
 * \brief           Solve AX = B system using LU factorization
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[out]      result: pointer to result matrix object
 *
 * \retval          UTILS_STATUS_SUCCESS if the system is solved
 * \retval          UTILS_STATUS_ERROR if A is singular, or if the computed result is non-finite (NaN/Inf)
 */
utilsStatus_t LinSolveLU(const matrix_t* A, const matrix_t* B, matrix_t* result);

/**
 * \brief           Solve AX = B system using LUP factorization
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[out]      result: pointer to result matrix object
 *
 * \retval          UTILS_STATUS_SUCCESS if the system is solved
 * \retval          UTILS_STATUS_ERROR if A is singular, or if the computed result is non-finite (NaN/Inf)
 */
utilsStatus_t LinSolveLUP(const matrix_t* A, const matrix_t* B, matrix_t* result);

/**
 * \brief           Solve AX = B system using Gauss elimination with partial pivoting
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[out]      result: pointer to result matrix object
 *
 * \retval          UTILS_STATUS_SUCCESS if the system is solved
 * \retval          UTILS_STATUS_ERROR if A is singular (zero or non-finite pivot), or if the computed result is non-finite (NaN/Inf)
 */
utilsStatus_t LinSolveGauss(const matrix_t* A, const matrix_t* B, matrix_t* result);

/**
 * \brief           Solve AX = B for symmetric positive-definite A using Cholesky factorization
 *
 * \param[in]       A: pointer to A matrix object (symmetric positive-definite, only lower triangle read)
 * \param[in]       B: pointer to B matrix object (can have multiple columns)
 * \param[out]      result: pointer to result matrix object
 *
 * \retval          UTILS_STATUS_SUCCESS if the system is solved
 * \retval          UTILS_STATUS_ERROR if A is not positive-definite, or if the computed result is non-finite (NaN/Inf)
 */
utilsStatus_t LinSolveCholesky(const matrix_t* A, const matrix_t* B, matrix_t* result);

/**
 * \brief           Solve AX = B using economy Householder QR (least-squares when rows > cols)
 *
 * \param[in]       A: pointer to A matrix object (rows >= cols)
 * \param[in]       B: pointer to B matrix object (can have multiple columns)
 * \param[out]      result: pointer to result matrix object
 *
 * \retval          UTILS_STATUS_SUCCESS if the system is solved
 * \retval          UTILS_STATUS_ERROR if A is rank-deficient, or if the computed result is non-finite (NaN/Inf)
 */
utilsStatus_t LinSolveQR(const matrix_t* A, const matrix_t* B, matrix_t* result);

/**
 * \brief           Solve discrete-time algebraic Riccati equation `P = A'*P*A - (B'*P*A)'*inv(R + B'*P*B)*B'*P*A + Q`
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[in]       Q: pointer to Q matrix object
 * \param[in]       R: pointer to R matrix object
 * \param[in]       nmax: maximum number of iterations (200 is generally fine, even if it usually converges within 15 iterations)
 * \param[in]       tol: stopping tolerance (1e-6 is generally fine)
 * \param[out]      result: pointer to P matrix object
 *
 * \retval          UTILS_STATUS_SUCCESS success
 * \retval          UTILS_STATUS_TIMEOUT nmax is reached
 */
utilsStatus_t DARE(const matrix_t* A, const matrix_t* B, const matrix_t* Q, const matrix_t* R, uint16_t nmax, float tol, matrix_t* result);

/**
 * \brief           Gauss-Newton sensor calibration with 9 parameters
 *
 * \attention       Approximates Data to a sphere of radius k by calculating 6 gains (s) and 3 biases (b), useful to calibrate some sensors (meas_sphere=S*(meas-B) with S symmetric)
 * \attention       Data has n>=9 rows corresponding to the number of measures and 3 columns corresponding to the 3 axes
 * \attention       b1=out(0,0);
 * b2=out(1,0);
 * b3=out(2,0);
 * s11=out(3,0);
 * s12=out(4,0);
 * s13=out(5,0);
 * s22=out(6,0);
 * s23=out(7,0);
 * s33=out(8,0);
 *
 * \param[in]       Data: pointer to raw data matrix object Data
 * \param[in]       k: radius of sphere to be approximated. If 0, it is calculated automatically
 * \param[in]       X0: pointer to starting vector X0 (usually [0 0 0 1 0 0 1 0 1]). If NULL it is calculated automatically
 * \param[in]       nmax: maximum number of iterations (200 is generally fine, even if it usually converges within 10 iterations)
 * \param[in]       tol: stopping tolerance (1e-6 is generally fine)
 * \param[out]      result: pointer to result matrix object S
 *
 * \retval          UTILS_STATUS_SUCCESS success
 * \retval          UTILS_STATUS_TIMEOUT nmax is reached
 * \retval          UTILS_STATUS_ERROR errors
 */
utilsStatus_t GaussNewton_Sens_Cal_9(const matrix_t* Data, float k, const matrix_t* X0, uint16_t nmax, float tol, matrix_t* result);

/**
 * \brief           Gauss-Newton sensor calibration with 6 parameters
 * 
 * \attention       Approximates Data to a sphere of radius k by calculating 3 gains (s) and 3 biases (b), useful to calibrate some sensors (meas_sphere=S*(meas-B) with S symmetric)
 * \attention       Data has n>=6 rows corresponding to the number of measures and 3 columns corresponding to the 3 axes
 * \attention       b1=out(0,0);
 * b2=out(1,0);
 * b3=out(2,0);
 * s11=out(3,0);
 * s22=out(4,0);
 * s33=out(5,0);
 *
 * \param[in]       Data: pointer to raw data matrix object Data
 * \param[in]       k: radius of sphere to be approximated. If 0, it is calculated automatically
 * \param[in]       X0: pointer to starting vector X0 (usually [0 0 0 1 1 1]). If NULL it is calculated automatically
 * \param[in]       nmax: maximum number of iterations (200 is generally fine, even if it usually converges within 10 iterations)
 * \param[in]       tol: stopping tolerance (1e-6 is generally fine)
 * \param[out]      result: pointer to result matrix object S
 *
 * \retval          UTILS_STATUS_SUCCESS success
 * \retval          UTILS_STATUS_TIMEOUT nmax is reached
 * \retval          UTILS_STATUS_ERROR errors
 */
utilsStatus_t GaussNewton_Sens_Cal_6(const matrix_t* Data, float k, const matrix_t* X0, uint16_t nmax, float tol, matrix_t* result);

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION
/**
 * \brief           Calculate L (lower triangular) and U (upper triangular) matrices such that A = LU with Cormen's Method and static allocation
 *
 * \param[in]       A: pointer to A matrix object
 * \param[out]       L: pointer to L matrix object
 * \param[out]       U: pointer to U matrix object
 *
 * \retval          UTILS_STATUS_SUCCESS success
 * \retval          UTILS_STATUS_ERROR on a zero pivot, or if a non-finite (NaN/Inf) pivot is encountered
 */
utilsStatus_t LU_CormenStatic(const matrix_t* A, matrix_t* L, matrix_t* U);

/**
 * \brief           Calculate L (lower triangular), U (upper triangular) and P (permutation) matrices such that A = LUP with Cormen's Method and static allocation
 *
 * \param[in]       A: pointer to A matrix object
 * \param[out]       L: pointer to L matrix object
 * \param[out]       U: pointer to U matrix object
 * \param[in]       P: pointer to P matrix object
 *
 * \return          factor to be multiplied by determinant of U to obtain determinant of A, or 0 if A is singular (a zero or non-finite pivot is found)
 */
int8_t LUP_CormenStatic(const matrix_t* A, matrix_t* L, matrix_t* U, matrix_t* P);

/**
 * \brief           Economy Householder QR factorization A = Q*R for a tall matrix with static memory allocation
 *
 * \attention       A must have rows >= cols. Q and R must be distinct from A
 *
 * \param[in]       A: pointer to A matrix object (rows >= cols)
 * \param[out]      Q: pointer to Q matrix object (rows-by-cols, orthonormal columns)
 * \param[out]      R: pointer to R matrix object (cols-by-cols, upper-triangular)
 *
 * \retval          UTILS_STATUS_SUCCESS if the factorization succeeds
 * \retval          UTILS_STATUS_ERROR if A is rank-deficient (a near-zero pivot is found), or if the factorization is non-finite (NaN/Inf)
 */
utilsStatus_t QR_HouseholderStatic(const matrix_t* A, matrix_t* Q, matrix_t* R);

/**
 * \brief           Solve AX = B system using LU factorization with static allocation
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[out]      result: pointer to result matrix object
 *
 * \retval          UTILS_STATUS_SUCCESS if the system is solved
 * \retval          UTILS_STATUS_ERROR if A is singular, or if the computed result is non-finite (NaN/Inf)
 */
utilsStatus_t LinSolveLUStatic(const matrix_t* A, const matrix_t* B, matrix_t* result);

/**
 * \brief           Solve AX = B system using LUP factorization with static allocation
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[out]      result: pointer to result matrix object
 *
 * \retval          UTILS_STATUS_SUCCESS if the system is solved
 * \retval          UTILS_STATUS_ERROR if A is singular, or if the computed result is non-finite (NaN/Inf)
 */
utilsStatus_t LinSolveLUPStatic(const matrix_t* A, const matrix_t* B, matrix_t* result);

/**
 * \brief           Solve AX = B system using Gauss elimination with partial pivoting and static allocation
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[out]      result: pointer to result matrix object
 *
 * \retval          UTILS_STATUS_SUCCESS if the system is solved
 * \retval          UTILS_STATUS_ERROR if A is singular (zero or non-finite pivot), or if the computed result is non-finite (NaN/Inf)
 */
utilsStatus_t LinSolveGaussStatic(const matrix_t* A, const matrix_t* B, matrix_t* result);

/**
 * \brief           Solve AX = B for symmetric positive-definite A using Cholesky factorization with static memory allocation
 *
 * \param[in]       A: pointer to A matrix object (symmetric positive-definite, only lower triangle read)
 * \param[in]       B: pointer to B matrix object (can have multiple columns)
 * \param[out]      result: pointer to result matrix object
 *
 * \retval          UTILS_STATUS_SUCCESS if the system is solved
 * \retval          UTILS_STATUS_ERROR if A is not positive-definite, or if the computed result is non-finite (NaN/Inf)
 */
utilsStatus_t LinSolveCholeskyStatic(const matrix_t* A, const matrix_t* B, matrix_t* result);

/**
 * \brief           Solve discrete-time algebraic Riccati equation `P = A'*P*A - (B'*P*A)'*inv(R + B'*P*B)*B'*P*A + Q` with static allocation
 *
 * \param[in]       A: pointer to A matrix object
 * \param[in]       B: pointer to B matrix object
 * \param[in]       Q: pointer to Q matrix object
 * \param[in]       R: pointer to R matrix object
 * \param[in]       nmax: maximum number of iterations (200 is generally fine, even if it usually converges within 15 iterations)
 * \param[in]       tol: stopping tolerance (1e-6 is generally fine)
 * \param[out]      result: pointer to P matrix object
 *
 * \retval          UTILS_STATUS_SUCCESS success
 * \retval          UTILS_STATUS_TIMEOUT nmax is reached
 */
utilsStatus_t DAREStatic(const matrix_t* A, const matrix_t* B, const matrix_t* Q, const matrix_t* R, uint16_t nmax, float tol, matrix_t* result);

/**
 * \brief           Gauss-Newton sensor calibration with 9 parameters and static allocation
 * 
 * \attention       Approximates Data to a sphere of radius k by calculating 6 gains (s) and 3 biases (b), useful to calibrate some sensors (meas_sphere=S*(meas-B) with S symmetric)
 * \attention       Data has n>=9 rows corresponding to the number of measures and 3 columns corresponding to the 3 axes
 * \attention       b1=out(0,0);
 * b2=out(1,0);
 * b3=out(2,0);
 * s11=out(3,0);
 * s12=out(4,0);
 * s13=out(5,0);
 * s22=out(6,0);
 * s23=out(7,0);
 * s33=out(8,0);
 *
 * \param[in]       Data: pointer to raw data matrix object Data
 * \param[in]       k: radius of sphere to be approximated. If 0, it is calculated automatically
 * \param[in]       X0: pointer to starting vector X0 (usually [0 0 0 1 0 0 1 0 1]). If NULL it is calculated automatically
 * \param[in]       nmax: maximum number of iterations (200 is generally fine, even if it usually converges within 10 iterations)
 * \param[in]       tol: stopping tolerance (1e-6 is generally fine)
 * \param[out]      result: pointer to result matrix object S
 *
 * \retval          UTILS_STATUS_SUCCESS success
 * \retval          UTILS_STATUS_TIMEOUT nmax is reached
 * \retval          UTILS_STATUS_ERROR errors
 */
utilsStatus_t GaussNewton_Sens_Cal_9Static(const matrix_t* Data, float k, const matrix_t* X0, uint16_t nmax, float tol, matrix_t* result);

/**
 * \brief           Gauss-Newton sensor calibration with 6 parameters and static allocation
 * 
 * \attention       Approximates Data to a sphere of radius k by calculating 3 gains (s) and 3 biases (b), useful to calibrate some sensors (meas_sphere=S*(meas-B) with S symmetric)
 * \attention       Data has n>=6 rows corresponding to the number of measures and 3 columns corresponding to the 3 axes
 * \attention       b1=out(0,0);
 * b2=out(1,0);
 * b3=out(2,0);
 * s11=out(3,0);
 * s22=out(4,0);
 * s33=out(5,0);
 *
 * \param[in]       Data: pointer to raw data matrix object Data
 * \param[in]       k: radius of sphere to be approximated. If 0, it is calculated automatically
 * \param[in]       X0: pointer to starting vector X0 (usually [0 0 0 1 1 1]). If NULL it is calculated automatically
 * \param[in]       nmax: maximum number of iterations (200 is generally fine, even if it usually converges within 10 iterations)
 * \param[in]       tol: stopping tolerance (1e-6 is generally fine)
 * \param[out]      result: pointer to result matrix object S
 *
 * \retval          UTILS_STATUS_SUCCESS success
 * \retval          UTILS_STATUS_TIMEOUT nmax is reached
 * \retval          UTILS_STATUS_ERROR errors
 */
utilsStatus_t GaussNewton_Sens_Cal_6Static(const matrix_t* Data, float k, const matrix_t* X0, uint16_t nmax, float tol, matrix_t* result);

/**
 * \brief           Solve AX = B using economy Householder QR (least-squares when rows > cols) with static memory allocation
 *
 * \param[in]       A: pointer to A matrix object (rows >= cols)
 * \param[in]       B: pointer to B matrix object (can have multiple columns)
 * \param[out]      result: pointer to result matrix object
 *
 * \retval          UTILS_STATUS_SUCCESS if the system is solved
 * \retval          UTILS_STATUS_ERROR if A is rank-deficient, or if the computed result is non-finite (NaN/Inf)
 */
utilsStatus_t LinSolveQRStatic(const matrix_t* A, const matrix_t* B, matrix_t* result);

#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ADVUTILS_NUMMETHODS_H */
