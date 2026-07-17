/* BEGIN Header */
/**
 ******************************************************************************
 * \file            LKHashTable.c
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

/* Configuration check -------------------------------------------------------*/
#if !defined(ADVUTILS_USE_DYNAMIC_ALLOCATION) && !defined(ADVUTILS_USE_STATIC_ALLOCATION)
#error Either ADVUTILS_USE_DYNAMIC_ALLOCATION or ADVUTILS_USE_STATIC_ALLOCATION must be set for ADVUtils to work
#endif

/* Includes ------------------------------------------------------------------*/

#include "LKHashTable.h"
#include <string.h>
#include "ADVUtilsAssert.h"
#include "ADVUtilsMemory.h"
#include "hashFunctions.h"

/* Maximum size of the list objects used by the hash-table */
#ifndef LKHT_LIST_SIZE
#define LKHT_LIST_SIZE UINT16_MAX
#endif /* LKHT_LIST_SIZE */

/* Hash function used by hash-table */
#ifndef LKHT_HASHFUN
#define LKHT_HASHFUN hash_FNV1A
#endif /* LKHT_HASHFUN */

/* Private Functions ---------------------------------------------------------*/

static inline char* lkHashTableStrdup(const char* s) {
    size_t bufsize = strlen(s) + 1U;
    /* cppcheck-suppress misra-c2012-11.5 ; deviation: generic container returns typed pointer from void* storage */
    char* retval = ADVUTILS_MALLOC(bufsize);
    ADVUTILS_ASSERT(retval != NULL);
    if (retval != NULL) {
        (void)memcpy(retval, s, bufsize);
    }
    return retval;
}

/* Functions -----------------------------------------------------------------*/

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION

utilsStatus_t lkHashTableInit(lkHashTable_t* lkht, size_t itemSize, uint32_t size) {
    lkht->items = 0;
    lkht->size = size;
    lkht->itemSize = itemSize;

    /* cppcheck-suppress misra-c2012-11.5 ; deviation: generic container returns typed pointer from void* storage */
    lkht->entries = ADVUTILS_CALLOC(lkht->size, sizeof(list_t));
    ADVUTILS_ASSERT(lkht->entries != NULL);
    if (lkht->entries == NULL) {
        return UTILS_STATUS_ERROR;
    }

    for (uint32_t i = 0; i < lkht->size; i++) {
        listInit(&(lkht->entries[i]), sizeof(lkHashTableEntry_t), LKHT_LIST_SIZE);
    }

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t lkHashTablePut(lkHashTable_t* lkht, const char* key, const void* value) {
    if ((value == NULL) || (key == NULL)) {
        return UTILS_STATUS_ERROR;
    }

    if (lkht->items == lkht->size) {
        return UTILS_STATUS_FULL;
    }

    /* limit hash to current memory size */
    uint32_t ii = LKHT_HASHFUN(key) & (lkht->size - 1U);

    lkHashTableEntry_t entry;

    /* check if entry exists and update it */
    LIST_STYPE idx;

    for (idx = 0; idx < lkht->entries[ii].items; idx++) {
        (void)listPeekAtPos(&(lkht->entries[ii]), &entry, idx);
        if (!strcmp(key, entry.key)) {
            (void)memcpy(entry.value, value, lkht->itemSize);
            return UTILS_STATUS_SUCCESS;
        }
    }

    /* set entry and update length */
    entry.key = lkHashTableStrdup(key);
    entry.value = ADVUTILS_CALLOC(1, lkht->itemSize);
    ADVUTILS_ASSERT(entry.value != NULL);

    if ((entry.key == NULL) || (entry.value == NULL)) {
        ADVUTILS_FREE(entry.key);
        ADVUTILS_FREE(entry.value);
        return UTILS_STATUS_ERROR;
    }

    (void)memcpy(entry.value, value, lkht->itemSize);

    if (listPush(&(lkht->entries[ii]), &entry) == UTILS_STATUS_SUCCESS) {
        lkht->items++;
        return UTILS_STATUS_SUCCESS;
    }
    ADVUTILS_FREE(entry.key);
    ADVUTILS_FREE(entry.value);
    return UTILS_STATUS_ERROR;
}

utilsStatus_t lkHashTableGet(lkHashTable_t* lkht, const char* key, void* value, lkHashTableRemoval_t remove) {

    if (!lkht->items) {
        return UTILS_STATUS_EMPTY;
    }

    uint32_t ii = LKHT_HASHFUN(key) & (lkht->size - 1U);

    if (!lkht->entries[ii].items) {
        return UTILS_STATUS_BUCKET_EMPTY;
    }

    /* check if entry exists and return it */
    lkHashTableEntry_t entry;
    listIterator_t iterator;

    listIt(&iterator, &(lkht->entries[ii]));

    while (listItNext(&iterator) == UTILS_STATUS_SUCCESS) {
        (void)memcpy(&entry, iterator.ptr->data, sizeof(lkHashTableEntry_t));

        if (!strcmp(key, entry.key)) {
            (void)memcpy(value, entry.value, lkht->itemSize);
            if (remove == LKHT_REMOVE_ITEM) {
                (void)listRemove(&(lkht->entries[ii]), &entry, iterator.idx);
                lkht->items--;
                ADVUTILS_FREE(entry.key);
                ADVUTILS_FREE(entry.value);
                return UTILS_STATUS_SUCCESS;
            }
            return UTILS_STATUS_SUCCESS;
        }
    }

    return UTILS_STATUS_ERROR;
}

utilsStatus_t lkHashTableFlush(lkHashTable_t* lkht) {
    if (!lkht->items) {
        return UTILS_STATUS_SUCCESS;
    }

    lkHashTableEntry_t entry;

    for (uint32_t i = 0; i < lkht->size; i++) {
        LIST_STYPE idx = lkht->entries[i].items;

        while (idx-- != 0U) {
            (void)listPop(&(lkht->entries[i]), &entry);
            ADVUTILS_FREE(entry.key);
            ADVUTILS_FREE(entry.value);
        }
    }

    lkht->items = 0;

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t lkHashTableDelete(lkHashTable_t* lkht) {

    if (lkht->entries == NULL) {
        return UTILS_STATUS_ERROR;
    }

    (void)lkHashTableFlush(lkht);

    ADVUTILS_FREE(lkht->entries);
    lkht->entries = NULL;

    return UTILS_STATUS_SUCCESS;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */
