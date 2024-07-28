/* BEGIN Header */
/**
 ******************************************************************************
 * \file            LPHashTable.h
 * \author          Andrea Vivani
 * \brief           Implementation of a simple linear probing hash-table
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
#ifndef __LPHASHTABLE_H__
#define __LPHASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stddef.h>
#include <stdint.h>
#include "commonTypes.h"

/* Macros --------------------------------------------------------------------*/

#define LPHT_MIN_SIZE       5
#define LPHT_MIN_SATURATION 0.2
#define LPHT_MAX_SATURATION 0.7

/* Typedefs ------------------------------------------------------------------*/

/**
 * Hash-Table resizability setting
 */
typedef enum { LPHT_NOT_RESIZABLE = 0, LPHT_RESIZABLE = 1 } lpHashTableResizable_t;

/*
 * Hash-Table entry struct
 */
typedef struct {
    char* key; /* key is NULL if this slot is empty */
    void* value;
} lpHashTableEntry_t;

/*
 * Hash-Table struct
 */
typedef struct {
    lpHashTableEntry_t* entries; /* hash slots */
    uint32_t size, items;
    size_t itemSize;
    lpHashTableResizable_t resizable;
} lpHashTable_t;

/**
 * Hash-Table iterator 
 */
typedef struct {
    char* key;
    void* value;
    lpHashTable_t* _lpht;
    uint32_t _index;
} lpHashTableIterator_t;

/**
 * Hash-Table removal setting
 */
typedef enum { LPHT_REMOVE_ITEM = 0, LPHT_DO_NOT_REMOVE_ITEM = 1 } lpHashTableRemoval_t;

/* Function prototypes -------------------------------------------------------*/

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Init linear-probing hash-table structure
 *
 * \param[in]       lpht: pointer to hash-table object
 * \param[in]       itemSize: size of items to be handled by hash-table
 * \param[in]       init_items: number of items to be initially allocated in the hash-table
 * \param[in]       resizable: LPHT_RESIZABLE if table should be resized once max saturation is reached, LPHT_NOT_RESIZABLE otherwise
 * 
 * \return          UTILS_STATUS_SUCCESS if hash-table was initialized, UTILS_STATUS_ERROR if data was not allocated correctly
 */
utilsStatus_t lpHashTableInit(lpHashTable_t* lpht, size_t itemSize, uint32_t init_items,
                              lpHashTableResizable_t resizable);

/**
 * \brief           Set item with given key to value. If not already present in table, key is copied to newly allocated memory, otherwise it is only updated.
 *
 * \param[in]       lpht: pointer to hash-table object
 * \param[in]       key: pointer to key
 * \param[in]       value: pointer to value to be written
 * 
 * \return          UTILS_STATUS_SUCCESS if data is written correctly, UTILS_STATUS_ERROR otherwise 
 */
utilsStatus_t lpHashTablePut(lpHashTable_t* lpht, char* key, void* value);

/**
 * \brief           Get item with given key from hash table
 *
 * \param[in]       lpht: pointer to hash-table object
 * \param[in]       key: pointer to key
 * \param[out]      value: pointer to value to be read
 * \param[in]       remove: LPHT_REMOVE_ITEM if item should be removed after being read, LPHT_DO_NOT_REMOVE_ITEM if item should not be removed
 * 
 * \return          UTILS_STATUS_SUCCESS if data is read correctly, UTILS_STATUS_ERROR if data 
 */
utilsStatus_t lpHashTableGet(lpHashTable_t* lpht, char* key, void* value, lpHashTableRemoval_t remove);

/**
 * \brief           Returns hash-table info
 *
 * \param[in]       lpht: pointer to hash-table object
 * \param[out]      size: pointer to size
 * \param[out]      items: pointer to number of items currently in the hash-table
 */
static inline void lpHashTableInfo(lpHashTable_t* lpht, uint32_t* size, uint32_t* items) {
    *size = lpht->size;
    *items = lpht->items;
}

/**
 * \brief           Flush hash-table removing all values
 *
 * \param[in]       lpht: pointer to hash-table object
 *
 * \return          UTILS_STATUS_SUCCESS if list is flushed correctly, UTILS_STATUS_ERROR if data cannot be flushed
 */
utilsStatus_t lpHashTableFlush(lpHashTable_t* lpht);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef __cplusplus
}
#endif

#endif /* __LPHASHTABLE_H__ */
