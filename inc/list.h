/* BEGIN Header */
/**
 ******************************************************************************
 * \file            list.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LIST_H__
#define __LIST_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stddef.h>
#include <stdint.h>
#include "commonTypes.h"

/* Macros --------------------------------------------------------------------*/

/* Type of list size property */
#ifndef LIST_STYPE
#define LIST_STYPE uint16_t
#endif /* LIST_STYPE */

/* Typedefs ------------------------------------------------------------------*/

/**
 * Node struct
 */
typedef struct node_str {
    void* data;
    struct node_str* next;
} listNode_t;

/**
 * List struct
 */
typedef struct {
    uint8_t itemSize;
    listNode_t *_front, *_rear;
    LIST_STYPE size, items;
} list_t;

/**
 * List iterator 
 */
typedef struct {
    list_t* _list;
    listNode_t *_prev, *ptr;
    LIST_STYPE idx;
} listIterator_t;

/* Function prototypes -------------------------------------------------------*/

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Init list structure
 *
 * \param[in]       list: pointer to list object
 * \param[in]       itemSize: size in bytes of each item in the list
 * \param[in]       size: maximum list size (number of objects)
 */
void listInit(list_t* list, size_t itemSize, LIST_STYPE size);

/**
 * \brief           Add data to end of list
 *
 * \param[in]       list: pointer to list object
 * \param[in]       value: pointer to value to be pushed
 *
 * \return          UTILS_STATUS_SUCCESS if data can be pushed correctly, UTILS_STATUS_FULL if list is full, UTILS_STATUS_ERROR if pointer is NULL
 */
utilsStatus_t listPush(list_t* list, const void* value);

/**
 * \brief           Add data to beginning of list
 *
 * \param[in]       list: pointer to list object
 * \param[in]       value: pointer to value to be pushed to front
 *
 * \return          UTILS_STATUS_SUCCESS if data can be pushed to front correctly, UTILS_STATUS_FULL if list is full, UTILS_STATUS_ERROR if pointer is NULL
 */
utilsStatus_t listPushFront(list_t* list, const void* value);

/**
 * \brief           Add data to a specific position within list
 *
 * \param[in]       list: pointer to list object
 * \param[in]       value: pointer to value to be pushed to front
 * \param[in]       position: position where to add data
 *
 * \return          UTILS_STATUS_SUCCESS if data can be added correctly, UTILS_STATUS_FULL if list is full, UTILS_STATUS_ERROR if position is invalid or pointer is NULL
 */
utilsStatus_t listInsert(list_t* list, const void* value, LIST_STYPE position);

/**
 * \brief           Update data at a specific position in the list
 *
 * \param[in]       list: pointer to list object
 * \param[in]       value: pointer to value to be updated
 * \param[in]       position: position where to update data
 *
 * \return          UTILS_STATUS_SUCCESS if data can be updated correctly, UTILS_STATUS_ERROR if position is invalid
 */
utilsStatus_t listUpdate(list_t* list, const void* value, LIST_STYPE position);

/**
 * \brief           Read data from beginning of list, removing it
 *
 * \param[in]       list: pointer to list object
 * \param[out]      value: pointer to value to be read and removed
 *
 * \return          UTILS_STATUS_SUCCESS if data can be read and removed correctly, UTILS_STATUS_EMPTY if list is empty
 */
utilsStatus_t listPop(list_t* list, void* value);

/**
 * \brief           Read data from end of list, removing it
 *
 * \param[in]       list: pointer to list object
 * \param[out]      value: pointer to value to be read and removed
 *
 * \return          UTILS_STATUS_SUCCESS if data can be read and removed correctly, UTILS_STATUS_EMPTY if list is empty
 */
utilsStatus_t listPopBack(list_t* list, void* value);

/**
 * \brief           Read data from a specific position within list, removing it
 *
 * \param[in]       list: pointer to list object
 * \param[out]      value: pointer to value to be read and removed
 * \param[in]       position: position where to read data
 *
 * \return          UTILS_STATUS_SUCCESS if data can be read correctly, UTILS_STATUS_EMPTY if list is empty, UTILS_STATUS_ERROR if position is invalid
 */
utilsStatus_t listRemove(list_t* list, void* value, LIST_STYPE position);

/**
 * \brief           Read data from beginning of list, without removing it
 *
 * \param[in]       list: pointer to list object
 * \param[out]      value: pointer to value to be read
 *
 * \return          UTILS_STATUS_SUCCESS if data can be read correctly, UTILS_STATUS_EMPTY if list is empty
 */
utilsStatus_t listPeek(const list_t* list, void* value);

/**
 * \brief           Read data from end of list, without removing it
 *
 * \param[in]       list: pointer to list object
 * \param[out]      value: pointer to value to be read
 *
 * \return          UTILS_STATUS_SUCCESS if data can be read correctly, UTILS_STATUS_EMPTY if list is empty
 */
utilsStatus_t listPeekBack(const list_t* list, void* value);

/**
 * \brief           Read data from a specific position within list, without removing it
 *
 * \param[in]       list: pointer to list object
 * \param[out]      value: pointer to value to be read
 * \param[in]       position: position where to read data
 *
 * \return          UTILS_STATUS_SUCCESS if data can be read correctly, UTILS_STATUS_EMPTY if list is empty, UTILS_STATUS_ERROR if position is invalid
 */
utilsStatus_t listPeekAtPos(const list_t* list, void* value, LIST_STYPE position);

/**
 * \brief           Returns list info
 *
 * \param[in]       list: pointer to list object
 * \param[out]      size: pointer to size
 * \param[out]      items: pointer to number of items currently in the list
 */
static inline void listInfo(const list_t* list, LIST_STYPE* size, LIST_STYPE* items) {
    if (size != NULL) {
        *size = list->size;
    }
    if (items != NULL) {
        *items = list->items;
    }
}

/**
 * \brief           Flush list removing all values
 *
 * \param[in]       list: pointer to list object
 *
 * \return          UTILS_STATUS_SUCCESS if list is flushed correctly, UTILS_STATUS_EMPTY if list is already empty
 */
utilsStatus_t listFlush(list_t* list);

/**
 * \brief           Create new iterator
 *
 * \param[in]       it: pointer to iterator object
 * \param[in]       list: pointer to list object
 */
void listIt(listIterator_t* it, list_t* list);

/**
 * \brief           Move iterator to next item in list
 *
 * \param[in]       it: pointer to iterator object
 *
 * \return          UTILS_STATUS_SUCCESS if iterator is moved to next value, UTILS_STATUS_ERROR if there are no more items
 */
utilsStatus_t listItNext(listIterator_t* it);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef __cplusplus
}
#endif

#endif /* __LIST_H__ */
