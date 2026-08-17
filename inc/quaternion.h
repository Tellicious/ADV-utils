/* BEGIN Header */
/**
 ******************************************************************************
 * \file            quaternion.h
 * \author          Andrea Vivani
 * \brief           Implementation of quaternion manipulation functions
 ******************************************************************************
 * \copyright
 *
 * Copyright 2022 Andrea Vivani
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
#ifndef ADVUTILS_QUATERNION_H
#define ADVUTILS_QUATERNION_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "commonTypes.h"
#include "matrix.h"

/**
 * \defgroup quaternion Quaternion
 * \brief Quaternion manipulation
 * @{
 */

/* Macros --------------------------------------------------------------------*/

#define AVOID_GIMBAL_LOCK

/* Typedefs ------------------------------------------------------------------*/

#ifndef ADVUTILS_QUATERNION_TYPEDEF
#define ADVUTILS_QUATERNION_TYPEDEF

/**
 * \brief           Unit quaternion (q0 + q1 i + q2 j + q3 k)
 */
typedef struct {
    float q0; /**< Scalar (real) component */
    float q1; /**< i component */
    float q2; /**< j component */
    float q3; /**< k component */
#ifdef AVOID_GIMBAL_LOCK
    axis3f_t ea_pre; /**< Previous Euler angles, used to avoid gimbal lock */
#endif
} quaternion_t;

#endif

/* Function prototypes -------------------------------------------------------*/

/**
 * \brief           Normalize quaternion (in place)
 *
 * \param[in]       q: pointer to quaternion object
 */
void quaternionNorm(quaternion_t* q);

/**
 * \brief           Quaternion multiplication qo = qa * qb
 *
 * \attention       qo can be different from qa/qb, or the same as qa/qb
 *
 * \param[in]       qa: pointer to left-hand side quaternion object
 * \param[in]       qb: pointer to right-hand side quaternion object
 * \param[out]      qo: pointer to resulting quaternion object
 */
void quaternionMult(const quaternion_t* qa, const quaternion_t* qb, quaternion_t* qo);

/**
 * \brief           Quaternion rotation
 *
 * \param[in]       qr: pointer to rotation quaternion object
 * \param[in]       qv: pointer to quaternion object to rotate
 * \param[out]      qo: pointer to resulting quaternion object qo = qr' * qv * qr
 */
void quaternionRotation(const quaternion_t* qr, const quaternion_t* qv, quaternion_t* qo);

/**
 * \brief           Quaternion conjugate
 *
 * \param[in]       qa: pointer to left-hand side quaternion object
 * \param[out]      qo: pointer to resulting quaternion object
 */
void quaternionConj(const quaternion_t* qa, quaternion_t* qo);

/**
 * \brief           Convert quaternion to Euler angles
 *
 * \param[in]       qr: pointer to input quaternion object
 * \param[out]      ea: pointer to resulting euler angles
 */
void quaternionToEuler(quaternion_t* qr, axis3f_t* ea);

/**
 * \brief           Build the active rotation matrix (direction cosine matrix) from a quaternion
 *
 * \param[in]       q: pointer to input quaternion object
 * \param[out]      result: pointer to resulting 3-by-3 matrix object (active DCM)
 *
 * \attention       result must be a 3-by-3 matrix. The matrix is the active rotation (transpose of the frame rotation applied by quaternionRotation)
 */
void quaternionToMatrix(const quaternion_t* q, matrix_t* result);

/**
 * \brief           Build a quaternion from a rotation axis and angle
 *
 * \param[in]       axis: pointer to rotation axis (must be a unit vector)
 * \param[in]       angle: rotation angle in radians
 * \param[out]      q: pointer to resulting quaternion object (normalized)
 *
 * \attention       axis is assumed to be a unit vector
 */
void quaternionFromAxisAngle(const axis3f_t* axis, float angle, quaternion_t* q);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ADVUTILS_QUATERNION_H */
