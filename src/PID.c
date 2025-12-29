/* BEGIN Header */
/**
 ******************************************************************************
 * \file            PID.c
 * \author          Andrea Vivani
 * \brief           Implementation of PID controller with several anti-windup
 *                  options
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

#include "PID.h"
#include "basicMath.h"

/* Functions -----------------------------------------------------------------*/

void PID_init(PID_t* PID, float kpVal, float kiVal, float kdVal, float ndVal, float kbVal, float dT_ms, float satMin, float satMax) {
    PID->dT = dT_ms * 1e-3f;
    PID->kp = kpVal;
    PID->ki = 0.5f * kiVal * PID->dT;
    PID->nd = ndVal;
    PID->kb = 0.5f * kbVal * PID->dT;
    PID->kd = (2 * kdVal * ndVal) / (2 + ndVal * PID->dT);
    PID->kf = (2 - ndVal * PID->dT) / (2 + ndVal * PID->dT);
    PID_setIntegralSaturation(PID, satMin, satMax);
    PID->DuD = 0;
    PID->DuI = 0;
}

void PID_calc(PID_t* PID, float setPoint, float measure) {
    float e = setPoint - measure;
    PID->DuI += PID->ki * e;
    PID->DuD += PID->kd * e;
    PID->output = ADVUTILS_CONSTRAIN(PID->kp * e + PID->DuI + PID->DuD, PID->satMin, PID->satMax);
    PID->DuI += PID->ki * e;
    PID->DuD = PID->kf * PID->DuD - PID->kd * e;
}

utilsStatus_t PID_calcAeroClamp(PID_t* PID, float setPoint, float measure) {
    float e = setPoint - measure;
    PID->DuI += PID->ki * e;
    PID->DuI = ADVUTILS_CONSTRAIN(PID->DuI, PID->satMin, PID->satMax);
    PID->DuD += PID->kd * e;
    PID->output = PID->kp * e + PID->DuI + PID->DuD;
    /* Prepare variables for next step */
    PID->DuD = PID->kf * PID->DuD - PID->kd * e;
    if ((PID->DuI == PID->satMin) || (PID->DuI == PID->satMax)) {
        return UTILS_STATUS_FULL;
    } else {
        PID->DuI += PID->ki * e;
        return UTILS_STATUS_SUCCESS;
    }
}

utilsStatus_t PID_calcIntegralClamp(PID_t* PID, float setPoint, float measure) {
    float e = setPoint - measure;
    PID->DuI += PID->ki * e;
    PID->DuD += PID->kd * e;
    PID->output = PID->kp * e + PID->DuI + PID->DuD;
    if (((e * PID->output) > 0) && ((PID->output < PID->satMin) || (PID->output > PID->satMax))) {
        PID->DuI -= PID->ki * e;
        PID->output -= PID->ki * e;
    } else {
        PID->DuI += PID->ki * e;
    }
    PID->output = ADVUTILS_CONSTRAIN(PID->output, PID->satMin, PID->satMax);
    /* Prepare variables for next step */
    PID->DuD = PID->kf * PID->DuD - PID->kd * e;
    return (((PID->output == PID->satMax) || (PID->output == PID->satMin)) ? UTILS_STATUS_FULL : UTILS_STATUS_SUCCESS);
}

utilsStatus_t PID_calcBackCalc(PID_t* PID, float setPoint, float measure) {
    float bcVal;
    float e = setPoint - measure;
    PID->DuD += PID->kd * e;
    PID->output = PID->kp * e + PID->DuI + PID->DuD;
    if (PID->output > PID->satMax) {
        bcVal = PID->satMax - PID->output;
    } else if (PID->output < PID->satMin) {
        bcVal = PID->satMin - PID->output;
    } else {
        bcVal = 0;
    }
    PID->DuI += PID->ki * e + PID->kb * bcVal;
    PID->output = PID->kp * e + PID->DuI + PID->DuD;
    PID->output = ADVUTILS_CONSTRAIN(PID->output, PID->satMin, PID->satMax);
    /* Prepare variables for next step */
    PID->DuI += PID->ki * e + PID->kb * bcVal;
    PID->DuD = PID->kf * PID->DuD - PID->kd * e;
    return (((PID->output == PID->satMax) || (PID->output == PID->satMin)) ? UTILS_STATUS_FULL : UTILS_STATUS_SUCCESS);
}
