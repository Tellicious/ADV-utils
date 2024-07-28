/* BEGIN Header */
/**
 ******************************************************************************
 * \file            LPHashTable.c
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

/* Configuration check -------------------------------------------------------*/
#if !defined(ADVUTILS_USE_DYNAMIC_ALLOCATION) && !defined(ADVUTILS_USE_STATIC_ALLOCATION)
#error Either ADVUTILS_USE_DYNAMIC_ALLOCATION or ADVUTILS_USE_STATIC_ALLOCATION must be set for ADVUtils to work
#endif

/* Includes ------------------------------------------------------------------*/

#include "LPHashTable.h"
#include <string.h>
#include "hashFunctions.h"
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

#define LPHT_HASHFUN(x) hash_FNV1A(x)

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

/* Function prototypes -------------------------------------------------------*/

static utilsStatus_t lpHashTableSetEntry(lpHashTable_t* lpht, char* key, void* value);
static utilsStatus_t lpHashTableXpand(lpHashTable_t* lpht, uint8_t increase);

/* Private Functions ---------------------------------------------------------*/

static inline char* lpHashTableStrdup(const char* s) {
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

utilsStatus_t lpHashTableInit(lpHashTable_t* lpht, size_t itemSize, uint32_t init_items,
                              lpHashTableResizable_t resizable) {
    lpht->items = 0;
    lpht->size = init_items;
    lpht->itemSize = itemSize;
    lpht->resizable = resizable;

    lpht->entries = ADVUTILS_CALLOC(lpht->size, sizeof(lpHashTableEntry_t));
    ADVUTILS_ASSERT(lpht->entries != NULL);
    if (lpht->entries == NULL) {
        return UTILS_STATUS_ERROR;
    }

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t lpHashTablePut(lpHashTable_t* lpht, char* key, void* value) {
    if ((value == NULL) || (key == NULL)) {
        return UTILS_STATUS_ERROR;
    }

    /* If length will exceed half of current capacity, expand it. */
    if ((lpht->items >= (lpht->size * LPHT_MAX_SATURATION)) && (lpht->resizable == LPHT_RESIZABLE)) {
        if (lpHashTableXpand(lpht, 1) != UTILS_STATUS_SUCCESS) {
            return UTILS_STATUS_ERROR;
        }
    } else if (lpht->items == lpht->size) {
        return UTILS_STATUS_FULL;
    } else if ((lpht->items <= (lpht->size * LPHT_MIN_SATURATION)) && (lpht->size > LPHT_MIN_SIZE)
               && (lpht->resizable == LPHT_RESIZABLE)) {
        if (lpHashTableXpand(lpht, 0) != UTILS_STATUS_SUCCESS) {
            return UTILS_STATUS_ERROR;
        }
    }

    /* Set entry and update length. */

    if (lpHashTableSetEntry(lpht, key, value) == UTILS_STATUS_SUCCESS) {
        return UTILS_STATUS_SUCCESS;
    }

    return UTILS_STATUS_ERROR;
}

utilsStatus_t lpHashTableGet(lpHashTable_t* lpht, char* key, void* value, lpHashTableRemoval_t remove) {
    uint32_t ii = LPHT_HASHFUN(key) & (lpht->size - 1);

    while (lpht->entries[ii].key != NULL) {
        if (!strcmp(key, lpht->entries[ii].key)) {
            memcpy(value, lpht->entries[ii].value, lpht->itemSize);

            if (remove == LPHT_REMOVE_ITEM) {
                ADVUTILS_FREE(lpht->entries[ii].key);
                ADVUTILS_FREE(lpht->entries[ii].value);
                lpht->entries[ii].key = NULL;
                lpht->entries[ii].value = NULL;
                lpht->items--;
            }

            return UTILS_STATUS_SUCCESS;
        }

        ii++;

        if (ii >= lpht->size) {
            ii = 0;
        }
    }
    return UTILS_STATUS_ERROR;
}

utilsStatus_t lpHashTableFlush(lpHashTable_t* lpht) {
    uint32_t ii;

    if (!lpht->items) {
        return UTILS_STATUS_EMPTY;
    }

    for (ii = 0; ii < lpht->size; ii++) {
        ADVUTILS_FREE(lpht->entries[ii].key);
        ADVUTILS_FREE(lpht->entries[ii].value);
        lpht->entries[ii].key = NULL;
        lpht->entries[ii].value = NULL;
    }

    lpht->items = 0;

    return UTILS_STATUS_SUCCESS;
}

static utilsStatus_t lpHashTableSetEntry(lpHashTable_t* lpht, char* key, void* value) {
    /* limit hash to current memory size */
    uint32_t ii = LPHT_HASHFUN(key) & (lpht->size - 1);

    /* Loop till we find an empty entry. */
    while (lpht->entries[ii].key != NULL) {
        if (!strcmp(key, lpht->entries[ii].key)) {
            /* Found key (it already exists), update value. */
            memcpy(lpht->entries[ii].value, value, lpht->itemSize);
            return UTILS_STATUS_SUCCESS;
        }
        ii++;
        if (ii >= lpht->size) {
            ii = 0;
        }
    }

    /* Didn't find key, allocate+copy if needed, then insert it. */
    lpht->entries[ii].key = lpHashTableStrdup(key);
    lpht->entries[ii].value = ADVUTILS_CALLOC(1, lpht->itemSize);
    ADVUTILS_ASSERT(lpht->entries[ii].value != NULL);

    if ((lpht->entries[ii].key == NULL) || (lpht->entries[ii].value == NULL)) {
        ADVUTILS_FREE(lpht->entries[ii].key);
        ADVUTILS_FREE(lpht->entries[ii].value);
        return UTILS_STATUS_ERROR;
    }

    memcpy(lpht->entries[ii].value, value, lpht->itemSize);

    lpht->items++;

    return UTILS_STATUS_SUCCESS;
}

static utilsStatus_t lpHashTableXpand(lpHashTable_t* lpht, uint8_t increase) {
    /* Allocate new entries array. */
    uint32_t ii;
    uint32_t old_size = lpht->size;

    /* hash is currently a uint32_t so Hash-table size should not exceed that */
    if (increase) {
        /* increase table size */
        if (lpht->size == UINT32_MAX) {
            return UTILS_STATUS_ERROR;
        } else if (lpht->size >= (UINT32_MAX >> 1)) {
            lpht->size = UINT32_MAX;
        } else {
            lpht->size *= 2;
        }
    } else {
        /* decrease table size */
        if (lpht->size >= (LPHT_MIN_SIZE * 2)) {
            lpht->size = (lpht->size >> 1);
        } else {
            lpht->size = LPHT_MIN_SIZE;
        }
    }

    lpHashTableEntry_t* old_entries = lpht->entries;
    lpht->entries = ADVUTILS_CALLOC(lpht->size, sizeof(lpHashTableEntry_t));
    ADVUTILS_ASSERT(lpht->entries != NULL);
    if (lpht->entries == NULL) {
        lpht->size = old_size;
        lpht->entries = old_entries;
        return UTILS_STATUS_ERROR;
    }

    /* Iterate entries, move all non-empty ones to new table's entries. */
    lpht->items = 0;

    for (ii = 0; ii < old_size; ii++) {
        if (old_entries[ii].key != NULL) {
            lpHashTableSetEntry(lpht, old_entries[ii].key, old_entries[ii].value);
            ADVUTILS_FREE(old_entries[ii].key);
            ADVUTILS_FREE(old_entries[ii].value);
        }
    }

    ADVUTILS_FREE(old_entries);

    return UTILS_STATUS_SUCCESS;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */
