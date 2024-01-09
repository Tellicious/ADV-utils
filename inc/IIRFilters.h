/* BEGIN Header */
/**
 ******************************************************************************
 * \file            IIRFilters.h
 * \author          Andrea Vivani
 * \brief           Implementation of simple discrete-time IIR filters
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
#ifndef __IIRFILTER_H__
#define __IIRFILTER_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/* Macros --------------------------------------------------------------------*/

/* Typedefs ------------------------------------------------------------------*/

/**
 * Generic IIR filter struct
 */
typedef struct {
    float d1, d2, d3;     /* denominator coefficients */
    float n0, n1, n2, n3; /* numerator coefficients */
    float i1, i2, i3;     /* previous input values */
    float o1, o2, o3;     /* previous output values */
} IIRFilterGeneric_t;

/**
 * Derivative filter struct
 */
typedef struct {
    float d1;     /* denominator coefficient */
    float n0;     /* numerator coefficient */
    float i1;     /* previous input value */
    float output; /* output value */
} IIRFilterDerivative_t;

/**
 * Integrator filter struct
 */
typedef struct {
    float n0;     /* numerator coefficients */
    float i1;     /* previous input values */
    float output; /* output values */
} IIRFilterIntegrator_t;

/* Function prototypes --------------------------------------------------------*/

/**
 * \brief           Initialize generic IIR filter with custom coefficients
 *
 *
 * \param[in]       filter: pointer to IIR filter structure
 * \param[in]       n0...n3: IIR numerator coefficients
 * \param[in]       d1...d3: IIR denominator coefficients
 *
 */
void IIRFilterInit(IIRFilterGeneric_t* filter, float n0, float n1, float n2, float n3, float d1, float d2, float d3);

/**
 * \brief           Apply generic IIR filter to provided sample
 *
 *
 * \param[in]       filter: pointer to IIR filter structure
 * \param[in]       input: input sample to be filtered
 *
 * \return		    filtered value
 *
 */
float IIRFilterProcess(IIRFilterGeneric_t* filter, float input);

/**
 * \brief           Reset generic IIR filter
 *
 *
 * \param[in]       filter: pointer to IIR filter structure
 *
 */
static inline void IIRFilterReset(IIRFilterGeneric_t* filter) {
    /* Initialize state variables */
    filter->i1 = filter->i2 = filter->i3 = filter->o1 = filter->o2 = filter->o3 = 0.0;
}

/**
 * \brief           Initialize derivative IIR filter
 *
 *
 * \param[in]       filter: pointer to IIR derivative filter structure
 * \param[in]       ndVal: derivative filter constant N - derivative in Laplace=s/(1+s/N)
 * \param[in]       dT_ms: loop time in ms
 *
 */
static inline void IIRFilterDerivativeInit(IIRFilterDerivative_t* filter, float ndVal, float dT_ms) {
    /* Store filter coefficients */
    filter->n0 = (2 * ndVal) / (2 + ndVal * dT_ms * 1e-3f);
    filter->d1 = (2 - ndVal * dT_ms * 1e-3f) / (2 + ndVal * dT_ms * 1e-3f);

    /* Initialize state variables */
    filter->i1 = filter->output = 0.0;
}

/**
 * \brief           Apply derivative IIR filter to provided sample
 *
 *
 * \param[in]       filter: pointer to IIR filter structure
 * \param[in]       input: input sample to be filtered
 *
 * \return		    filtered value
 *
 */
static inline float IIRFilterDerivativeProcess(IIRFilterDerivative_t* filter, float input) {
    filter->output = filter->n0 * (input - filter->i1) + filter->d1 * filter->output;
    filter->i1 = input;
    return filter->output;
}

/**
 * \brief           Reset derivative IIR filter
 *
 *
 * \param[in]       filter: pointer to IIR filter structure
 *
 */
static inline void IIRFilterDerivativeReset(IIRFilterDerivative_t* filter) {
    /* Initialize state variables */
    filter->i1 = filter->output = 0.0;
}

/**
 * \brief           Initialize integrator IIR filter
 *
 *
 * \param[in]       filter: pointer to IIR integrator filter structure
 * \param[in]       dT_ms: loop time in ms
 *
 */
static inline void IIRFilterIntegratorInit(IIRFilterIntegrator_t* filter, float dT_ms) {
    /* Store filter coefficients */
    filter->n0 = 0.5 * dT_ms * 1e-3f;

    /* Initialize state variables */
    filter->i1 = filter->output = 0.0;
}

/**
 * \brief           Apply integrator IIR filter to provided sample
 *
 *
 * \param[in]       filter: pointer to IIR filter structure
 * \param[in]       input: input sample to be filtered
 *
 * \return		    filtered value
 *
 */
static inline float IIRFilterIntegratorProcess(IIRFilterIntegrator_t* filter, float input) {
    filter->output += filter->n0 * (input + filter->i1);
    filter->i1 = input;
    return filter->output;
}

/**
 * \brief           Reset integrator IIR filter
 *
 *
 * \param[in]       filter: pointer to IIR filter structure
 *
 */
static inline void IIRFilterIntegratorReset(IIRFilterIntegrator_t* filter) {
    /* Initialize state variables */
    filter->i1 = filter->output = 0.0;
}

#ifdef __cplusplus
}
#endif

#endif /* __IIRFILTER_H__ */