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
#include "hashFunctions.h"

#ifdef ADVUTILS_UNIT_TESTS
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include <cmocka.h>
#else
#ifdef ADVUTILS_MEMORY_MGMT_HEADER
#if !defined(ADVUTILS_MALLOC) || !defined(ADVUTILS_CALLOC) || !defined(ADVUTILS_FREE)
#error ADVUTILS_MALLOC, ADVUTILS_CALLOC and ADVUTILS_FREE must be defined by the user!
#else
#include ADVUTILS_MEMORY_MGMT_HEADER
#endif /* !defined(ADVUTILS_MALLOC) || !defined(ADVUTILS_CALLOC) || !defined(ADVUTILS_FREE) */
#else
#include <stdlib.h>
#endif /* ADVUTILS_MEMORY_MGMT_HEADER */
#endif /* ADVUTILS_UNIT_TESTS */

/* Macros --------------------------------------------------------------------*/

#ifdef ADVUTILS_UNIT_TESTS
#define ADVUTILS_MALLOC test_malloc
#define ADVUTILS_CALLOC test_calloc
#define ADVUTILS_FREE   test_free
#elif !defined(ADVUTILS_MEMORY_MGMT_HEADER)
#define ADVUTILS_MALLOC malloc
#define ADVUTILS_CALLOC calloc
#define ADVUTILS_FREE   free
#endif /* ADVUTILS_UNIT_TESTS */

#define LPHT_HASHFUN(x) hash_FNV1A(x)

#ifndef ADVUTILS_ASSERT
#ifdef DEBUG
#ifdef ADVUTILS_UNIT_TESTS
#define ADVUTILS_ASSERT(x) assert_true(x)
#else
#define ADVUTILS_ASSERT(x)                                                                                             \
    if ((x) == 0) {                                                                                                    \
        for (;;)                                                                                                       \
            ;                                                                                                          \
    }
#endif /* ADVUTILS_UNIT_TESTS */
#else
#define ADVUTILS_ASSERT(x)
#endif /* DEBUG */
#endif /* ADVUTILS_ASSERT */

/* Private Functions ---------------------------------------------------------*/

static inline char* lkHashTableStrdup(const char* s) {
    size_t bufsize = strlen(s) + 1;
    char* retval = ADVUTILS_MALLOC(bufsize);
    ADVUTILS_ASSERT(retval != NULL);
    if (retval) {
        memcpy(retval, s, bufsize);
    }
    return retval;
}

/* Functions -----------------------------------------------------------------*/

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION

utilsStatus_t lkHashTableInit(lkHashTable_t* lkht, size_t itemSize, uint32_t size) {
    uint32_t ii;
    lkht->items = 0;
    lkht->size = size;
    lkht->itemSize = itemSize;

    lkht->entries = ADVUTILS_CALLOC(lkht->size, sizeof(list_t));
    ADVUTILS_ASSERT(lkht->entries != NULL);
    if (lkht->entries == NULL) {
        return UTILS_STATUS_ERROR;
    }

    for (ii = 0; ii < lkht->size; ii++) {
        listInit(&(lkht->entries[ii]), sizeof(lkHashTableEntry_t), UINT16_MAX);
    }

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t lkHashTablePut(lkHashTable_t* lkht, char* key, void* value) {
    if ((value == NULL) || (key == NULL)) {
        return UTILS_STATUS_ERROR;
    }

    if (lkht->items == lkht->size) {
        return UTILS_STATUS_FULL;
    }

    /* limit hash to current memory size */
    uint32_t ii = LPHT_HASHFUN(key) & (lkht->size - 1);

    lkHashTableEntry_t entry;

    /* check if entry exists and update it */
    LIST_STYPE idx;

    for (idx = 0; idx < lkht->entries[ii].items; idx++) {
        if (listPeekAtPos(&(lkht->entries[ii]), &entry, idx) == UTILS_STATUS_SUCCESS) {
            if (!strcmp(key, entry.key)) {
                memcpy(entry.value, value, lkht->itemSize);
                return UTILS_STATUS_SUCCESS;
            }
        }
    }

    /* set entry and update length */
    entry.key = lkHashTableStrdup(key);
    entry.value = ADVUTILS_CALLOC(1, lkht->itemSize);
    ADVUTILS_ASSERT(entry.value != NULL);
    memcpy(entry.value, value, lkht->itemSize);

    if (listPush(&(lkht->entries[ii]), &entry) == UTILS_STATUS_SUCCESS) {
        lkht->items++;
        return UTILS_STATUS_SUCCESS;
    }
    return UTILS_STATUS_ERROR;
}

utilsStatus_t lkHashTableGet(lkHashTable_t* lkht, char* key, void* value, lkHashTableRemoval_t remove) {

    if (!lkht->items) {
        return UTILS_STATUS_EMPTY;
    }

    uint32_t ii = LPHT_HASHFUN(key) & (lkht->size - 1);

    if (!lkht->entries[ii].items) {
        return UTILS_STATUS_BUCKET_EMPTY;
    }

    /* check if entry exists and return it */
    lkHashTableEntry_t entry;
    listIterator_t iterator;

    listIt(&iterator, &(lkht->entries[ii]));

    while (listItNext(&iterator) == UTILS_STATUS_SUCCESS) {
        memcpy(&entry, iterator.ptr->data, sizeof(lkHashTableEntry_t));

        if (!strcmp(key, entry.key)) {
            memcpy(value, entry.value, lkht->itemSize);
            if (remove == LKHT_REMOVE_ITEM) {
                listRemove(&(lkht->entries[ii]), &entry, iterator.idx);
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
    uint32_t ii;

    if (!lkht->items) {
        return UTILS_STATUS_SUCCESS;
    }

    lkHashTableEntry_t entry;
    LIST_STYPE idx;

    for (ii = 0; ii < lkht->size; ii++) {
        idx = lkht->entries[ii].items;

        while (idx--) {
            listPop(&(lkht->entries[ii]), &entry);
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

    lkHashTableFlush(lkht);

    ADVUTILS_FREE(lkht->entries);

    return UTILS_STATUS_SUCCESS;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */
