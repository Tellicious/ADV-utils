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
#ifndef ADVUTILS_BASICMATH_H
#define ADVUTILS_BASICMATH_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "math.h"

/**
 * \defgroup basicMath Basic math
 * \brief Math helper macros and constants
 * @{
 */

/* Macros --------------------------------------------------------------------*/

/** \brief Absolute value */
#define ABS(value)                               (((value) >= 0) ? (value) : (-(value)))

/** \brief Get sign of value */
#define SIGN(x)                                  (((x) >= 0) ? 1 : -1)

/** \brief Constrain value between low and high */
#define CONSTRAIN(value, low, high)              ((value) < (low) ? (low) : ((value) > (high) ? (high) : (value)))

/** \brief Re-map value according to new scale */
#define MAP(x, fromLow, fromHigh, toLow, toHigh) (((x) - (fromLow)) * ((toHigh) - (toLow)) / ((fromHigh) - (fromLow)) + (toLow))

/** \brief Apply a deadband to value */
#define DEADBAND(value, threshold)               ((ABS(value) <= (threshold)) ? 0 : (((value) > 0) ? ((value) - (threshold)) : ((value) + (threshold))))

#ifdef __GNUC__
/** \brief Get maximum between 2 values */
#define MAX(a, b)                                                                                                                                              \
({                                                                                                                                                         \
    __typeof__(a) _a = (a);                                                                                                                                \
    __typeof__(b) _b = (b);                                                                                                                                \
    _a > _b ? _a : _b;                                                                                                                                     \
})
#else
/** \brief Get maximum between 2 values */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifdef __GNUC__
/** \brief Get minimum between 2 values */
#define MIN(a, b)                                                                                                                                              \
({                                                                                                                                                         \
    __typeof__(a) _a = (a);                                                                                                                                \
    __typeof__(b) _b = (b);                                                                                                                                \
    _a < _b ? _a : _b;                                                                                                                                     \
})
#else
/** \brief Get minimum between 2 values */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/** \brief Conversion between rad and deg */
#define RAD2DEG(x)                     ((x) * 57.29578f)

/** \brief Conversion between deg and rad */
#define DEG2RAD(x)                     ((x) * 0.0174533f)

/** \brief Conversion between rad/s and milliDegrees-per-second */
#define RADPS2MDPS(x)                  ((x) * 57295.779513f)

/** \brief Conversion between milliDegrees-per-second and rad/s */
#define MDPS2RADPS(x)                  ((x) * 1.745329252e-5f)

/** \brief Conversion between C and K */
#define C2K(x)                         ((x) + 273.15f)

/** \brief Conversion between K and C */
#define K2C(x)                         ((x) - 273.15f)

/** \brief Conversion between milliG and m/s^2 */
#define MG2MS2(x)                      ((x) * 0.00980665f)

/** \brief Conversion between m/s^2 and milliG  */
#define MS22MG(x)                      ((x) * 101.9716212978f)

/** \brief Bit shift left */
#define SHIFT(val, shift)              ((val) << (shift))

/** \brief Bit shift left in place */
#define SHIFT_IN_PLACE(val, shift)     (val) <<= (shift)

/** \brief Bit shift right */
#define UNSHIFT(val, shift)            ((val) >> (shift))

/** \brief Bit shift right in place */
#define UNSHIFT_IN_PLACE(val, shift)   (val) >>= (shift)

/** \brief Check if all bit are set */
#define IS_BIT_SET_ALL(val, mask)      (((val) & (mask)) == (mask))

/** \brief Check if any bit is set */
#define IS_BIT_SET_ANY(val, mask)      (((val) & (mask)) != 0U)

/** \brief Check if all bit are not set */
#define IS_BIT_NOT_SET_ALL(val, mask)  (((val) & (mask)) != (mask))

/** \brief Check if none bit is set */
#define IS_BIT_NOT_SET_ANY(val, mask)  (((val) & (mask)) == 0U)

/** \brief Mask bits */
#define BIT_MASK(val, mask)            ((val) & (mask))

/** \brief Set bits */
#define BIT_SET(val, mask)             ((val) | (mask))

/** \brief Set bits in place */
#define BIT_SET_IN_PLACE(val, mask)    (val) |= (mask)

/** \brief Clear bits */
#define BIT_CLEAR(val, mask)           ((val) & ~(mask))

/** \brief Clear bits in place */
#define BIT_CLEAR_IN_PLACE(val, mask)  (val) &= ~(mask)

/** \brief Toggle bits */
#define BIT_TOGGLE(val, mask)          ((val) ^ (mask))

/** \brief Toggle bits in place */
#define BIT_TOGGLE_IN_PLACE(val, mask) (val) ^= (mask)

/* Faster math operations */
#ifdef USE_FAST_MATH
/** \brief Sine \hideinitializer */
#define SIN(x)     fastSin(x)
/** \brief Cosine \hideinitializer */
#define COS(x)     fastCos(x)
/** \brief Square root \hideinitializer */
#define SQRT(x)    fastSqrt(x)
/** \brief Inverse square root (1/sqrt) \hideinitializer */
#define INVSQRT(x) fastInvSqrt(x)
/** \brief Tangent \hideinitializer */
#define TAN(x)     (SIN(x) / COS(x))
#else
/** \brief Sine \hideinitializer */
#define SIN(x)     sinf(x)
/** \brief Cosine \hideinitializer */
#define COS(x)     cosf(x)
/** \brief Square root \hideinitializer */
#define SQRT(x)    sqrtf(x)
/** \brief Inverse square root (1/sqrt) \hideinitializer */
#define INVSQRT(x) 1.0f / sqrtf(x)
/** \brief Tangent \hideinitializer */
#define TAN(x)     tanf(x)
#endif /* USE_FAST_MATH */

/* Constants -----------------------------------------------------------------*/

/* Pi value */
#ifdef M_PI
/** \brief pi value */
/* cppcheck-suppress misra-c2012-20.4 ; deviation: false positive - identifier is not a keyword */
#define constPI M_PI
#else
/** \brief pi value */
/* cppcheck-suppress misra-c2012-20.4 ; deviation: false positive - identifier is not a keyword */
#define constPI 3.14159265358979323846f
#endif

#ifdef M_PI_2
/** \brief pi / 2 value */
/* cppcheck-suppress misra-c2012-20.4 ; deviation: false positive - identifier is not a keyword */
#define constPI_2 M_PI_2
#else
/** \brief pi / 2 value */
/* cppcheck-suppress misra-c2012-20.4 ; deviation: false positive - identifier is not a keyword */
#define constPI_2 1.57079632679489661923f
#endif

#ifdef M_TWOPI
/** \brief 2 * pi value */
/* cppcheck-suppress misra-c2012-20.4 ; deviation: false positive - identifier is not a keyword */
#define constTWOPI M_TWOPI
#else
/** \brief 2 * pi value */
/* cppcheck-suppress misra-c2012-20.4 ; deviation: false positive - identifier is not a keyword */
#define constTWOPI 6.28318530717958647692f
#endif

/** \brief G value in m/s^2 */
/* cppcheck-suppress misra-c2012-20.4 ; deviation: false positive - identifier is not a keyword */
#define constG 9.80665f

#ifdef M_E
/** \brief e value */
/* cppcheck-suppress misra-c2012-20.4 ; deviation: false positive - identifier is not a keyword */
#define constE M_E
#else
/** \brief e value */
/* cppcheck-suppress misra-c2012-20.4 ; deviation: false positive - identifier is not a keyword */
#define constE 2.7182818284590452354f
#endif

/* sqrt(2) */
#ifdef M_SQRT2
/** \brief sqrt(2) value */
/* cppcheck-suppress misra-c2012-20.4 ; deviation: false positive - identifier is not a keyword */
#define constSQRT2 M_SQRT2
#else
/** \brief sqrt(2) value */
/* cppcheck-suppress misra-c2012-20.4 ; deviation: false positive - identifier is not a keyword */
#define constSQRT2 1.41421356237309504880f
#endif

#ifdef M_SQRT1_2
/** \brief sqrt(1/2) value */
/* cppcheck-suppress misra-c2012-20.4 ; deviation: false positive - identifier is not a keyword */
#define constSQRT1_2 M_SQRT1_2
#else
/** \brief sqrt(1/2) value */
/* cppcheck-suppress misra-c2012-20.4 ; deviation: false positive - identifier is not a keyword */
#define constSQRT1_2 0.70710678118654752440f
#endif

/* sqrt(3) */
#ifdef M_SQRT3
/** \brief sqrt(3) value */
/* cppcheck-suppress misra-c2012-20.4 ; deviation: false positive - identifier is not a keyword */
#define constSQRT3 M_SQRT3
#else
/** \brief sqrt(3) value */
/* cppcheck-suppress misra-c2012-20.4 ; deviation: false positive - identifier is not a keyword */
#define constSQRT3 1.73205080756887719000f
#endif

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

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ADVUTILS_BASICMATH_H */
