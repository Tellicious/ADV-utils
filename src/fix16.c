/* BEGIN Header */
/**
 ******************************************************************************
 * \file            fix16.c
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

/* Includes ------------------------------------------------------------------*/
#include "fix16.h"

/* Functions -----------------------------------------------------------------*/

fix16_t fix16_add(fix16_t a, fix16_t b) {
    /* perform the addition in a wider type to detect overflow safely */
    int64_t sum = (int64_t)a + (int64_t)b;
    if (sum > (int64_t)FIX16_MAX) {
        return FIX16_MAX;
    }
    if (sum < (int64_t)FIX16_MIN) {
        return FIX16_MIN;
    }
    return (fix16_t)sum;
}

fix16_t fix16_sub(fix16_t a, fix16_t b) {
    int64_t diff = (int64_t)a - (int64_t)b;
    if (diff > (int64_t)FIX16_MAX) {
        return FIX16_MAX;
    }
    if (diff < (int64_t)FIX16_MIN) {
        return FIX16_MIN;
    }
    return (fix16_t)diff;
}

fix16_t fix16_mul(fix16_t a, fix16_t b) {
    int64_t product = (int64_t)a * (int64_t)b;
    /* round half away from zero using truncation toward zero (not floor) so negatives are symmetric */
    int64_t result;
    if (product >= 0) {
        /* cppcheck-suppress misra-c2012-10.1 ; deviation: round-to-nearest via signed arithmetic shift after Q16.16 multiply */
        result = (product + ((int64_t)1 << (FIX16_FRAC_BITS - 1))) >> FIX16_FRAC_BITS;
    } else {
        /* cppcheck-suppress misra-c2012-10.1 ; deviation: round-to-nearest via signed arithmetic shift after Q16.16 multiply (negative branch) */
        result = -(((-product) + ((int64_t)1 << (FIX16_FRAC_BITS - 1))) >> FIX16_FRAC_BITS);
    }
    if (result > (int64_t)FIX16_MAX) {
        return FIX16_MAX;
    }
    if (result < (int64_t)FIX16_MIN) {
        return FIX16_MIN;
    }
    return (fix16_t)result;
}

fix16_t fix16_div(fix16_t a, fix16_t b) {
    if (b == 0) {
        return (a >= 0) ? FIX16_MAX : FIX16_MIN;
    }
    /* cppcheck-suppress misra-c2012-10.1 ; deviation: signed left shift to scale numerator into Q16.16 before divide */
    int64_t num = (int64_t)a << FIX16_FRAC_BITS;
    int64_t quot = num / (int64_t)b;
    int64_t rem = num % (int64_t)b;
    /* round half away from zero */
    int64_t absRem = (rem < 0) ? -rem : rem;
    int64_t absB = (b < 0) ? -(int64_t)b : (int64_t)b;
    if ((absRem * 2) >= absB) {
        quot += (((num < 0) == (b < 0)) ? 1 : -1);
    }
    if (quot > (int64_t)FIX16_MAX) {
        return FIX16_MAX;
    }
    if (quot < (int64_t)FIX16_MIN) {
        return FIX16_MIN;
    }
    return (fix16_t)quot;
}

fix16_t fix16_sqrt(fix16_t x) {
    if (x < 0) {
        return 0; /* undefined; callers guard non-positive operands before calling */
    }
    /* integer sqrt of (x << 16): result R satisfies R/2^16 = sqrt(x/2^16) */
    uint64_t n = ((uint64_t)(uint32_t)x) << FIX16_FRAC_BITS;
    uint64_t res = 0;
    uint64_t bit = (uint64_t)1 << 46; /* highest even bit not exceeding the 2^47 range */
    while (bit > n) {
        bit >>= 2;
    }
    while (bit != 0U) {
        if (n >= (res + bit)) {
            n -= res + bit;
            res = (res >> 1) + bit;
        } else {
            res >>= 1;
        }
        bit >>= 2;
    }
    return (fix16_t)res;
}

fix16_t fix16_abs(fix16_t x) {
    if (x == FIX16_MIN) {
        return FIX16_MAX;
    }
    return (x < 0) ? -x : x;
}

uint8_t fix16_saturated(fix16_t v) { return ((v == FIX16_MAX) || (v == FIX16_MIN)) ? 1U : 0U; }

fix16_t fix16_fromInt(int16_t value) {
    /* cppcheck-suppress misra-c2012-10.1 ; deviation: signed left shift to scale integer into Q16.16 */
    return (fix16_t)((int32_t)value << FIX16_FRAC_BITS);
}

int16_t fix16_toInt(fix16_t value) {
    if (value >= 0) {
        /* cppcheck-suppress misra-c2012-10.1 ; deviation: round-to-nearest via signed arithmetic shift when converting Q16.16 to int */
        return (int16_t)((value + (FIX16_ONE >> 1)) >> FIX16_FRAC_BITS);
    }
    /* symmetric round half away from zero for negatives (truncation toward zero, not floor) */
    /* cppcheck-suppress[misra-c2012-10.1,misra-c2012-10.8] ; deviation: round-to-nearest via signed arithmetic shift and narrowing composite cast when converting Q16.16 to int (negative branch) */
    return (int16_t)(-((int64_t)(((int64_t)(-(int64_t)value) + (int64_t)(FIX16_ONE >> 1)) >> FIX16_FRAC_BITS)));
}

fix16_t fix16_fromFloat(float value) {
    float scaled = value * 65536.0f;
    scaled += (value >= 0.0f) ? 0.5f : -0.5f;
    if (scaled >= 2147483647.0f) {
        return FIX16_MAX;
    }
    if (scaled <= -2147483648.0f) {
        return FIX16_MIN;
    }
    return (fix16_t)scaled;
}

float fix16_toFloat(fix16_t value) { return (float)value / 65536.0f; }

fix16_t fix16_satRoundQ32(int64_t acc, uint8_t* sat) {
    /* round half away from zero using truncation toward zero (not floor) so negatives are symmetric */
    int64_t r;
    if (acc >= 0) {
        /* cppcheck-suppress misra-c2012-10.1 ; deviation: round-to-nearest via signed arithmetic shift in Q16.16 saturating round */
        r = (acc + ((int64_t)1 << (FIX16_FRAC_BITS - 1))) >> FIX16_FRAC_BITS;
    } else {
        /* cppcheck-suppress misra-c2012-10.1 ; deviation: round-to-nearest via signed arithmetic shift in Q16.16 saturating round (negative branch) */
        r = -(((-acc) + ((int64_t)1 << (FIX16_FRAC_BITS - 1))) >> FIX16_FRAC_BITS);
    }
    if (r > (int64_t)FIX16_MAX) {
        *sat = 1U;
        return FIX16_MAX;
    }
    if (r < (int64_t)FIX16_MIN) {
        *sat = 1U;
        return FIX16_MIN;
    }
    return (fix16_t)r;
}
