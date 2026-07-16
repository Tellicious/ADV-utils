/* BEGIN Header */
/**
 ******************************************************************************
 * \file            queue.c
 * \author          Andrea Vivani
 * \brief           Implementation of queue structure
 ******************************************************************************
 * \copyright
 *
 * Copyright 2023 Andrea Vivani
 *
 * Permission is hereby granted, ADVUTILS_FREE of charge, to any person obtaining a copy
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

#include "queue.h"
#include <string.h>
#include "ADVUtilsAssert.h"
#ifdef ADVUTILS_MEMORY_MGMT_HEADER
#if !defined(ADVUTILS_MALLOC) || !defined(ADVUTILS_CALLOC) || !defined(ADVUTILS_FREE)
#error ADVUTILS_MEMORY_MGMT_HEADER, ADVUTILS_MALLOC, ADVUTILS_CALLOC and ADVUTILS_FREE must be defined by the user!
#else
#include ADVUTILS_MEMORY_MGMT_HEADER
#endif
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

utilsStatus_t queueInit(queue_t* queue, size_t itemSize, QUEUE_STYPE size) {
    queue->data = NULL;
    queue->data = ADVUTILS_CALLOC(size, itemSize);
    /* queue->data = ADVUTILS_MALLOC(size * itemSize); */
    ADVUTILS_ASSERT(queue->data != NULL);
    if (queue->data == NULL) {
        queue->size = 0;
        return UTILS_STATUS_ERROR;
    }

    queue->size = size * itemSize;
    queue->itemSize = itemSize;
    queue->items = 0;
    queue->_front = 0;
    queue->_rear = 0;
    return UTILS_STATUS_SUCCESS;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef ADVUTILS_USE_STATIC_ALLOCATION

void queueInitStatic(queue_t* queue, uint8_t* data, size_t itemSize, QUEUE_STYPE size) {
    queue->data = data;
    queue->size = size * itemSize;
    queue->itemSize = itemSize;
    queue->items = 0;
    queue->_front = 0;
    queue->_rear = 0;
    return;
}

#endif /* ADVUTILS_USE_STATIC_ALLOCATION */

utilsStatus_t queuePush(queue_t* queue, const void* value) {
    if (queue->items == queue->size) {
        return UTILS_STATUS_FULL;
    }

    if (queue->_rear >= queue->size) {
        queue->_rear = 0;
    }

    memcpy(&(queue->data[queue->_rear]), value, queue->itemSize);
    queue->_rear += queue->itemSize;
    queue->items += queue->itemSize;

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t queuePushArr(queue_t* queue, void* data, QUEUE_STYPE num) {
    QUEUE_STYPE num2End = 0;
    QUEUE_STYPE numBytes = num * queue->itemSize;

    if ((queue->size - queue->items) < numBytes) {
        return UTILS_STATUS_ERROR;
    }

    if (queue->_rear >= queue->size) {
        queue->_rear = 0;
    }

    num2End = queue->size - queue->_rear;

    if (num2End < numBytes) {
        memcpy(&(queue->data[queue->_rear]), data, num2End);
        memcpy(queue->data, ((uint8_t*)data + num2End), (numBytes - num2End));
        queue->_rear = numBytes - num2End;
    } else {
        memcpy(&(queue->data[queue->_rear]), data, numBytes);
        queue->_rear += numBytes;
    }
    queue->items += numBytes;

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t queuePushFront(queue_t* queue, const void* value) {
    if (queue->items == queue->size) {
        return UTILS_STATUS_FULL;
    }

    if (queue->_front == 0) {
        queue->_front = queue->size;
    }

    queue->_front -= queue->itemSize;

    memcpy(&(queue->data[queue->_front]), value, queue->itemSize);
    queue->items += queue->itemSize;

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t queuePushFrontArr(queue_t* queue, void* data, QUEUE_STYPE num) {
    QUEUE_STYPE numBytes = num * queue->itemSize;

    if ((queue->size - queue->items) < numBytes) {
        return UTILS_STATUS_ERROR;
    }

    if (queue->_front == 0) {
        queue->_front = queue->size;
    }

    if (queue->_front < numBytes) {
        QUEUE_STYPE numFromEnd = numBytes - queue->_front;
        memcpy(queue->data, ((uint8_t*)data + numFromEnd), queue->_front);
        memcpy(&(queue->data[queue->size - numFromEnd]), data, numFromEnd);
        queue->_front = queue->size - numFromEnd;
    } else {
        memcpy(&(queue->data[queue->_front - numBytes]), data, numBytes);
        queue->_front -= numBytes;
    }
    queue->items += numBytes;

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t queuePop(queue_t* queue, void* value) {
    if (queue->items == 0) {
        return UTILS_STATUS_EMPTY;
    }

    if (queue->_front >= queue->size) {
        queue->_front = 0;
    }

    memcpy(value, &(queue->data[queue->_front]), queue->itemSize);
    queue->items -= queue->itemSize;

    queue->_front += queue->itemSize;

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t queuePopArr(queue_t* queue, void* data, QUEUE_STYPE num) {
    QUEUE_STYPE num2End = 0;
    QUEUE_STYPE numBytes = num * queue->itemSize;

    if (queue->items < numBytes) {
        return UTILS_STATUS_ERROR;
    }

    if (queue->_front >= queue->size) {
        queue->_front = 0;
    }

    num2End = queue->size - queue->_front;

    if (num2End < numBytes) {
        memcpy(data, &(queue->data[queue->_front]), num2End);
        memcpy(((uint8_t*)data + num2End), queue->data, (numBytes - num2End));
        queue->_front = numBytes - num2End;
    } else {
        memcpy(data, &(queue->data[queue->_front]), numBytes);
        queue->_front += numBytes;
    }
    queue->items -= numBytes;

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t queuePopBack(queue_t* queue, void* value) {
    if (queue->items == 0) {
        return UTILS_STATUS_EMPTY;
    }

    if (queue->_rear == 0) {
        queue->_rear = queue->size;
    }

    queue->_rear -= queue->itemSize;

    memcpy(value, &(queue->data[queue->_rear]), queue->itemSize);
    queue->items -= queue->itemSize;

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t queuePopBackArr(queue_t* queue, void* data, QUEUE_STYPE num) {
    QUEUE_STYPE numBytes = num * queue->itemSize;

    if (queue->items < numBytes) {
        return UTILS_STATUS_ERROR;
    }

    if (queue->_rear == 0) {
        queue->_rear = queue->size;
    }

    if (queue->_rear < numBytes) {
        QUEUE_STYPE numFromEnd = numBytes - queue->_rear;
        memcpy(((uint8_t*)data + numFromEnd), queue->data, queue->_rear);
        memcpy(data, &(queue->data[queue->size - numFromEnd]), numFromEnd);
        queue->_rear = queue->size - numFromEnd;
    } else {
        memcpy(data, &(queue->data[queue->_rear - numBytes]), numBytes);
        queue->_rear -= numBytes;
    }
    queue->items -= numBytes;

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t queuePeek(queue_t* queue, void* value) {
    if (queue->items == 0) {
        return UTILS_STATUS_EMPTY;
    }

    if (queue->_front >= queue->size) {
        queue->_front = 0;
    }

    memcpy(value, &(queue->data[queue->_front]), queue->itemSize);

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t queuePeekBack(queue_t* queue, void* value) {
    if (queue->items == 0) {
        return UTILS_STATUS_EMPTY;
    }

    if (queue->_rear == 0) {
        queue->_rear = queue->size;
    }

    memcpy(value, &(queue->data[queue->_rear - queue->itemSize]), queue->itemSize);

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t queueFlush(queue_t* queue) {
    if (queue->data == NULL) {
        return UTILS_STATUS_ERROR;
    }
    memset(queue->data, 0x00, queue->size);
    queue->items = 0;
    queue->_front = 0;
    queue->_rear = 0;

    return UTILS_STATUS_SUCCESS;
}

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION

utilsStatus_t queueDelete(queue_t* queue) {

    if (queue->data == NULL) {
        return UTILS_STATUS_ERROR;
    }

    ADVUTILS_FREE(queue->data);
    queue->data = NULL;

    return UTILS_STATUS_SUCCESS;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */
