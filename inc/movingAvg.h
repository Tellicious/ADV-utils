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
#ifndef ADVUTILS_MOVINGAVG_H
#define ADVUTILS_MOVINGAVG_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "commonTypes.h"

/**
 * \defgroup movingAvg Moving average
 * \brief Moving average
 * @{
 */

/* Macros --------------------------------------------------------------------*/

/* Moving average data type */
#ifndef MOVAVG_TYPE
/**
 * \def MOVAVG_TYPE
 * \brief Data type of the samples stored and averaged (default float)
 */
#define MOVAVG_TYPE float
#endif /* MOVAVG_TYPE */

/* Type of moving average size property */
#ifndef MOVAVG_STYPE
/**
 * \def MOVAVG_STYPE
 * \brief Unsigned integer type for the window size/index (default uint16_t)
 */
#define MOVAVG_STYPE uint16_t
#endif /* MOVAVG_STYPE */

/* Typedefs ------------------------------------------------------------------*/

/**
 * \brief           Moving-average filter instance
 */
typedef struct {
    MOVAVG_TYPE* data;    /**< Sample window storage */
    MOVAVG_TYPE sum;      /**< Running sum of the window */
    MOVAVG_TYPE inv_size; /**< Precomputed 1/size */
    MOVAVG_STYPE size;    /**< Window length */
    MOVAVG_STYPE _write;  /**< Next write index (internal) */
} movingAvg_t;

/* Function prototypes -------------------------------------------------------*/

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Init moving average structure with dynamic memory allocation
 *
 * \param[in]       movingAvg: pointer to moving average object
 * \param[in]       size: required queue size
 * 
 * \retval         UTILS_STATUS_SUCCESS moving average was initialized
 * \retval         UTILS_STATUS_ERROR data was not allocated correctly
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
static inline MOVAVG_TYPE movingAvgGetLatest(const movingAvg_t* movingAvg) { return (movingAvg->sum * movingAvg->inv_size); }

/**
 * \brief           Flush moving average setting all values to 0
 *
 * \param[in]       movingAvg: pointer to moving average object
 * \retval          UTILS_STATUS_SUCCESS moving average was flushed
 * \retval          UTILS_STATUS_ERROR moving average was not initialized
 */
utilsStatus_t movingAvgFlush(movingAvg_t* movingAvg);

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Delete moving average
 *
 * \param[in]       movingAvg: pointer to moving average object
 * \retval          UTILS_STATUS_SUCCESS moving average was deleted
 * \retval          UTILS_STATUS_ERROR moving average was not initialized
 */
utilsStatus_t movingAvgDelete(movingAvg_t* movingAvg);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ADVUTILS_MOVINGAVG_H */
