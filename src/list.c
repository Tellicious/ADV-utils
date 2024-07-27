/* BEGIN Header */
/**
 ******************************************************************************
 * \file            list.c
 * \author          Andrea Vivani
 * \brief           Implementation of linked list with dynamic memory allocation
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

#include "list.h"
#include <string.h>
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

#ifndef ADVUTILS_ASSERT
#ifdef DEBUG
#define ADVUTILS_ASSERT(x)                                                                                             \
    if ((x) == 0) {                                                                                                    \
        for (;;)                                                                                                       \
            ;                                                                                                          \
    }
#else
#define ADVUTILS_ASSERT(x)
#endif /* DEBUG */
#endif /* ADVUTILS_ASSERT */

/* Functions -----------------------------------------------------------------*/

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION

void listInit(list_t* list, size_t itemSize, LIST_STYPE size) {
    list->_front = NULL;
    list->_rear = NULL;
    list->itemSize = itemSize;
    list->size = size;
    list->items = 0;
}

utilsStatus_t listPush(list_t* list, void* value) {
    if (list->items >= list->size) {
        return UTILS_STATUS_FULL;
    }

    listNode_t* ptr;
    ptr = ADVUTILS_MALLOC(sizeof(listNode_t));
    ADVUTILS_ASSERT(ptr != NULL);
    ptr->data = ADVUTILS_CALLOC(1, list->itemSize);
    ADVUTILS_ASSERT(ptr->data != NULL);
    memcpy(ptr->data, value, list->itemSize);
    ptr->next = NULL;

    if ((list->_front == NULL) && (list->_rear == NULL)) {
        list->_front = list->_rear = ptr;
    } else {
        list->_rear->next = ptr;
        list->_rear = ptr;
    }
    list->items++;
    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t listPushFront(list_t* list, void* value) {
    if (list->items >= list->size) {
        return UTILS_STATUS_FULL;
    }

    listNode_t* ptr;
    ptr = ADVUTILS_MALLOC(sizeof(listNode_t));
    ADVUTILS_ASSERT(ptr != NULL);
    ptr->data = ADVUTILS_CALLOC(1, list->itemSize);
    ADVUTILS_ASSERT(ptr->data != NULL);
    memcpy(ptr->data, value, list->itemSize);
    ptr->next = NULL;

    if ((list->_front == NULL) && (list->_rear == NULL)) {
        list->_front = list->_rear = ptr;
    } else {
        ptr->next = list->_front;
        list->_front = ptr;
    }

    list->items++;
    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t listInsert(list_t* list, void* value, LIST_STYPE position) {
    LIST_STYPE ii;

    if (list->items >= list->size) {
        return UTILS_STATUS_FULL;
    }

    if (position > list->items) {
        return UTILS_STATUS_ERROR;
    }

    listNode_t* ptr;
    ptr = ADVUTILS_MALLOC(sizeof(listNode_t));
    ADVUTILS_ASSERT(ptr != NULL);
    ptr->data = ADVUTILS_CALLOC(1, list->itemSize);
    ADVUTILS_ASSERT(ptr->data != NULL);
    memcpy(ptr->data, value, list->itemSize);
    ptr->next = NULL;

    if ((list->_front == NULL) && (list->_rear == NULL)) {
        list->_front = list->_rear = ptr;
    } else if (position == list->items) {
        list->_rear->next = ptr;
        list->_rear = ptr;
    } else if (position == 0) {
        ptr->next = list->_front;
        list->_front = ptr;
    } else {
        /* search for node at position - 1 */
        listNode_t* prev = list->_front;
        for (ii = 1; ii < position; ii++) {
            prev = prev->next;
        }
        ptr->next = prev->next;
        prev->next = ptr;
    }

    list->items++;
    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t listUpdate(list_t* list, void* value, LIST_STYPE position) {
    LIST_STYPE ii;

    if ((position + 1) > list->items) {
        return UTILS_STATUS_ERROR;
    }

    listNode_t* ptr = list->_front;

    /* search for node at position */
    for (ii = 0; ii < position; ii++) {
        ptr = ptr->next;
    }

    memcpy(ptr->data, value, list->itemSize);

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t listPop(list_t* list, void* value) {
    if (!list->items) {
        return UTILS_STATUS_EMPTY;
    }

    listNode_t* ptr = list->_front;
    memcpy(value, ptr->data, list->itemSize);
    list->_front = ptr->next;
    ADVUTILS_FREE(ptr->data);
    ADVUTILS_FREE(ptr);

    list->items--;
    if (!list->items) {
        list->_rear = NULL;
    }
    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t listPopBack(list_t* list, void* value) {
    LIST_STYPE ii;

    if (!list->items) {
        return UTILS_STATUS_EMPTY;
    }

    memcpy(value, list->_rear->data, list->itemSize);

    /* search for node at end - 1 */
    listNode_t* prev = list->_front;
    for (ii = 2; ii < list->items; ii++) {
        prev = prev->next;
    }
    prev->next = NULL;

    ADVUTILS_FREE(list->_rear->data);
    ADVUTILS_FREE(list->_rear);
    list->_rear = prev;

    list->items--;
    if (!list->items) {
        list->_front = list->_rear = NULL;
    }
    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t listRemove(list_t* list, void* value, LIST_STYPE position) {
    LIST_STYPE ii;

    if (!list->items) {
        return UTILS_STATUS_EMPTY;
    }

    if ((position + 1) > list->items) {
        return UTILS_STATUS_ERROR;
    }

    listNode_t* prev = NULL;
    listNode_t* ptr = list->_front;

    /* search for node at position - 1 and node at position */
    for (ii = 0; ii < position; ii++) {
        prev = ptr;
        ptr = ptr->next;
    }
    memcpy(value, ptr->data, list->itemSize);

    if (position == 0) {
        list->_front = ptr->next;
    } else if ((position + 1) == list->items) {
        prev->next = NULL;
        list->_rear = prev;
    } else {
        prev->next = ptr->next;
    }

    ADVUTILS_FREE(ptr->data);
    ADVUTILS_FREE(ptr);

    list->items--;
    if (!list->items) {
        list->_front = list->_rear = NULL;
    }
    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t listPeek(list_t* list, void* value) {
    if (list->items == 0) {
        return UTILS_STATUS_EMPTY;
    }

    memcpy(value, list->_front->data, list->itemSize);

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t listPeekBack(list_t* list, void* value) {
    if (list->items == 0) {
        return UTILS_STATUS_EMPTY;
    }

    memcpy(value, list->_rear->data, list->itemSize);

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t listPeekAtPos(list_t* list, void* value, LIST_STYPE position) {
    LIST_STYPE ii;

    if (list->items == 0) {
        return UTILS_STATUS_EMPTY;
    }

    if ((position + 1) > list->items) {
        return UTILS_STATUS_ERROR;
    }

    listNode_t* ptr = list->_front;

    /* search for node at position */
    for (ii = 0; ii < position; ii++) {
        ptr = ptr->next;
    }

    memcpy(value, ptr->data, list->itemSize);

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t listFlush(list_t* list) {
    if (!list->items) {
        return UTILS_STATUS_EMPTY;
    }

    listNode_t* ptr = list->_front;
    while (ptr != NULL) {
        list->_front = ptr->next;
        ADVUTILS_FREE(ptr->data);
        ADVUTILS_FREE(ptr);
        ptr = list->_front;
    }
    list->items = 0;
    list->_rear = NULL;

    return UTILS_STATUS_SUCCESS;
}

void listIt(listIterator_t* it, list_t* list) {
    it->_list = list;
    it->idx = 0;
    it->_prev = NULL;
    it->ptr = NULL;
}

utilsStatus_t listItNext(listIterator_t* it) {
    if (it->ptr == NULL) {
        it->ptr = it->_list->_front;
        it->idx = 0;
        return UTILS_STATUS_SUCCESS;
    } else if (it->ptr->next != NULL) {
        it->_prev = it->ptr;
        it->ptr = it->ptr->next;
        it->idx++;
        return UTILS_STATUS_SUCCESS;
    }
    return UTILS_STATUS_ERROR;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */
