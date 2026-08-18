/* BEGIN Header */
/**
 ******************************************************************************
 * \file            fix16.h
 * \author          Andrea Vivani
 * \brief           Q16.16 fixed-point scalar type and core arithmetic
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
#ifndef ADVUTILS_FIX16_H
#define ADVUTILS_FIX16_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/**
 * \defgroup fix16 Fixed-point (Q16.16)
 * \brief Signed 16.16 fixed-point scalar type and saturating arithmetic
 * @{
 */

/* Macros --------------------------------------------------------------------*/

/** \brief Number of fractional bits */
#define FIX16_FRAC_BITS (16)
/** \brief Value of 1.0 in Q16.16 */
#define FIX16_ONE       ((fix16_t)0x00010000)
/** \brief Largest representable value (~32767.99998) */
#define FIX16_MAX       ((fix16_t)0x7FFFFFFF)
/** \brief Smallest representable value (-32768.0); also used as the overflow sentinel */
#define FIX16_MIN       ((fix16_t)0x80000000U)

/* Typedefs ------------------------------------------------------------------*/

/** \brief Signed Q16.16 fixed-point scalar (16 integer bits, 16 fractional bits) */
typedef int32_t fix16_t;

/* Function prototypes -------------------------------------------------------*/

/**
 * \brief           Saturating Q16.16 addition
 * \param[in]       a: first operand
 * \param[in]       b: second operand
 * \return          a + b, saturated to FIX16_MAX/FIX16_MIN on overflow
 */
fix16_t fix16_add(fix16_t a, fix16_t b);

/**
 * \brief           Saturating Q16.16 subtraction
 * \param[in]       a: minuend
 * \param[in]       b: subtrahend
 * \return          a - b, saturated to FIX16_MAX/FIX16_MIN on overflow
 */
fix16_t fix16_sub(fix16_t a, fix16_t b);

/**
 * \brief           Rounded, saturating Q16.16 multiplication
 * \param[in]       a: first operand
 * \param[in]       b: second operand
 * \return          a * b (round half away from zero), saturated to FIX16_MAX/FIX16_MIN on overflow
 */
fix16_t fix16_mul(fix16_t a, fix16_t b);

/**
 * \brief           Rounded, saturating Q16.16 division
 * \param[in]       a: dividend
 * \param[in]       b: divisor
 * \return          a / b (round half away from zero), saturated to FIX16_MAX/FIX16_MIN on overflow or if b == 0
 */
fix16_t fix16_div(fix16_t a, fix16_t b);

/**
 * \brief           Q16.16 square root (integer-only)
 * \param[in]       x: non-negative operand
 * \return          sqrt(x); returns 0 if x < 0 (callers guard non-positive operands)
 */
fix16_t fix16_sqrt(fix16_t x);

/**
 * \brief           Absolute value
 * \param[in]       x: operand
 * \return          |x|, saturated to FIX16_MAX if x == FIX16_MIN
 */
fix16_t fix16_abs(fix16_t x);

/**
 * \brief           Test whether a value sits on a saturation rail (overflow indicator)
 * \param[in]       v: Q16.16 operand
 * \return          1 if v equals FIX16_MAX or FIX16_MIN, 0 otherwise
 */
uint8_t fix16_saturated(fix16_t v);

/**
 * \brief           Convert an integer to Q16.16
 * \param[in]       value: integer value in [-32768, 32767]
 * \return          value expressed in Q16.16
 */
fix16_t fix16_fromInt(int16_t value);

/**
 * \brief           Convert Q16.16 to the nearest integer
 * \param[in]       value: Q16.16 operand
 * \return          value rounded to the nearest integer
 */
int16_t fix16_toInt(fix16_t value);

/**
 * \brief           Convert a float to Q16.16 (API-boundary bridge only)
 * \param[in]       value: float operand
 * \return          value expressed in Q16.16, saturated to the representable range
 */
fix16_t fix16_fromFloat(float value);

/**
 * \brief           Convert Q16.16 to float (API-boundary bridge only)
 * \param[in]       value: Q16.16 operand
 * \return          value expressed as a float
 */
float fix16_toFloat(fix16_t value);

/**
 * \brief           Round a Q32.32 (int64) accumulator to a saturated Q16.16 value
 * \param[in]       acc: accumulator in Q32.32 (e.g. a sum of Q16.16*Q16.16 products)
 * \param[in,out]   sat: set to 1 if the result saturated; left unchanged otherwise
 * \return          acc rounded to Q16.16, saturated to FIX16_MAX/FIX16_MIN on overflow
 */
fix16_t fix16_satRoundQ32(int64_t acc, uint8_t* sat);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ADVUTILS_FIX16_H */
