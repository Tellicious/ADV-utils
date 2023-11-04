/* BEGIN Header */
/**
 ******************************************************************************
 * \file    quaternion.h
 * \author  Andrea Vivani
 * \brief   Implementation of quaternion manipulation functions
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

/* Includes ------------------------------------------------------------------*/

#include "quaternion.h"
#include "basicMath.h"
#include <math.h>

/* Macros --------------------------------------------------------------------*/

#define PI_2    1.57

/* Private variables ---------------------------------------------------------*/

#ifdef AVOID_GIMBAL_LOCK
static eulerAngles_t ea_pre;
#endif

/* Functions -----------------------------------------------------------------*/

void quaternionNorm(quaternion_t *q)
{
    float norm;
    
    norm = fastInvSqrt(q->q0 * q->q0 + q->q1 * q->q1 + q->q2 * q->q2 + q->q3 * q->q3);
    q->q0 *= norm;
    q->q1 *= norm;
    q->q2 *= norm;
    q->q3 *= norm;
}

void quaternionMult(quaternion_t *qa, quaternion_t *qb, quaternion_t *qo)
{
    float q0, q1, q2, q3;
    
    q0 = qa->q0 * qb->q0 - qa->q1 * qb->q1 - qa->q2 * qb->q2 - qa->q3 * qb->q3;
    q1 = qa->q0 * qb->q1 + qa->q1 * qb->q0 + qa->q2 * qb->q3 - qa->q3 * qb->q2;
    q2 = qa->q0 * qb->q2 - qa->q1 * qb->q3 + qa->q2 * qb->q0 + qa->q3 * qb->q1;
    q3 = qa->q0 * qb->q3 + qa->q1 * qb->q2 - qa->q2 * qb->q1 + qa->q3 * qb->q0;
    qo->q0 = q0;
    qo->q1 = q1;
    qo->q2 = q2;
    qo->q3 = q3;
}

void quaternionRotation(quaternion_t *qr, quaternion_t *qv, quaternion_t *qo)
{
    float q0q0, q1q1, q2q2, q3q3;
    float dq0, dq1, dq2;
    float dq1q2, dq1q3, dq0q2, dq0q3;
    float dq0q1, dq2q3;
    
    q0q0 = qr->q0 * qr->q0;
    q1q1 = qr->q1 * qr->q1;
    q2q2 = qr->q2 * qr->q2;
    q3q3 = qr->q3 * qr->q3;
    dq0 = 2 * qr->q0;
    dq1 = 2 * qr->q1;
    dq2 = 2 * qr->q2;
    dq1q2 = dq1 * qr->q2;
    dq1q3 = dq1 * qr->q3;
    dq0q2 = dq0 * qr->q2;
    dq0q3 = dq0 * qr->q3;
    dq0q1 = dq0 * qr->q1;
    dq2q3 = dq2 * qr->q3;
    
    qo->q0 = 0;
    qo->q1 = (q0q0 + q1q1 - q2q2 - q3q3) * qv->q1 + (dq1q2 + dq0q3) * qv->q2 + (dq1q3 - dq0q2) * qv->q3;
    qo->q2 = (dq1q2 - dq0q3) * qv->q1 + (q0q0 + q2q2 - q1q1 - q3q3) * qv->q2 + (dq0q1 + dq2q3) * qv->q3;
    qo->q3 = (dq0q2 + dq1q3) * qv->q1 + (dq2q3 - dq0q1) * qv->q2 + (q0q0 + q3q3 - q1q1 - q2q2) * qv->q3;
}

void quaternionConj(quaternion_t *qa, quaternion_t *qo)
{
    qo->q0 = qa->q0;
    qo->q1 = -qa->q1;
    qo->q2 = -qa->q2;
    qo->q3 = -qa->q3;
}

void quaternionToEuler(quaternion_t *qr, eulerAngles_t *ea)
{
    float q0q0, q1q1, q2q2, q3q3;
    float dq0, dq1, dq2;
    float dq1q3, dq0q2;
    float dq0q1, dq2q3;
#ifdef AVOID_GIMBAL_LOCK
    float dq1q2, dq0q3;
#endif
    
    q0q0 = qr->q0 * qr->q0;
    q1q1 = qr->q1 * qr->q1;
    q2q2 = qr->q2 * qr->q2;
    q3q3 = qr->q3 * qr->q3;
    dq0 = 2 * qr->q0;
    dq1 = 2 * qr->q1;
    dq2 = 2 * qr->q2;
    dq1q3 = dq1 * qr->q3;
    dq0q2 = dq0 * qr->q2;
    dq0q1 = dq0 * qr->q1;
    dq2q3 = dq2 * qr->q3;
#ifdef AVOID_GIMBAL_LOCK
    dq1q2 = dq1 * qr->q2;
    dq0q3 = dq0 * qr->q3;
#endif
    
    ea->thx = atan2f(dq0q1 + dq2q3, q0q0 + q3q3 - q1q1 - q2q2);
    ea->thy = asinf(dq0q2 - dq1q3);
    
    /* This part is needed  to manage angle >90 deg */
#ifdef AVOID_GIMBAL_LOCK
    if(ea->thx > PI_2 || ea->thx < -PI_2)
        ea->thx = ea_pre.thx;
    if(ea->thy > PI_2 || ea->thy < -PI_2)
        ea->thy = ea_pre.thy;
    
    ea_pre.thx = ea->thx;
    ea_pre.thy = ea->thy;
    ea->thz = atan2f(dq1q2+dq0q3, q0q0+q1q1-q2q2-q3q3);
#endif
}
