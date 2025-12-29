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
#define ADVUTILS_ABS(value)                               (((value) >= 0) ? (value) : (-value))

/* Get sign of value */
#define ADVUTILS_SIGN(x)                                  (((x) >= 0) ? 1 : -1)

/* Constrain value between low and high */
#define ADVUTILS_CONSTRAIN(value, low, high)              ((value) < (low) ? (low) : ((value) > (high) ? (high) : (value)))

/* Re-map value according to new scale */
#define ADVUTILS_MAP(x, fromLow, fromHigh, toLow, toHigh) (((x) - (fromLow)) * ((toHigh) - (toLow)) / ((fromHigh) - (fromLow)) + (toLow))

/* Apply a deadband to value */
#define ADVUTILS_DEADBAND(value, threshold)               ((ADVUTILS_ABS(value) <= threshold) ? 0 : ((value > 0) ? (value - threshold) : (value + threshold)))

/* Get maximum between 2 values */
#ifdef __GNUC__
#define ADVUTILS_MAX(a, b)                                                                                                                                     \
    ({                                                                                                                                                         \
        __typeof__(a) _a = (a);                                                                                                                                \
        __typeof__(b) _b = (b);                                                                                                                                \
        _a > _b ? _a : _b;                                                                                                                                     \
    })
#else
#define ADVUTILS_MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/* Get minimum between 2 values */
#ifdef __GNUC__
#define ADVUTILS_MIN(a, b)                                                                                                                                     \
    ({                                                                                                                                                         \
        __typeof__(a) _a = (a);                                                                                                                                \
        __typeof__(b) _b = (b);                                                                                                                                \
        _a < _b ? _a : _b;                                                                                                                                     \
    })
#else
#define ADVUTILS_MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/* Conversion between rad and deg */
#define ADVUTILS_RAD2DEG(x)                     ((x) * 57.29578f)

/* Conversion between deg and rad */
#define ADVUTILS_DEG2RAD(x)                     ((x) * 0.0174533f)

/* Conversion between rad/s and milliDegrees-per-second */
#define ADVUTILS_RADPS2MDPS(x)                  ((x) * 57295.779513f)

/* Conversion between milliDegrees-per-second and rad/s */
#define ADVUTILS_MDPS2RADPS(x)                  ((x) * 1.745329252e-5f)

/* Conversion between C and K */
#define ADVUTILS_C2K(x)                         ((x) + 273.15f)

/* Conversion between K and C */
#define ADVUTILS_K2C(x)                         ((x) - 273.15f)

/* Conversion between milliG and m/s^2 */
#define ADVUTILS_MG2MS2(x)                      ((x) * 0.00980665f)

/* Conversion between m/s^2 and milliG  */
#define ADVUTILS_MS22MG(x)                      ((x) * 101.9716212978f)

/* Bit shift left */
#define ADVUTILS_SHIFT(val, shift)              ((val) << (shift))

/* Bit shift left in place */
#define ADVUTILS_SHIFT_IN_PLACE(val, shift)     val <<= (shift)

/* Bit shift right */
#define ADVUTILS_UNSHIFT(val, shift)            ((val) >> (shift))

/* Bit shift right in place */
#define ADVUTILS_UNSHIFT_IN_PLACE(val, shift)   val >>= (shift)

/* Check if all bit are set */
#define ADVUTILS_IS_BIT_SET_ALL(val, mask)      (((val) & (mask)) == (mask))

/* Check if any bit is set */
#define ADVUTILS_IS_BIT_SET_ANY(val, mask)      (((val) & (mask)) != 0)

/* Mask bits */
#define ADVUTILS_BIT_MASK(val, mask)            ((val) & (mask))

/* Set bits */
#define ADVUTILS_BIT_SET(val, mask)             ((val) | (mask))

/* Set bits in place */
#define ADVUTILS_BIT_SET_IN_PLACE(val, mask)    val |= (mask)

/* Clear bits */
#define ADVUTILS_BIT_CLEAR(val, mask)           ((val) & ~(mask))

/* Clear bits in place */
#define ADVUTILS_BIT_CLEAR_IN_PLACE(val, mask)  val &= ~(mask)

/* Toggle bits */
#define ADVUTILS_BIT_TOGGLE(val, mask)          ((val) ^ (mask))

/* Toggle bits in place */
#define ADVUTILS_BIT_TOGGLE_IN_PLACE(val, mask) val ^= (mask)

/* Faster math operations */
#ifdef ADVUTILS_USE_FAST_MATH
#define ADVUTILS_SIN(x)     fastSin(x)
#define ADVUTILS_COS(x)     fastCos(x)
#define ADVUTILS_SQRT(x)    fastSqrt(x)
#define ADVUTILS_INVSQRT(x) fastInvSqrt(x)
#define ADVUTILS_TAN(x)     (ADVUTILS_SIN(x) / ADVUTILS_COS(x))
#else
#define ADVUTILS_SIN(x)     sinf(x)
#define ADVUTILS_COS(x)     cosf(x)
#define ADVUTILS_SQRT(x)    sqrtf(x)
#define ADVUTILS_INVSQRT(x) 1.0f / sqrtf(x)
#define ADVUTILS_TAN(x)     tanf(x)
#endif /* ADVUTILS_USE_FAST_MATH */

/* Constants -----------------------------------------------------------------*/

/* Pi value */
#define ADVUTILS_constPI 3.141592654f

/* G value in m/s^2 */
#define ADVUTILS_constG  9.80665f

/* e value */
#define ADVUTILS_constE  2.71828182845904523536028747135266249f

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
