/* BEGIN Header */
/**
 ******************************************************************************
 * \file            vector3f.c
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

/* Includes ------------------------------------------------------------------*/

#include "vector3f.h"
#include "basicMath.h"

/* Functions -----------------------------------------------------------------*/

void vector3fAdd(const axis3f_t* a, const axis3f_t* b, axis3f_t* out) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

void vector3fSub(const axis3f_t* a, const axis3f_t* b, axis3f_t* out) {
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

void vector3fScale(const axis3f_t* a, float k, axis3f_t* out) {
    out->x = a->x * k;
    out->y = a->y * k;
    out->z = a->z * k;
}

float vector3fDot(const axis3f_t* a, const axis3f_t* b) { return (a->x * b->x) + (a->y * b->y) + (a->z * b->z); }

float vector3fNorm(const axis3f_t* a) { return SQRT(vector3fDot(a, a)); }

void vector3fNormalize(const axis3f_t* a, axis3f_t* out) {
    float inv_norm = INVSQRT(vector3fDot(a, a));

    if (isnan(inv_norm) || isinf(inv_norm)) {
        inv_norm = 0.0f;
    }

    out->x = a->x * inv_norm;
    out->y = a->y * inv_norm;
    out->z = a->z * inv_norm;
}

void vector3fCross(const axis3f_t* a, const axis3f_t* b, axis3f_t* out) {
    float x = (a->y * b->z) - (a->z * b->y);
    float y = (a->z * b->x) - (a->x * b->z);
    float z = (a->x * b->y) - (a->y * b->x);

    out->x = x;
    out->y = y;
    out->z = z;
}
