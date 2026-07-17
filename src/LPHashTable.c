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
#include "ADVUtilsAssert.h"
#include "ADVUtilsMemory.h"
#include "hashFunctions.h"

/* Minimum size of hash-table when resizing */
#ifndef LPHT_MIN_SIZE
#define LPHT_MIN_SIZE 5
#endif /* LPHT_MIN_SIZE */

/* Maximum size of hash-table when resizing */
#ifndef LPHT_MAX_SIZE
#define LPHT_MAX_SIZE UINT32_MAX
#endif /* LPHT_MAX_SIZE */

/* Minimum saturation of hash-table, that triggers a down-sizing */
#ifndef LPHT_MIN_SATURATION
#define LPHT_MIN_SATURATION 0.2
#endif /* LPHT_MIN_SATURATION */

/* Maximum saturation of hash-table, that triggers an up-sizing */
#ifndef LPHT_MAX_SATURATION
#define LPHT_MAX_SATURATION 0.7
#endif /* LPHT_MAX_SATURATION */

/* Hash function used by hash-table */
#ifndef LPHT_HASHFUN
#define LPHT_HASHFUN hash_FNV1A
#endif /* LPHT_HASHFUN */

/* Function prototypes -------------------------------------------------------*/

static utilsStatus_t lpHashTableSetEntry(lpHashTable_t* lpht, const char* key, const void* value);
static utilsStatus_t lpHashTableUpdateEntry(lpHashTable_t* lpht, const char* key, const void* value);
static void lpHashTableReplaceEntries(lpHashTable_t* lpht, uint32_t startIndex);
static utilsStatus_t lpHashTableXpand(lpHashTable_t* lpht, uint8_t increase);

/* Private Functions ---------------------------------------------------------*/

static inline char* lpHashTableStrdup(const char* s) {
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

utilsStatus_t lpHashTableInit(lpHashTable_t* lpht, size_t itemSize, uint32_t init_items, lpHashTableResizable_t resizable) {
    lpht->items = 0;
    lpht->size = init_items;
    lpht->itemSize = itemSize;
    lpht->resizable = resizable;

    /* cppcheck-suppress misra-c2012-11.5 ; deviation: generic container returns typed pointer from void* storage */
    lpht->entries = ADVUTILS_CALLOC(lpht->size, sizeof(lpHashTableEntry_t));
    ADVUTILS_ASSERT(lpht->entries != NULL);
    if (lpht->entries == NULL) {
        return UTILS_STATUS_ERROR;
    }

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t lpHashTablePut(lpHashTable_t* lpht, const char* key, const void* value) {
    if ((value == NULL) || (key == NULL)) {
        return UTILS_STATUS_ERROR;
    }

    /* Try to update the entry and go on if it doesn't exist already */
    if (lpHashTableUpdateEntry(lpht, key, value) == UTILS_STATUS_SUCCESS) {
        return UTILS_STATUS_SUCCESS;
    }

    /* If length will exceed defined quota of current capacity, expand it. */
    if (((((double)lpht->items + 1.0) >= ((double)lpht->size * LPHT_MAX_SATURATION))) && (lpht->resizable == LPHT_RESIZABLE)) {
        if (lpHashTableXpand(lpht, 1) == UTILS_STATUS_ERROR) {
            return UTILS_STATUS_ERROR;
        }
    }

    /* If table is full, return */
    if (lpht->items >= lpht->size) {
        return UTILS_STATUS_FULL;
    }

    /* Set entry and update length. */
    if (lpHashTableSetEntry(lpht, key, value) == UTILS_STATUS_SUCCESS) {
        return UTILS_STATUS_SUCCESS;
    }

    return UTILS_STATUS_ERROR;
}

utilsStatus_t lpHashTableGet(lpHashTable_t* lpht, const char* key, void* value, lpHashTableRemoval_t remove) {

    if (!lpht->items) {
        return UTILS_STATUS_EMPTY;
    }

    uint32_t ii = LPHT_HASHFUN(key) & (lpht->size - 1U);
    uint32_t cnt = 0;

    while (lpht->entries[ii].key != NULL) {
        if (!strcmp(key, lpht->entries[ii].key)) {
            (void)memcpy(value, lpht->entries[ii].value, lpht->itemSize);

            if (remove == LPHT_REMOVE_ITEM) {
                ADVUTILS_FREE(lpht->entries[ii].key);
                ADVUTILS_FREE(lpht->entries[ii].value);
                lpht->entries[ii].key = NULL;
                lpht->entries[ii].value = NULL;
                if ((--lpht->items <= (lpht->size * LPHT_MIN_SATURATION)) && (lpht->resizable == LPHT_RESIZABLE)) {
                    if (lpHashTableXpand(lpht, 0) == UTILS_STATUS_ERROR) {
                        lpHashTableReplaceEntries(lpht, ii);
                        return UTILS_STATUS_WARNING;
                    }
                } else {
                    lpHashTableReplaceEntries(lpht, ii);
                }
            }
            return UTILS_STATUS_SUCCESS;
        }

        if (++ii >= lpht->size) {
            ii = 0;
        }

        /* Check to avoid infinite loop in case of full not-resizable list */
        if (++cnt >= lpht->items) {
            return UTILS_STATUS_ERROR;
        }
    }
    return UTILS_STATUS_ERROR;
}

utilsStatus_t lpHashTableFlush(lpHashTable_t* lpht) {
    if (!lpht->items) {
        return UTILS_STATUS_EMPTY;
    }

    for (uint32_t i = 0; i < lpht->size; i++) {
        ADVUTILS_FREE(lpht->entries[i].key);
        ADVUTILS_FREE(lpht->entries[i].value);
        lpht->entries[i].key = NULL;
        lpht->entries[i].value = NULL;
    }

    lpht->items = 0;

    return UTILS_STATUS_SUCCESS;
}

utilsStatus_t lpHashTableDelete(lpHashTable_t* lpht) {

    if (lpht->entries == NULL) {
        return UTILS_STATUS_ERROR;
    }

    (void)lpHashTableFlush(lpht);

    ADVUTILS_FREE(lpht->entries);
    lpht->entries = NULL;

    return UTILS_STATUS_SUCCESS;
}

static utilsStatus_t lpHashTableSetEntry(lpHashTable_t* lpht, const char* key, const void* value) {
    /* limit hash to current memory size */
    uint32_t ii = LPHT_HASHFUN(key) & (lpht->size - 1U);

    /* Loop till we find an empty entry. */
    while (lpht->entries[ii].key != NULL) {
        if (++ii >= lpht->size) {
            ii = 0;
        }
    }

    /* Empty slot found, insert item. */
    lpht->entries[ii].key = lpHashTableStrdup(key);
    lpht->entries[ii].value = ADVUTILS_CALLOC(1, lpht->itemSize);
    ADVUTILS_ASSERT(lpht->entries[ii].value != NULL);

    if ((lpht->entries[ii].key == NULL) || (lpht->entries[ii].value == NULL)) {
        ADVUTILS_FREE(lpht->entries[ii].key);
        ADVUTILS_FREE(lpht->entries[ii].value);
        lpht->entries[ii].key = NULL;
        lpht->entries[ii].value = NULL;
        return UTILS_STATUS_ERROR;
    }

    (void)memcpy(lpht->entries[ii].value, value, lpht->itemSize);

    lpht->items++;

    return UTILS_STATUS_SUCCESS;
}

static utilsStatus_t lpHashTableUpdateEntry(lpHashTable_t* lpht, const char* key, const void* value) {
    /* limit hash to current memory size */
    uint32_t ii = LPHT_HASHFUN(key) & (lpht->size - 1U);
    uint32_t cnt = 0;

    /* Loop to search the entry to be updated */
    while (lpht->entries[ii].key != NULL) {
        if (!strcmp(key, lpht->entries[ii].key)) {
            /* Found entry, now update it */
            (void)memcpy(lpht->entries[ii].value, value, lpht->itemSize);
            return UTILS_STATUS_SUCCESS;
        }
        if (++ii >= lpht->size) {
            ii = 0;
        }
        /* Check to avoid infinite loop in case of full list */
        if (++cnt >= lpht->size) {
            return UTILS_STATUS_FULL;
        }
    }

    return UTILS_STATUS_ERROR;
}

static void lpHashTableReplaceEntries(lpHashTable_t* lpht, uint32_t startIndex) {
    uint32_t idx = startIndex;
    if (++idx >= lpht->size) {
        idx = 0;
    }
    uint32_t cnt = lpht->items;
    while ((lpht->entries[idx].key != NULL) && (cnt > 0U)) {
        cnt--;
        /* limit hash to current memory size */
        uint32_t ii = LPHT_HASHFUN(lpht->entries[idx].key) & (lpht->size - 1U);
        if (idx != ii) {
            while (lpht->entries[ii].key != NULL) {
                if (++ii >= lpht->size) {
                    ii = 0;
                }
            }
            lpht->entries[ii].key = lpht->entries[idx].key;
            lpht->entries[ii].value = lpht->entries[idx].value;
            lpht->entries[idx].key = NULL;
            lpht->entries[idx].value = NULL;
        }
        if (++idx >= lpht->size) {
            idx = 0;
        }
    }
}

static utilsStatus_t lpHashTableXpand(lpHashTable_t* lpht, uint8_t increase) {
    /* Allocate new entries array. */
    uint32_t old_size = lpht->size;

    /* hash is currently a uint32_t so Hash-table size should not exceed that */
    if (increase != 0U) {
        /* increase table size */
        if (lpht->size >= LPHT_MAX_SIZE) {
            return UTILS_STATUS_SUCCESS;
        } else if (lpht->size >= (LPHT_MAX_SIZE >> 1)) {
            lpht->size = LPHT_MAX_SIZE;
        } else {
            lpht->size *= 2;
        }
    } else {
        /* decrease table size */
        if (lpht->size <= (uint32_t)LPHT_MIN_SIZE) {
            return UTILS_STATUS_SUCCESS;
        } else if (lpht->size >= ((uint32_t)LPHT_MIN_SIZE * 2U)) {
            lpht->size = (lpht->size >> 1);
        } else {
            lpht->size = LPHT_MIN_SIZE;
        }
    }

    lpHashTableEntry_t* old_entries = lpht->entries;
    /* cppcheck-suppress misra-c2012-11.5 ; deviation: generic container returns typed pointer from void* storage */
    lpht->entries = ADVUTILS_CALLOC(lpht->size, sizeof(lpHashTableEntry_t));
    ADVUTILS_ASSERT(lpht->entries != NULL);
    if (lpht->entries == NULL) {
        lpht->size = old_size;
        lpht->entries = old_entries;
        return UTILS_STATUS_ERROR;
    }

    /* Iterate entries, move all non-empty ones to new table's entries. */
    lpht->items = 0;

    for (uint32_t i = 0; i < old_size; i++) {
        if (old_entries[i].key != NULL) {
            /* limit hash to current memory size */
            uint32_t j = LPHT_HASHFUN(old_entries[i].key) & (lpht->size - 1U);
            while (lpht->entries[j].key != NULL) {
                if (++j >= lpht->size) {
                    j = 0;
                }
            }
            lpht->entries[j].key = old_entries[i].key;
            lpht->entries[j].value = old_entries[i].value;
            lpht->items++;
        }
    }

    ADVUTILS_FREE(old_entries);

    return UTILS_STATUS_SUCCESS;
}

#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */
