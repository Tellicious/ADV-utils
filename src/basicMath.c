/* BEGIN Header */
/**
 ******************************************************************************
 * \file            basicMath.c
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

/* Includes ------------------------------------------------------------------*/

#include "basicMath.h"

/* Macros --------------------------------------------------------------------*/
#define INT16_BITS (16)
#ifndef INT16_MAX
#define INT16_MAX ((1 << (INT16_BITS - 1)) - 1)
#endif
#define TABLE_BITS   (5)
#define TABLE_SIZE   (1 << TABLE_BITS)
#define TABLE_MASK   (TABLE_SIZE - 1)
#define LOOKUP_BITS  (TABLE_BITS + 2)
#define LOOKUP_MASK  ((1 << LOOKUP_BITS) - 1)
#define FLIP_BIT     (1 << TABLE_BITS)
#define NEGATE_BIT   (1 << (TABLE_BITS + 1))
#define INTERP_BITS  (INT16_BITS - 1 - LOOKUP_BITS)
#define INTERP_MASK  ((1 << INTERP_BITS) - 1)
#define FLOAT2Q15(x) ((x) * 5215.189175235226362f) // x * 32768.0 / (2 * pi))
#define Q152FLOAT(x) ((x) * 3.051850947599719e-5f) // x * 1.0 / (double)((1 << 15) - 1)

/* Private variables ---------------------------------------------------------*/

static int16_t sin90[TABLE_SIZE + 1] = {0x0000, 0x0647, 0x0c8b, 0x12c7, 0x18f8, 0x1f19, 0x2527, 0x2b1e, 0x30fb,
                                        0x36b9, 0x3c56, 0x41cd, 0x471c, 0x4c3f, 0x5133, 0x55f4, 0x5a81, 0x5ed6,
                                        0x62f1, 0x66ce, 0x6a6c, 0x6dc9, 0x70e1, 0x73b5, 0x7640, 0x7883, 0x7a7c,
                                        0x7c29, 0x7d89, 0x7e9c, 0x7f61, 0x7fd7, 0x7fff};

/* Functions -----------------------------------------------------------------*/

/* Fast square-root*/
float fastSqrt(float value) {
    union {
        int i;
        float x;
    } u;

    if (value < 0) {
        return NAN;
    }

    u.x = value;
    u.i = (1 << 29) + (u.i >> 1) - (1 << 22);

    /* Two Babylonian Steps (simplified from:) 
     * u.x = 0.5f * (u.x + x/u.x);
     * u.x = 0.5f * (u.x + x/u.x); */
    u.x = u.x + value / u.x;
    u.x = 0.25f * u.x + value / u.x;

    return u.x;
}

/* 
 * Fast inverse square-root
 * See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
 */

float fastInvSqrt(float value) {
    union {
        float f;
        uint32_t i;
    } conv = {.f = value};

    if (value < 0) {
        return NAN;
    }

    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f *= 1.5f - (value * 0.5f * conv.f * conv.f);
    return conv.f;
}

/* Sin lookup table generation
void generate_tables()
{
  #define Q15 (1.0/(double)((1<<15)-1))
  #define TABLE_SIZE  (1<<5)
  #define SCALER ((M_PI/2.0) / TABLE_SIZE)
  int i;
  printf("static int16_t sin90_offset[TABLE_SIZE+1] = {\n  ");
  for(i=0; i < TABLE_SIZE; i++) {
    printf("0x%04x%s", (int16_t) (sin(SCALER * i) / Q15), (i%8!=7) ? "," : ",\n  ");
  }
  printf("0x7fff\n};\n");
}
*/

float fastSin(float angle) {
    int16_t v0, v1;
    int16_t angleInt = FLOAT2Q15(angle);
    if (angleInt < 0) {
        angleInt += INT16_MAX;
        angleInt += 1;
    }
    v0 = (angleInt >> INTERP_BITS);
    if (v0 & FLIP_BIT) {
        v0 = ~v0;
        v1 = ~angleInt;
    } else {
        v1 = angleInt;
    }
    v0 &= TABLE_MASK;
    v1 = sin90[v0] + (int16_t)(((int32_t)(sin90[v0 + 1] - sin90[v0]) * (v1 & INTERP_MASK)) >> INTERP_BITS);
    if ((angleInt >> INTERP_BITS) & NEGATE_BIT) {
        v1 = -v1;
    }
    return (float)Q152FLOAT(v1);
}

float fastCos(float angle) { return fastSin(angle + 1.570796326794897f); }
