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
#ifndef ADVUTILS_LIST_H
#define ADVUTILS_LIST_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stddef.h>
#include <stdint.h>
#include "commonTypes.h"

/**
 * \defgroup list Linked list
 * \brief Linked list
 * @{
 */

/* Macros --------------------------------------------------------------------*/

/* Type of list size property */
#ifndef LIST_STYPE
/**
 * \def LIST_STYPE
 * \brief Unsigned integer type for list length and indices (default uint16_t)
 */
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
    listNode_t* _front;
    listNode_t* _rear;
    LIST_STYPE size;
    LIST_STYPE items;
} list_t;

/**
 * List iterator 
 */
typedef struct {
    list_t* _list;
    listNode_t* _prev;
    listNode_t* ptr;
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
 * \retval          UTILS_STATUS_SUCCESS data pushed correctly
 * \retval          UTILS_STATUS_FULL list is full
 * \retval          UTILS_STATUS_ERROR pointer is NULL
 */
utilsStatus_t listPush(list_t* list, const void* value);

/**
 * \brief           Add data to beginning of list
 *
 * \param[in]       list: pointer to list object
 * \param[in]       value: pointer to value to be pushed to front
 *
 * \retval          UTILS_STATUS_SUCCESS data pushed to front correctly
 * \retval          UTILS_STATUS_FULL list is full
 * \retval          UTILS_STATUS_ERROR pointer is NULL
 */
utilsStatus_t listPushFront(list_t* list, const void* value);

/**
 * \brief           Add data to a specific position within list
 *
 * \param[in]       list: pointer to list object
 * \param[in]       value: pointer to value to be pushed to front
 * \param[in]       position: position where to add data
 *
 * \retval          UTILS_STATUS_SUCCESS data added correctly
 * \retval          UTILS_STATUS_FULL list is full
 * \retval          UTILS_STATUS_ERROR position is invalid or pointer is NULL
 */
utilsStatus_t listInsert(list_t* list, const void* value, LIST_STYPE position);

/**
 * \brief           Update data at a specific position in the list
 *
 * \param[in]       list: pointer to list object
 * \param[in]       value: pointer to value to be updated
 * \param[in]       position: position where to update data
 *
 * \retval          UTILS_STATUS_SUCCESS data updated correctly
 * \retval          UTILS_STATUS_ERROR position is invalid
 */
utilsStatus_t listUpdate(list_t* list, const void* value, LIST_STYPE position);

/**
 * \brief           Read data from beginning of list, removing it
 *
 * \param[in]       list: pointer to list object
 * \param[out]      value: pointer to value to be read and removed
 *
 * \retval          UTILS_STATUS_SUCCESS data read and removed correctly
 * \retval          UTILS_STATUS_EMPTY list is empty
 */
utilsStatus_t listPop(list_t* list, void* value);

/**
 * \brief           Read data from end of list, removing it
 *
 * \param[in]       list: pointer to list object
 * \param[out]      value: pointer to value to be read and removed
 *
 * \retval          UTILS_STATUS_SUCCESS data read and removed correctly
 * \retval          UTILS_STATUS_EMPTY list is empty
 */
utilsStatus_t listPopBack(list_t* list, void* value);

/**
 * \brief           Read data from a specific position within list, removing it
 *
 * \param[in]       list: pointer to list object
 * \param[out]      value: pointer to value to be read and removed
 * \param[in]       position: position where to read data
 *
 * \retval          UTILS_STATUS_SUCCESS data read correctly
 * \retval          UTILS_STATUS_EMPTY list is empty
 * \retval          UTILS_STATUS_ERROR position is invalid
 */
utilsStatus_t listRemove(list_t* list, void* value, LIST_STYPE position);

/**
 * \brief           Read data from beginning of list, without removing it
 *
 * \param[in]       list: pointer to list object
 * \param[out]      value: pointer to value to be read
 *
 * \retval          UTILS_STATUS_SUCCESS data read correctly
 * \retval          UTILS_STATUS_EMPTY list is empty
 */
utilsStatus_t listPeek(const list_t* list, void* value);

/**
 * \brief           Read data from end of list, without removing it
 *
 * \param[in]       list: pointer to list object
 * \param[out]      value: pointer to value to be read
 *
 * \retval          UTILS_STATUS_SUCCESS data read correctly
 * \retval          UTILS_STATUS_EMPTY list is empty
 */
utilsStatus_t listPeekBack(const list_t* list, void* value);

/**
 * \brief           Read data from a specific position within list, without removing it
 *
 * \param[in]       list: pointer to list object
 * \param[out]      value: pointer to value to be read
 * \param[in]       position: position where to read data
 *
 * \retval          UTILS_STATUS_SUCCESS data read correctly
 * \retval          UTILS_STATUS_EMPTY list is empty
 * \retval          UTILS_STATUS_ERROR position is invalid
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
 * \retval          UTILS_STATUS_SUCCESS list is flushed correctly
 * \retval          UTILS_STATUS_EMPTY list is already empty
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
 * \retval          UTILS_STATUS_SUCCESS iterator is moved to next value
 * \retval          UTILS_STATUS_ERROR there are no more items
 */
utilsStatus_t listItNext(listIterator_t* it);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ADVUTILS_LIST_H */
