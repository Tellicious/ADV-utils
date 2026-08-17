/* BEGIN Header */
/**
 ******************************************************************************
 * \file            commonTypes.h
 * \author          Andrea Vivani
 * \brief           Implementation of common type definitions
 ******************************************************************************
 * \copyright
 *
 * Copyright 2023 Andrea Vivani
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
#ifndef ADVUTILS_COMMONTYPES_H
#define ADVUTILS_COMMONTYPES_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/**
 * \defgroup commonTypes Common types
 * \brief Shared types and status codes
 * @{
 */

/* Typedefs ------------------------------------------------------------------*/

/**
 * \brief           Generic return status shared by all ADV-utils modules
 */
typedef enum {
    UTILS_STATUS_SUCCESS = 0,  /**< Operation completed successfully */
    UTILS_STATUS_ERROR,        /**< Generic failure (e.g. NULL pointer or invalid argument) */
    UTILS_STATUS_WARNING,      /**< Completed, but a non-fatal condition was detected */
    UTILS_STATUS_ONGOING,      /**< Operation still in progress; call again */
    UTILS_STATUS_TIMEOUT,      /**< Did not converge/complete within the allotted iterations */
    UTILS_STATUS_EMPTY,        /**< Container is empty (nothing to read or remove) */
    UTILS_STATUS_FULL,         /**< Container is full (no room to add) */
    UTILS_STATUS_BUCKET_EMPTY, /**< Requested hash-table bucket holds no entry */
} utilsStatus_t;

/**
 * \brief           3-axis floating-point vector
 */
typedef struct {
    float x; /**< X component */
    float y; /**< Y component */
    float z; /**< Z component */
} axis3f_t;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ADVUTILS_COMMONTYPES_H */
