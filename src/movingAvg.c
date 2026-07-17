/* BEGIN Header */
/**
 ******************************************************************************
 * \file            movingAvg.c
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

/* Configuration check -------------------------------------------------------*/
#if !defined(ADVUTILS_USE_DYNAMIC_ALLOCATION) && !defined(ADVUTILS_USE_STATIC_ALLOCATION)
#error Either ADVUTILS_USE_DYNAMIC_ALLOCATION or ADVUTILS_USE_STATIC_ALLOCATION must be set for ADVUtils to work
#endif

/* Includes ------------------------------------------------------------------*/

#include "movingAvg.h"
#include <string.h>
#include "ADVUtilsAssert.h"
#ifdef ADVUTILS_MEMORY_MGMT_HEADER
#if !defined(ADVUTILS_MALLOC) || !defined(ADVUTILS_CALLOC) || !defined(ADVUTILS_FREE)
#error ADVUTILS_MALLOC, ADVUTILS_CALLOC and ADVUTILS_FREE must be defined by the user!
#else
#include ADVUTILS_MEMORY_MGMT_HEADER
#endif /* !defined(ADVUTILS_MALLOC) || !defined(ADVUTILS_CALLOC) || !defined(ADVUTILS_FREE) */
#else
#include <stdlib.h>
#endif /* ADVUTILS_MEMORY_MGMT_HEADER */

/* Macros --------------------------------------------------------------------*/

#ifndef ADVUTILS_MEMORY_MGMT_HEADER
#define ADVUTILS_MALLOC malloc
#define ADVUTILS_CALLOC calloc
#define ADVUTILS_FREE   free
#endif /* ADVUTILS_MEMORY_MGMT_HEADER */

/* Functions -----------------------------------------------------------------*/

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION

utilsStatus_t movingAvgInit(movingAvg_t* movingAvg, MOVAVG_STYPE size) {
    movingAvg->data = NULL;
    /* cppcheck-suppress misra-c2012-11.5 ; deviation: generic container returns typed pointer from void* storage */
    movingAvg->data = ADVUTILS_CALLOC(size, sizeof(MOVAVG_TYPE));
    ADVUTILS_ASSERT(movingAvg->data != NULL);
    if (movingAvg->data == NULL) {
        movingAvg->size = 0;
        return UTILS_STATUS_ERROR;
    }

    movingAvg->size = size;
    movingAvg->sum = 0;
    movingAvg->inv_size = (MOVAVG_TYPE)1.0 / size;
    movingAvg->_write = 0;
    return UTILS_STATUS_SUCCESS;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION

void movingAvgInitStatic(movingAvg_t* movingAvg, MOVAVG_TYPE* data, MOVAVG_STYPE size) {
    movingAvg->data = data;
    movingAvg->size = size;
    movingAvg->sum = 0;
    movingAvg->inv_size = (MOVAVG_TYPE)1.0 / size;
    movingAvg->_write = 0;
    return;
}

#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

MOVAVG_TYPE movingAvgCalc(movingAvg_t* movingAvg, MOVAVG_TYPE value) {
    movingAvg->sum -= movingAvg->data[movingAvg->_write];
    movingAvg->sum += value;
    movingAvg->data[movingAvg->_write] = value;
    movingAvg->_write++;
    movingAvg->_write %= movingAvg->size;
    return (movingAvg->sum * movingAvg->inv_size);
}

utilsStatus_t movingAvgFlush(movingAvg_t* movingAvg) {
    if (movingAvg->data == NULL) {
        return UTILS_STATUS_ERROR;
    }

    (void)memset(movingAvg->data, 0x00, movingAvg->size * sizeof(MOVAVG_TYPE));

    movingAvg->sum = 0;
    movingAvg->_write = 0;

    return UTILS_STATUS_SUCCESS;
}

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION

utilsStatus_t movingAvgDelete(movingAvg_t* movingAvg) {

    if (movingAvg->data == NULL) {
        return UTILS_STATUS_ERROR;
    }

    ADVUTILS_FREE(movingAvg->data);
    movingAvg->data = NULL;

    return UTILS_STATUS_SUCCESS;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */
