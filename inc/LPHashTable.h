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
#ifndef ADVUTILS_LPHASHTABLE_H
#define ADVUTILS_LPHASHTABLE_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stddef.h>
#include <stdint.h>
#include "commonTypes.h"

/**
 * \defgroup LPHashTable Linear-probing hash table
 * \brief Linear-probing hash table
 * @{
 */

/* Typedefs ------------------------------------------------------------------*/

/**
 * Hash-Table resizability setting
 */
typedef enum { LPHT_NOT_RESIZABLE = 0, LPHT_RESIZABLE = 1 } lpHashTableResizable_t;

/**
 * \brief           Linear-probing hash-table entry
 */
typedef struct {
    char* key;   /**< Entry key; NULL if the slot is empty */
    void* value; /**< Pointer to the stored value */
} lpHashTableEntry_t;

/**
 * \brief           Linear-probing hash-table handle
 */
typedef struct {
    lpHashTableEntry_t* entries;      /**< Array of hash slots */
    uint32_t size;                    /**< Number of slots */
    uint32_t items;                   /**< Current number of stored items */
    size_t itemSize;                  /**< Size in bytes of each stored value */
    lpHashTableResizable_t resizable; /**< Whether the table may auto-resize */
} lpHashTable_t;

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
 * \retval          UTILS_STATUS_SUCCESS hash-table was initialized
 * \retval          UTILS_STATUS_ERROR data was not allocated correctly
 */
utilsStatus_t lpHashTableInit(lpHashTable_t* lpht, size_t itemSize, uint32_t init_items, lpHashTableResizable_t resizable);

/**
 * \brief           Set item with given key to value. If not already present in table, key is copied to newly allocated memory, otherwise it is only updated.
 *
 * \param[in]       lpht: pointer to hash-table object
 * \param[in]       key: pointer to key
 * \param[in]       value: pointer to value to be written
 * 
 * \retval          UTILS_STATUS_SUCCESS data is written correctly
 * \retval          UTILS_STATUS_ERROR otherwise
 */
utilsStatus_t lpHashTablePut(lpHashTable_t* lpht, const char* key, const void* value);

/**
 * \brief           Get item with given key from hash table
 *
 * \param[in]       lpht: pointer to hash-table object
 * \param[in]       key: pointer to key
 * \param[out]      value: pointer to value to be read
 * \param[in]       remove: LPHT_REMOVE_ITEM if item should be removed after being read, LPHT_DO_NOT_REMOVE_ITEM if item should not be removed
 * 
 * \retval          UTILS_STATUS_SUCCESS data is read correctly
 * \retval          UTILS_STATUS_ERROR data
 */
utilsStatus_t lpHashTableGet(lpHashTable_t* lpht, const char* key, void* value, lpHashTableRemoval_t remove);

/**
 * \brief           Returns hash-table info
 *
 * \param[in]       lpht: pointer to hash-table object
 * \param[out]      size: pointer to size
 * \param[out]      items: pointer to number of items currently in the hash-table
 */
static inline void lpHashTableInfo(const lpHashTable_t* lpht, uint32_t* size, uint32_t* items) {
    *size = lpht->size;
    *items = lpht->items;
}

/**
 * \brief           Flush hash-table removing all values
 *
 * \param[in]       lpht: pointer to hash-table object
 *
 * \retval          UTILS_STATUS_SUCCESS list is flushed correctly
 * \retval          UTILS_STATUS_ERROR data cannot be flushed
 */
utilsStatus_t lpHashTableFlush(lpHashTable_t* lpht);

/**
 * \brief           Delete hash-table
 *
 * \param[in]       lpht: pointer to hash-table object
 *
 * \retval          UTILS_STATUS_SUCCESS hash-table is deleted
 * \retval          UTILS_STATUS_ERROR hash-table was already deleted
 */
utilsStatus_t lpHashTableDelete(lpHashTable_t* lpht);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ADVUTILS_LPHASHTABLE_H */
