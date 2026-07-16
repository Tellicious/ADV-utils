/* BEGIN Header */
/**
 ******************************************************************************
 * \file            queue.h
 * \author          Andrea Vivani
 * \brief           Implementation of queue structure
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
#ifndef __QUEUE_H__
#define __QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stddef.h>
#include <stdint.h>
#include "commonTypes.h"

/* Macros --------------------------------------------------------------------*/

/* Type of queue size property */
#ifndef QUEUE_STYPE
#define QUEUE_STYPE uint16_t
#endif /* QUEUE_STYPE */

/* Typedefs ------------------------------------------------------------------*/

/**
 * Queue struct
 */
typedef struct {
    uint8_t* data;
    uint8_t itemSize;
    QUEUE_STYPE size, _front, _rear, items;
} queue_t;

/* Function prototypes -------------------------------------------------------*/

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Init queue structure with dynamic memory allocation
 *
 * \param[in]       queue: pointer to queue object
 * \param[in]       itemSize: size in bytes of each item in the queue
 * \param[in]       size: required queue size (number of objects)
 *
 * \return          UTILS_STATUS_SUCCESS if queue is initialized, UTILS_STATUS_ERROR if data was not allocated correctly
 */
utilsStatus_t queueInit(queue_t* queue, size_t itemSize, QUEUE_STYPE size);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION
/**
 * \brief           Init queue structure with static data
 *
 * \param[in]       queue: pointer to queue object
 * \param[in]       data: pointer to data array with size = size * itemSize
 * \param[in]       itemSize: size in bytes of each item in the queue
 * \param[in]       size: required queue size (number of objects)
 *
 * \return          UTILS_STATUS_SUCCESS if queue is initialized, UTILS_STATUS_ERROR if data was not allocated correctly
 */
void queueInitStatic(queue_t* queue, uint8_t* data, size_t itemSize, QUEUE_STYPE size);
#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

/**
 * \brief           Add data to end of queue
 *
 * \param[in]       queue: pointer to queue object
 * \param[in]       value: pointer to value to be pushed
 *
 * \return          UTILS_STATUS_SUCCESS if data can be pushed correctly, UTILS_STATUS_FULL if queue is full
 */
utilsStatus_t queuePush(queue_t* queue, const void* value);

/**
 * \brief           Add an array of data to end of queue
 *
 * \param[in]       queue: pointer to queue object
 * \param[in]       data: pointer to array of data to be pushed
 * \param[in]       num: number of values to be pushed
 *
 * \return          UTILS_STATUS_SUCCESS if data can be pushed correctly, UTILS_STATUS_ERROR if queue cannot hold all elements
 */
utilsStatus_t queuePushArr(queue_t* queue, void* data, QUEUE_STYPE num);

/**
 * \brief           Add data to beginning of queue
 *
 * \param[in]       queue: pointer to queue object
 * \param[in]       value: pointer to value to be pushed to front
 *
 * \return          UTILS_STATUS_SUCCESS if data can be pushed to front correctly, UTILS_STATUS_FULL if queue is full
 */
utilsStatus_t queuePushFront(queue_t* queue, const void* value);

/**
 * \brief           Add an array of data to beginning of queue
 *
 * \param[in]       queue: pointer to queue object
 * \param[in]       data: pointer to array of data to be pushed to front
 * \param[in]       num: number of values to be pushed to front
 *
 * \return          UTILS_STATUS_SUCCESS if data can be pushed to front correctly, UTILS_STATUS_ERROR if queue cannot hold all elements
 */
utilsStatus_t queuePushFrontArr(queue_t* queue, void* data, QUEUE_STYPE num);

/**
 * \brief           Read data from beginning of queue, removing it
 *
 * \param[in]       queue: pointer to queue object
 * \param[out]      value: pointer to value to be read and removed
 *
 * \return          UTILS_STATUS_SUCCESS if data can be read and removed correctly, UTILS_STATUS_EMPTY if queue is empty
 */
utilsStatus_t queuePop(queue_t* queue, void* value);

/**
 * \brief           Read an array of data from beginning of queue, removing it
 *
 * \param[in]       queue: pointer to queue object
 * \param[out]      data: pointer to array of data to be read and removed
 * \param[in]       num: number of values to be read and removed
 *
 * \return          UTILS_STATUS_SUCCESS if data can be read and removed correctly, UTILS_STATUS_ERROR if queue doesn't hold all requested items
 */
utilsStatus_t queuePopArr(queue_t* queue, void* data, QUEUE_STYPE num);

/**
 * \brief           Read data from end of queue, removing it
 *
 * \param[in]       queue: pointer to queue object
 * \param[out]      value: pointer to value to be read and removed
 *
 * \return          UTILS_STATUS_SUCCESS if data can be read and removed correctly, UTILS_STATUS_EMPTY if queue is empty
 */
utilsStatus_t queuePopBack(queue_t* queue, void* value);

/**
 * \brief           Read an array of data from end of queue, removing it
 *
 * \param[in]       queue: pointer to queue object
 * \param[out]      data: pointer to array of data to be read and removed
 * \param[in]       num: number of values to be read and removed
 *
 * \return          UTILS_STATUS_SUCCESS if data can be read and removed correctly, UTILS_STATUS_ERROR if queue doesn't hold all requested items
 */
utilsStatus_t queuePopBackArr(queue_t* queue, void* data, QUEUE_STYPE num);

/**
 * \brief           Read data from beginning of queue, without removing it
 *
 * \param[in]       queue: pointer to queue object
 * \param[out]      value: pointer to value to be read
 *
 * \return          UTILS_STATUS_SUCCESS if data can be read correctly, UTILS_STATUS_EMPTY if queue is empty
 */
utilsStatus_t queuePeek(queue_t* queue, void* value);

/**
 * \brief           Read data from end of queue, without removing it
 *
 * \param[in]       queue: pointer to queue object
 * \param[out]      value: pointer to value to be read
 *
 * \return          UTILS_STATUS_SUCCESS if data can be read correctly, UTILS_STATUS_EMPTY if queue is empty
 */
utilsStatus_t queuePeekBack(queue_t* queue, void* value);

/**
 * \brief           Returns queue info
 *
 * \param[in]       queue: pointer to queue object
 * \param[out]      size: pointer to size
 * \param[out]      items: pointer to number of items currently queued
 */
static inline void queueInfo(queue_t* queue, QUEUE_STYPE* size, QUEUE_STYPE* items) {
    *size = queue->size / queue->itemSize;
    *items = queue->items / queue->itemSize;
}

/**
 * \brief           Flush queue setting all values to 0
 *
 * \param[in]       queue: pointer to queue object
 *
 * \return          UTILS_STATUS_SUCCESS if queue is flushed correctly, UTILS_STATUS_ERROR if data cannot be flushed
 */
utilsStatus_t queueFlush(queue_t* queue);

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Delete queue
 *
 * \param[in]       queue: pointer to queue object
 *
 * \return          UTILS_STATUS_SUCCESS if queue data is deleted, UTILS_STATUS_ERROR if data was already deleted
 */
utilsStatus_t queueDelete(queue_t* queue);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef __cplusplus
}
#endif

#endif /* __QUEUE_H__ */
