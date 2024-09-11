/* BEGIN Header */
/**
 ******************************************************************************
 * \file            LKHashTable.h
 * \author          Andrea Vivani
 * \brief           Implementation of a simple dynamic linked hash-table
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
#ifndef __LINKEDHASHTABLE_H__
#define __LINKEDHASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stddef.h>
#include <stdint.h>
#include "commonTypes.h"
#include "list.h"

/* Macros --------------------------------------------------------------------*/

/* Maximum size of the list objects used by the hash-table */
#ifndef LKHT_LIST_SIZE
#define LKHT_LIST_SIZE UINT16_MAX
#endif /* LKHT_LIST_SIZE */

/* Hash function used by hash-table */
#ifndef LKHT_HASHFUN
#define LKHT_HASHFUN(x) hash_FNV1A(x)
#endif /* LKHT_HASHFUN */

/* Typedefs ------------------------------------------------------------------*/

/*
 * Hash-Table entry struct
 */
typedef struct {
    char* key; /* key is NULL if this slot is empty */
    void* value;
} lkHashTableEntry_t;

/*
 * Hash-Table struct
 */
typedef struct {
    list_t* entries; /* hash slots */
    uint32_t size, items;
    size_t itemSize;
} lkHashTable_t;

/**
 * Hash-Table removal setting
 */
typedef enum { LKHT_REMOVE_ITEM = 0, LKHT_DO_NOT_REMOVE_ITEM = 1 } lkHashTableRemoval_t;

/* Function prototypes -------------------------------------------------------*/

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Init linked hash-table structure
 *
 * \param[in]       lkht: pointer to hash-table object
 * \param[in]       itemSize: size of items to be handled by hash-table
 * \param[in]       size: max number of items that can be allocated in the hash-table
 * 
 * \return          UTILS_STATUS_SUCCESS if hash-table was initialized, UTILS_STATUS_ERROR if data was not allocated correctly
 */
utilsStatus_t lkHashTableInit(lkHashTable_t* lkht, size_t itemSize, uint32_t size);

/**
 * \brief           Set item with given key to value. If not already present in table, key is copied to newly allocated memory, otherwise it is only updated.
 *
 * \param[in]       lkht: pointer to hash-table object
 * \param[in]       key: pointer to key
 * \param[in]       value: pointer to value to be written
 * 
 * \return          UTILS_STATUS_SUCCESS if data is written correctly, UTILS_STATUS_ERROR otherwise
 */
utilsStatus_t lkHashTablePut(lkHashTable_t* lkht, char* key, void* value);

/**
 * \brief           Get item with given key from hash table
 *
 * \param[in]       lkht: pointer to hash-table object
 * \param[in]       key: pointer to key
 * \param[out]      value: pointer to value to be read
 * \param[in]       remove: LKHT_REMOVE_ITEM if item should be removed after being read, LKHT_DO_NOT_REMOVE_ITEM if item should not be removed
 * 
 * \return          UTILS_STATUS_SUCCESS if data is read correctly, UTILS_STATUS_ERROR if data 
 */
utilsStatus_t lkHashTableGet(lkHashTable_t* lkht, char* key, void* value, lkHashTableRemoval_t remove);

/**
 * \brief           Returns hash-table info
 *
 * \param[in]       lkht: pointer to hash-table object
 * \param[out]      size: pointer to size
 * \param[out]      items: pointer to number of items currently in the hash-table
 */
static inline void lkHashTableInfo(lkHashTable_t* lkht, uint32_t* size, uint32_t* items) {
    *size = lkht->size;
    *items = lkht->items;
}

/**
 * \brief           Flush hash-table removing all values
 *
 * \param[in]       lkht: pointer to hash-table object
 *
 * \return          UTILS_STATUS_SUCCESS if list is flushed correctly, UTILS_STATUS_ERROR if data cannot be flushed
 */
utilsStatus_t lkHashTableFlush(lkHashTable_t* lkht);

/**
 * \brief           Delete hash-table
 *
 * \param[in]       lkht: pointer to hash-table object
 *
 * \return          UTILS_STATUS_SUCCESS if hash-table is deleted, UTILS_STATUS_ERROR if hash-table was already deleted
 */
utilsStatus_t lkHashTableDelete(lkHashTable_t* lkht);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef __cplusplus
}
#endif

#endif /* __LINKEDHASHTABLE_H__ */
