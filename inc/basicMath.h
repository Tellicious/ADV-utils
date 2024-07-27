/* BEGIN Header */
/**
 ******************************************************************************
 * \file            basicMath.h
 * \author          Andrea Vivani
 * \brief           Implementation of useful math operations and constants
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
#ifndef __BASIC_MATH_H__
#define __BASIC_MATH_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "math.h"

/* Macros --------------------------------------------------------------------*/

/* Absolute value */
#define ABS(value)                  (((value) >= 0) ? (value) : (-value))

/* Get sign of value */
#define SIGN(x)                     (((x) >= 0) ? 1 : -1)

/* Constrain value between low and high */
#define CONSTRAIN(value, low, high) ((value) < (low) ? (low) : ((value) > (high) ? (high) : (value)))

/* Re-map value according to new scale */
#define MAP(x, fromLow, fromHigh, toLow, toHigh)                                                                       \
    (((x) - (fromLow)) * ((toHigh) - (toLow)) / ((fromHigh) - (fromLow)) + (fromLow))

/* Apply a deadband to value */
#define DEADBAND(value, threshold)                                                                                     \
    ((ABS(value) <= threshold) ? 0 : ((value > 0) ? (value - threshold) : (value + threshold)))

/* Get maximum between 2 values */
#ifdef __GNUC__
#define MAX(a, b)                                                                                                      \
    ({                                                                                                                 \
        __typeof__(a) _a = (a);                                                                                        \
        __typeof__(b) _b = (b);                                                                                        \
        _a > _b ? _a : _b;                                                                                             \
    })
#elif
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/* Get minimum between 2 values */
#ifdef __GNUC__
#define MIN(a, b)                                                                                                      \
    ({                                                                                                                 \
        __typeof__(a) _a = (a);                                                                                        \
        __typeof__(b) _b = (b);                                                                                        \
        _a < _b ? _a : _b;                                                                                             \
    })
#elif
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/* Conversion between rad and deg */
#define RAD2DEG(x)                     ((x) * 57.29578f)

/* Conversion between deg and rad */
#define DEG2RAD(x)                     ((x) * 0.0174533f)

/* Conversion between rad/s and milliDegrees-per-second */
#define RADPS2MDPS(x)                  ((x) * 57295.779513f)

/* Conversion between milliDegrees-per-second and rad/s */
#define MDPS2RADPS(x)                  ((x) * 1.745329252e-5f)

/* Conversion between C and K */
#define C2K(x)                         ((x) + 273.15f)

/* Conversion between K and C */
#define K2C(x)                         ((x) - 273.15f)

/* Conversion between milliG and m/s^2 */
#define MG2MS2(x)                      ((x) * 0.00980665f)

/* Conversion between m/s^2 and milliG  */
#define MS22MG(x)                      ((x) * 101.9716212978f)

/* Bit shift left */
#define SHIFT(val, shift)              ((val) << (shift))

/* Bit shift left in place */
#define SHIFT_IN_PLACE(val, shift)     val <<= (shift)

/* Bit shift right */
#define UNSHIFT(val, shift)            ((val) >> (shift))

/* Bit shift right in place */
#define UNSHIFT_IN_PLACE(val, shift)   val >>= (shift)

/* Check if all bit are set */
#define IS_BIT_SET_ALL(val, mask)      (((val) & (mask)) == (mask))

/* Check if any bit is set */
#define IS_BIT_SET_ANY(val, mask)      (((val) & (mask)) != 0)

/* Mask bits */
#define BIT_MASK(val, mask)            ((val) & (mask))

/* Set bits */
#define BIT_SET(val, mask)             ((val) | (mask))

/* Set bits in place */
#define BIT_SET_IN_PLACE(val, mask)    val |= (mask)

/* Clear bits */
#define BIT_CLEAR(val, mask)           ((val) & ~(mask))

/* Clear bits in place */
#define BIT_CLEAR_IN_PLACE(val, mask)  val &= ~(mask)

/* Toggle bits */
#define BIT_TOGGLE(val, mask)          ((val) ^ (mask))

/* Toggle bits in place */
#define BIT_TOGGLE_IN_PLACE(val, mask) val ^= (mask)

/* Faster math operations */
#ifdef USE_FAST_MATH
#define SIN(x)     fastSin(x)
#define COS(x)     fastCos(x)
#define SQRT(x)    fastSqrt(x)
#define INVSQRT(x) fastInvSqrt(x)
#define TAN(x)     (SIN(x) / COS(x))
#else
#define SIN(x)     sinf(x)
#define COS(x)     cosf(x)
#define SQRT(x)    sqrtf(x)
#define INVSQRT(x) 1.0f / sqrtf(x)
#define TAN(x)     tanf(x)
#endif /* USE_FAST_MATH */

/* Constants -----------------------------------------------------------------*/

/* Pi value */
#define constPI 3.141592654f

/* G value in m/s^2 */
#define constG  9.80665f

/* e value */
#define constE  2.71828182845904523536028747135266249f

/* Functions -----------------------------------------------------------------*/

/**
 * \brief           Perform square root
 *
 * \param[in]       value: input value
 *
 * \return          square root of value
 */
float fastSqrt(float value);

/**
 * \brief           Perform fast inverse square root
 *
 * \param[in]       value: input value
 *
 * \return          inverse square root of value
 */
float fastInvSqrt(float value);

/**
 * \brief           Perform fast sine calculation
 *
 * \param[in]       angle: value of angle in radians
 *
 * \return          sine value
 */
float fastSin(float angle);

/**
 * \brief           Perform fast cosine calculation
 *
 * \param[in]       angle: value of angle in radians
 *
 * \return          cosine value
 */
float fastCos(float angle);

#ifdef __cplusplus
}
#endif

#endif /* __BASIC_MATH_H__ */
