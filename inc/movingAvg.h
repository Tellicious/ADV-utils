/* BEGIN Header */
/**
 ******************************************************************************
 * \file            movingAvg.h
 * \author          Andrea Vivani
 * \brief           Implementation of moving average
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
#ifndef __MOVINGAVG_H__
#define __MOVINGAVG_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "commonTypes.h"

/* Macros --------------------------------------------------------------------*/

/* Moving average data type */
#ifndef MOVAVG_TYPE
#define MOVAVG_TYPE float
#endif /* MOVAVG_TYPE */

/* Type of moving average size property */
#ifndef MOVAVG_STYPE
#define MOVAVG_STYPE uint16_t
#endif /* MOVAVG_STYPE */

/* Typedefs ------------------------------------------------------------------*/

/**
 * Moving average struct
 */
typedef struct {
    MOVAVG_TYPE* data;
    MOVAVG_TYPE sum, inv_size;
    MOVAVG_STYPE size, _write;
} movingAvg_t;

/* Function prototypes -------------------------------------------------------*/

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Init moving average structure with dynamic memory allocation
 *
 * \param[in]       movingAvg: pointer to moving average object
 * \param[in]       size: required queue size
 */
utilsStatus_t movingAvgInit(movingAvg_t* movingAvg, MOVAVG_STYPE size);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION
/**
 * \brief           Init moving average structure with static data
 *
 * \param[in]       movingAvg: pointer to moving average object
 * \param[in]       data: pointer to data array
 * \param[in]       size: required queue size
 */
void movingAvgInitStatic(movingAvg_t* movingAvg, MOVAVG_TYPE* data, MOVAVG_STYPE size);
#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

/**
 * \brief           Calculate moving average 
 *
 * \param[in]       movingAvg: pointer to moving average object
 * \param[in]       value: input value
 * 
 * \return          updated moving average
 */
MOVAVG_TYPE movingAvgCalc(movingAvg_t* movingAvg, MOVAVG_TYPE value);

/**
 * \brief           Get latest moving average 
 *
 * \param[in]       movingAvg: pointer to moving average object
 * 
 * \return          latest moving average
 */
static inline MOVAVG_TYPE movingAvgGetLatest(movingAvg_t* movingAvg) { return (movingAvg->sum * movingAvg->inv_size); }

/**
 * \brief           Flush moving average setting all values to 0
 *
 * \param[in]       movingAvg: pointer to moving average object
 */
utilsStatus_t movingAvgFlush(movingAvg_t* movingAvg);

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Delete moving average
 *
 * \param[in]       movingAvg: pointer to moving average object
 */
utilsStatus_t movingAvgDelete(movingAvg_t* movingAvg);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef __cplusplus
}
#endif

#endif /* __QUEUE_H__ */
