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
#ifndef ADVUTILS_IIRFILTER_H
#define ADVUTILS_IIRFILTER_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/* Typedefs ------------------------------------------------------------------*/

/**
 * Generic IIR filter struct
 */
typedef struct {
    float d1;
    float d2;
    float d3; /* denominator coefficients */
    float n0;
    float n1;
    float n2;
    float n3; /* numerator coefficients */
    float i1;
    float i2;
    float i3; /* previous input values */
    float o1;
    float o2;
    float o3; /* previous output values */
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

/**
 * Leaky integrator filter struct
 */
typedef struct {
    float n0;     /* numerator coefficients */
    float leak;   /* leak coefficient */
    float i1;     /* previous input values */
    float output; /* output values */
} IIRFilterLeakyIntegrator_t;

/* Function prototypes -------------------------------------------------------*/

/**
 * \brief           Initialize generic IIR filter with custom coefficients
 *
 *
 * \param[in]       filter: pointer to IIR filter structure
 * \param[in]       n0...n3: IIR numerator coefficients
 * \param[in]       d1...d3: IIR denominator coefficients
 */
void IIRFilterInit(IIRFilterGeneric_t* filter, float n0, float n1, float n2, float n3, float d1, float d2, float d3);

/**
 * \brief           Initialize second-order Butterworth low-pass IIR filter
 *
 * \param[in]       filter: pointer to IIR filter structure
 * \param[in]       lpFreq: low-pass cutoff frequency in Hz
 * \param[in]       dT_ms: sampling time in ms
 */
void IIRFilterInitLP(IIRFilterGeneric_t* filter, float lpFreq, float dT_ms);

/**
 * \brief           Initialize second-order Butterworth high-pass IIR filter
 *
 * \param[in]       filter: pointer to IIR filter structure
 * \param[in]       hpFreq: high-pass cutoff frequency in Hz
 * \param[in]       dT_ms: sampling time in ms
 */
void IIRFilterInitHP(IIRFilterGeneric_t* filter, float hpFreq, float dT_ms);

/**
 * \brief           Initialize second-order Butterworth band-pass IIR filter
 *
 * \param[in]       filter: pointer to IIR filter structure
 * \param[in]       centerFreq: center frequency of pass-band in Hz
 * \param[in]       bandwidth: bandwidth of pass-band in Hz. Response will be symmetrical around centerFreq on a logarithmic scale
 * \param[in]       dT_ms: sampling time in ms
 */
void IIRFilterInitBP(IIRFilterGeneric_t* filter, float centerFreq, float bandwidth, float dT_ms);

/**
 * \brief           Initialize second-order Butterworth band-stop IIR filter
 *
 * \param[in]       filter: pointer to IIR filter structure
 * \param[in]       centerFreq: center frequency of stop-band in Hz
 * \param[in]       bandwidth: bandwidth of stop-band in Hz. Response will be symmetrical around centerFreq on a logarithmic scale
 * \param[in]       dT_ms: sampling time in ms
 */
void IIRFilterInitBS(IIRFilterGeneric_t* filter, float centerFreq, float bandwidth, float dT_ms);

/**
 * \brief           Apply generic IIR filter to provided sample
 *
 *
 * \param[in]       filter: pointer to IIR filter structure
 * \param[in]       input: input sample to be filtered
 *
 * \return		    filtered value
 */
float IIRFilterProcess(IIRFilterGeneric_t* filter, float input);

/**
 * \brief           Set generic IIR filter state variables to provided value
 *
 *
 * \param[in]       filter: pointer to IIR filter structure
 * \param[in]       value: value to set the filter state variables to
 */
static inline void IIRFilterSetValue(IIRFilterGeneric_t* filter, float value) {
    /* Initialize state variables */
    filter->i1 = value;
    filter->i2 = value;
    filter->i3 = value;
    filter->o1 = value;
    filter->o2 = value;
    filter->o3 = value;
}

/**
 * \brief           Reset generic IIR filter
 *
 *
 * \param[in]       filter: pointer to IIR filter structure
 */
#define IIRFilterReset(filter) IIRFilterSetValue(filter, 0.0f)

/**
 * \brief           Initialize derivative IIR filter
 *
 *
 * \param[in]       filter: pointer to IIR derivative filter structure
 * \param[in]       ndVal: derivative filter constant N - derivative in Laplace=s/(1+s/N)
 * \param[in]       dT_ms: loop time in ms
 */
static inline void IIRFilterDerivativeInit(IIRFilterDerivative_t* filter, float ndVal, float dT_ms) {
    /* Store filter coefficients */
    filter->n0 = (2.0f * ndVal) / (2.0f + (ndVal * dT_ms * 1e-3f));
    filter->d1 = (2.0f - (ndVal * dT_ms * 1e-3f)) / (2.0f + (ndVal * dT_ms * 1e-3f));

    /* Initialize state variables */
    filter->i1 = 0.0f;
    filter->output = 0.0f;
}

/**
 * \brief           Apply derivative IIR filter to provided sample
 *
 *
 * \param[in]       filter: pointer to IIR filter structure
 * \param[in]       input: input sample to be filtered
 *
 * \return		    filtered value
 */
static inline float IIRFilterDerivativeProcess(IIRFilterDerivative_t* filter, float input) {
    filter->output = (filter->n0 * (input - filter->i1)) + (filter->d1 * filter->output);
    filter->i1 = input;
    return filter->output;
}

/**
 * \brief           Set derivative IIR filter state variables to provided value
 *
 *
 * \param[in]       filter: pointer to IIR filter structure
 * \param[in]       value: value to set the filter state variables to
 */
static inline void IIRFilterDerivativeSetValue(IIRFilterDerivative_t* filter, float value) {
    /* Initialize state variables */
    filter->i1 = value;
    filter->output = value;
}

/**
 * \brief           Reset derivative IIR filter
 *
 *
 * \param[in]       filter: pointer to IIR filter structure
 */
#define IIRFilterDerivativeReset(filter) IIRFilterDerivativeSetValue(filter, 0.0f);

/**
 * \brief           Initialize IIR integrator
 *
 *
 * \param[in]       filter: pointer to IIR integrator structure
 * \param[in]       dT_ms: loop time in ms
 */
static inline void IIRFilterIntegratorInit(IIRFilterIntegrator_t* filter, float dT_ms) {
    /* Store filter coefficients */
    filter->n0 = 0.5f * dT_ms * 1e-3f;

    /* Initialize state variables */
    filter->i1 = 0.0f;
    filter->output = 0.0f;
}

/**
 * \brief           Apply IIR integrator to provided sample
 *
 *
 * \param[in]       filter: pointer to IIR integrator structure
 * \param[in]       input: input sample to be filtered
 *
 * \return		    filtered value
 */
static inline float IIRFilterIntegratorProcess(IIRFilterIntegrator_t* filter, float input) {
    filter->output += filter->n0 * (input + filter->i1);
    filter->i1 = input;
    return filter->output;
}

/**
 * \brief           Set IIR integrator state variables to provided value
 *
 *
 * \param[in]       filter: pointer to IIR integrator structure
 * \param[in]       value: value to set the filter state variables to
 */
static inline void IIRFilterIntegratorSetValue(IIRFilterIntegrator_t* filter, float value) {
    /* Initialize state variables */
    filter->i1 = value;
    filter->output = value;
}

/**
 * \brief           Reset IIR integrator
 *
 *
 * \param[in]       filter: pointer to IIR integrator structure
 */
#define IIRFilterIntegratorReset(filter) IIRFilterIntegratorSetValue(filter, 0.0f)

/**
 * \brief           Initialize IIR leaky integrator
 *
 *
 * \param[in]       filter: pointer to IIR leaky integrator structure
 * \param[in]       dT_ms: loop time in ms
 * \param[in]       tau_ms: leak time constant in ms
 */
static inline void IIRFilterLeakyIntegratorInit(IIRFilterLeakyIntegrator_t* filter, float dT_ms, float tau_ms) {
    const float dT = dT_ms * 1e-3f;
    const float tau = tau_ms * 1e-3f;

    /* Bilinear (Tustin) discretization of 1 / (s + 1/tau) */
    filter->n0 = (dT * tau) / ((2.0f * tau) + dT);
    filter->leak = ((2.0f * tau) - dT) / ((2.0f * tau) + dT);

    filter->i1 = 0.0f;
    filter->output = 0.0f;
}

/**
 * \brief           Apply IIR leaky integrator to provided sample
 *
 *
 * \param[in]       filter: pointer to IIR leaky integrator structure
 * \param[in]       input: input sample to be filtered
 *
 * \return		    filtered value
 */
static inline float IIRFilterLeakyIntegratorProcess(IIRFilterLeakyIntegrator_t* filter, float input) {
    filter->output = (filter->leak * filter->output) + (filter->n0 * (input + filter->i1));
    filter->i1 = input;
    return filter->output;
}

/**
 * \brief           Set IIR leaky integrator state variables to provided value
 *
 *
 * \param[in]       filter: pointer to IIR leaky integrator structure
 * \param[in]       value: value to set the filter state variables to
 */
static inline void IIRFilterLeakyIntegratorSetValue(IIRFilterLeakyIntegrator_t* filter, float value) {
    /* Initialize state variables */
    filter->i1 = value;
    filter->output = value;
}

/**
 * \brief           Reset IIR leaky integrator
 *
 *
 * \param[in]       filter: pointer to IIR leaky integrator structure
 */
#define IIRFilterLeakyIntegratorReset(filter) IIRFilterLeakyIntegratorSetValue(filter, 0.0f)

#ifdef __cplusplus
}
#endif

#endif /* ADVUTILS_IIRFILTER_H */
