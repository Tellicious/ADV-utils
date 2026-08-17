/* BEGIN Header */
/**
 ******************************************************************************
 * \file            vector3f.h
 * \author          Andrea Vivani
 * \brief           Implementation of 3-element vector manipulation functions
 ******************************************************************************
 * \copyright
 *
 * Copyright 2026 Andrea Vivani
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
#ifndef ADVUTILS_VECTOR3F_H
#define ADVUTILS_VECTOR3F_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "commonTypes.h"

/* Function prototypes -------------------------------------------------------*/

/**
 * \brief           Vector addition out = a + b
 *
 * \param[in]       a: pointer to left-hand side vector object
 * \param[in]       b: pointer to right-hand side vector object
 * \param[out]      out: pointer to resulting vector object (can alias a or b)
 */
void vector3fAdd(const axis3f_t* a, const axis3f_t* b, axis3f_t* out);

/**
 * \brief           Vector subtraction out = a - b
 *
 * \param[in]       a: pointer to left-hand side vector object
 * \param[in]       b: pointer to right-hand side vector object
 * \param[out]      out: pointer to resulting vector object (can alias a or b)
 */
void vector3fSub(const axis3f_t* a, const axis3f_t* b, axis3f_t* out);

/**
 * \brief           Vector scaling out = k * a
 *
 * \param[in]       a: pointer to input vector object
 * \param[in]       k: scalar factor
 * \param[out]      out: pointer to resulting vector object (can alias a)
 */
void vector3fScale(const axis3f_t* a, float k, axis3f_t* out);

/**
 * \brief           Vector dot product
 *
 * \param[in]       a: pointer to left-hand side vector object
 * \param[in]       b: pointer to right-hand side vector object
 *
 * \return          scalar dot product a . b
 */
float vector3fDot(const axis3f_t* a, const axis3f_t* b);

/**
 * \brief           Vector Euclidean norm
 *
 * \param[in]       a: pointer to input vector object
 *
 * \return          Euclidean norm of a
 */
float vector3fNorm(const axis3f_t* a);

/**
 * \brief           Normalize a vector to unit length
 *
 * \param[in]       a: pointer to input vector object
 * \param[out]      out: pointer to resulting unit vector object (can alias a)
 *
 * \attention       if a has zero norm, out is set to the zero vector
 */
void vector3fNormalize(const axis3f_t* a, axis3f_t* out);

/**
 * \brief           Vector cross product out = a x b
 *
 * \param[in]       a: pointer to left-hand side vector object
 * \param[in]       b: pointer to right-hand side vector object
 * \param[out]      out: pointer to resulting vector object (can alias a or b)
 */
void vector3fCross(const axis3f_t* a, const axis3f_t* b, axis3f_t* out);

#ifdef __cplusplus
}
#endif

#endif /* ADVUTILS_VECTOR3F_H */
