/* BEGIN Header */
/**
 ******************************************************************************
 * \file            test_LPHashTable.c
 * \author          Andrea Vivani
 * \brief           Unit tests for LPHashTable.c
 ******************************************************************************
 * \copyright
 *
 * Copyright 2024 Andrea Vivani
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

/* Includes ------------------------------------------------------------------*/

#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "LPHashTable.h"
#include "hashFunctions.h"

#include <cmocka.h>

/* Support functions ---------------------------------------------------------*/

static uint8_t callocFail = 0;

void* ADVUtils_testCalloc(const size_t number_of_elements, const size_t size) {
    if ((number_of_elements > 0) && !callocFail) {
        return test_calloc(number_of_elements, size);
    } else {
        return NULL;
    }
}

static uint8_t mallocFail = 0;

void* ADVUtils_testMalloc(const size_t size) {
    if (!mallocFail) {
        return test_malloc(size);
    } else {
        return NULL;
    }
}

static uint8_t skipAssert = 0;

void ADVUtils_testAssert(const int result, const char* const expression, const char* const file, const int line) {
    if (skipAssert) {
        return;
    } else {
        mock_assert(result, expression, file, line);
    }
}

static uint16_t mockHash = 0;

uint32_t ADVUtils_testHash(const char* key) {
    if (!mockHash) {
        return hash_FNV1A(key);
    } else {
        return mockHash;
    }
}

/* Functions -----------------------------------------------------------------*/

static void test_lpHashTableInit(void** state) {
    (void)state; /* unused */
    lpHashTable_t lpht;
    assert_int_equal(lpHashTableInit(&lpht, sizeof(int), 10, LPHT_RESIZABLE), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpht.size, 10);
    assert_int_equal(lpht.items, 0);
    assert_non_null(lpht.entries);
    test_free(lpht.entries);
    /* Check null initialization */
    skipAssert = 0;
    expect_assert_failure(lpHashTableInit(&lpht, 0, 0, LPHT_RESIZABLE));
    skipAssert = 1;
    assert_int_equal(lpHashTableInit(&lpht, 0, 0, LPHT_RESIZABLE), UTILS_STATUS_ERROR);
    skipAssert = 0;
}

static void test_lpHashTablePutAndGetResizeable(void** state) {
    (void)state; /* unused */
    lpHashTable_t lpht;
    int value1 = 42;
    int value2 = 43;
    int out_value;
    /* Test of resizable hash-table */
    assert_int_equal(lpHashTableInit(&lpht, sizeof(int), 3, LPHT_RESIZABLE), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTablePut(&lpht, NULL, &value1), UTILS_STATUS_ERROR);
    assert_int_equal(lpHashTablePut(&lpht, "key1", NULL), UTILS_STATUS_ERROR);
    assert_int_equal(lpHashTablePut(&lpht, "key1", &value1), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTablePut(&lpht, "key2", &value2), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpht.size, 3);
    /* Test that update has priority over resize */
    assert_int_equal(lpHashTablePut(&lpht, "key2", &value1), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpht.items, 2);
    assert_int_equal(lpht.size, 3);
    /* Test resize failure */
    callocFail = 1;
    skipAssert = 1;
    assert_int_equal(lpHashTablePut(&lpht, "key3", &value2), UTILS_STATUS_ERROR);
    callocFail = 0;
    skipAssert = 0;
    assert_int_equal(lpHashTablePut(&lpht, "key3", &value2), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpht.items, 3);
    assert_int_equal(lpht.size, 6);
    assert_int_equal(lpHashTableGet(&lpht, "key4", &out_value, LPHT_DO_NOT_REMOVE_ITEM), UTILS_STATUS_ERROR);
    assert_int_equal(lpHashTableGet(&lpht, "key1", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value1);
    assert_int_equal(lpHashTableGet(&lpht, "key1", &out_value, LPHT_DO_NOT_REMOVE_ITEM), UTILS_STATUS_ERROR);
    assert_int_equal(lpHashTableGet(&lpht, "key2", &out_value, LPHT_DO_NOT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value1);
    assert_int_equal(lpHashTableGet(&lpht, "key4", &out_value, LPHT_DO_NOT_REMOVE_ITEM), UTILS_STATUS_ERROR);
    /* Test down-size failure */
    callocFail = 1;
    skipAssert = 1;
    assert_int_equal(lpHashTableGet(&lpht, "key2", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_WARNING);
    assert_int_equal(out_value, value1);
    assert_int_equal(lpht.items, 1);
    assert_int_equal(lpht.size, 6);
    callocFail = 0;
    skipAssert = 0;
    /* Check resize to min-size */
    assert_int_equal(lpHashTablePut(&lpht, "key2", &value2), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTableGet(&lpht, "key2", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpht.items, 1);
    assert_int_equal(lpht.size, 3);
    assert_int_equal(lpHashTableGet(&lpht, "key3", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value2);
    assert_int_equal(lpht.items, 0);
    assert_int_equal(lpht.size, LPHT_MIN_SIZE);
    assert_int_equal(lpHashTablePut(&lpht, "key1", &value1), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpht.items, 1);
    assert_int_equal(lpht.size, LPHT_MIN_SIZE);
    assert_int_equal(lpHashTableGet(&lpht, "key1", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value1);
    assert_int_equal(lpht.items, 0);
    assert_int_equal(lpht.size, LPHT_MIN_SIZE);
    /* Check resize with wrap-around */
    assert_int_equal(lpHashTablePut(&lpht, "key1", &value1), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTablePut(&lpht, "key2", &value1), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpht.items, 2);
    assert_int_equal(lpht.size, LPHT_MIN_SIZE << 1);
    mockHash = 7;
    assert_int_equal(lpHashTablePut(&lpht, "key3", &value1), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTableGet(&lpht, "key1", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value1);
    assert_int_equal(lpHashTableGet(&lpht, "key2", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value1);
    assert_int_equal(lpHashTableGet(&lpht, "key3", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value1);
    mockHash = 0;
    assert_int_equal(lpht.items, 0);
    test_free(lpht.entries);
    /* Check resize to full-size */
    assert_int_equal(lpHashTableInit(&lpht, sizeof(int), LPHT_MAX_SIZE >> 1, LPHT_RESIZABLE), UTILS_STATUS_SUCCESS);
    for (uint8_t ii = 0; ii < ((LPHT_MAX_SIZE >> 1) * LPHT_MAX_SATURATION); ii++) {
        char key[6];
        snprintf(key, 6, "key1%d", ii);
        lpHashTablePut(&lpht, key, &value1);
    }
    assert_int_equal(lpht.items, (LPHT_MAX_SIZE >> 1) * LPHT_MAX_SATURATION);
    assert_int_equal(lpHashTablePut(&lpht, "key1", &value1), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpht.items, (LPHT_MAX_SIZE >> 1) * LPHT_MAX_SATURATION + 1);
    assert_int_equal(lpht.size, LPHT_MAX_SIZE);
    uint32_t items = lpht.items;
    for (uint8_t ii = 0; ii < (LPHT_MAX_SIZE - items); ii++) {
        char key[7];
        snprintf(key, 7, "key2%d", ii);
        lpHashTablePut(&lpht, key, &value1);
    }
    assert_int_equal(lpht.items, LPHT_MAX_SIZE);
    assert_int_equal(lpHashTablePut(&lpht, "key2", &value1), UTILS_STATUS_FULL);
    assert_int_equal(lpht.size, LPHT_MAX_SIZE);
    lpHashTableDelete(&lpht);
}

static void test_lpHashTablePutAndGetNotResizeable(void** state) {
    (void)state; /* unused */
    lpHashTable_t lpht;
    int value1 = 42;
    int value2 = 43;
    int out_value;
    assert_int_equal(lpHashTableInit(&lpht, sizeof(int), 4, LPHT_NOT_RESIZABLE), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTablePut(&lpht, "key1", &value1), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTablePut(&lpht, "key2", &value2), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpht.items, 2);
    assert_int_equal(lpht.size, 4);
    assert_int_equal(lpHashTablePut(&lpht, "key3", &value2), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTablePut(&lpht, "key4", &value2), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpht.items, 4);
    assert_int_equal(lpHashTablePut(&lpht, "key5", &value2), UTILS_STATUS_FULL);
    assert_int_equal(lpHashTableGet(&lpht, "key5", &out_value, LPHT_DO_NOT_REMOVE_ITEM), UTILS_STATUS_ERROR);
    assert_int_equal(lpHashTableGet(&lpht, "key1", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value1);
    assert_int_equal(lpht.items, 3);
    assert_int_equal(lpHashTableGet(&lpht, "key2", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value2);
    assert_int_equal(lpht.items, 2);
    assert_int_equal(lpHashTableGet(&lpht, "key3", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value2);
    assert_int_equal(lpht.items, 1);
    assert_int_equal(lpHashTableGet(&lpht, "key4", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value2);
    assert_int_equal(lpht.items, 0);
    assert_int_equal(lpHashTableGet(&lpht, "key2", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_EMPTY);
    assert_int_equal(lpHashTablePut(&lpht, "key2", &value2), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTablePut(&lpht, "key2", &value1), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTableGet(&lpht, "key2", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value1);
    assert_int_equal(lpHashTablePut(&lpht, "key4", &value1), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTableGet(&lpht, "key4", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value1);
    assert_int_equal(lpht.items, 0);
    /* Test of search algorithm wrap-around */
    mockHash = 2;
    assert_int_equal(lpHashTablePut(&lpht, "key1", &value1), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTablePut(&lpht, "key2", &value2), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTablePut(&lpht, "key3", &value2), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTablePut(&lpht, "key4", &value2), UTILS_STATUS_SUCCESS);
    assert_string_equal(lpht.entries[2].key, "key1");
    assert_string_equal(lpht.entries[3].key, "key2");
    assert_string_equal(lpht.entries[0].key, "key3");
    assert_string_equal(lpht.entries[1].key, "key4");
    /* Check that setEntry works in case of same hash when the first item with the same hash has been removed */
    assert_int_equal(lpHashTableGet(&lpht, "key1", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value1);
    assert_int_equal(lpHashTablePut(&lpht, "key2", &value1), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpht.items, 3);
    assert_string_equal(lpht.entries[0].key, "key4");
    assert_null(lpht.entries[1].key);
    assert_string_equal(lpht.entries[2].key, "key2");
    assert_string_equal(lpht.entries[3].key, "key3");
    assert_int_equal(lpHashTableGet(&lpht, "key2", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value1);
    assert_int_equal(lpHashTableGet(&lpht, "key3", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value2);
    assert_int_equal(lpHashTableGet(&lpht, "key4", &out_value, LPHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value2);
    mockHash = 0;
    assert_int_equal(lpht.items, 0);
    /* Check malloc / calloc fail  */
    callocFail = 0;
    skipAssert = 1;
    mallocFail = 1;
    assert_int_equal(lpHashTablePut(&lpht, "key2", &value1), UTILS_STATUS_ERROR);
    mallocFail = 0;
    callocFail = 1;
    assert_int_equal(lpHashTablePut(&lpht, "key2", &value1), UTILS_STATUS_ERROR);
    callocFail = 0;
    skipAssert = 0;
    test_free(lpht.entries);
}

static void test_lpHashTableInfo(void** state) {
    (void)state; /* unused */
    lpHashTable_t lpht;
    assert_int_equal(lpHashTableInit(&lpht, sizeof(int), 10, LPHT_RESIZABLE), UTILS_STATUS_SUCCESS);
    uint32_t size, items;
    lpHashTableInfo(&lpht, &size, &items);
    assert_int_equal(size, 10);
    assert_int_equal(items, 0);
    int value = 42;
    lpHashTablePut(&lpht, "key1", &value);
    lpHashTableInfo(&lpht, &size, &items);
    assert_int_equal(items, 1);
    lpHashTableDelete(&lpht);
}

static void test_lpHashTableFlush(void** state) {
    (void)state; /* unused */
    lpHashTable_t lpht;
    assert_int_equal(lpHashTableInit(&lpht, sizeof(int), 10, LPHT_RESIZABLE), UTILS_STATUS_SUCCESS);
    int value1 = 42, value2 = 43;
    assert_int_equal(lpHashTablePut(&lpht, "key1", &value1), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTablePut(&lpht, "key2", &value2), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpht.items, 2);
    assert_int_equal(lpHashTableFlush(&lpht), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpht.items, 0);
    for (uint32_t ii = 0; ii < lpht.size; ii++) {
        assert_null(lpht.entries[ii].key);
        assert_null(lpht.entries[ii].value);
    }
    test_free(lpht.entries);
}

static void test_lpHashTableDelete(void** state) {
    (void)state; /* unused */
    lpHashTable_t lpht;
    assert_int_equal(lpHashTableInit(&lpht, sizeof(int), 10, LPHT_RESIZABLE), UTILS_STATUS_SUCCESS);
    assert_int_equal(lpHashTableDelete(&lpht), UTILS_STATUS_SUCCESS);
    /* Check null deletion */
    lpht.entries = NULL;
    assert_int_equal(lpHashTableDelete(&lpht), UTILS_STATUS_ERROR);
}

int main(void) {
    const struct CMUnitTest test_LPHashTable[] = {
        cmocka_unit_test(test_lpHashTableInit),
        cmocka_unit_test(test_lpHashTablePutAndGetResizeable),
        cmocka_unit_test(test_lpHashTablePutAndGetNotResizeable),
        cmocka_unit_test(test_lpHashTableInfo),
        cmocka_unit_test(test_lpHashTableFlush),
        cmocka_unit_test(test_lpHashTableDelete),
    };

    return cmocka_run_group_tests(test_LPHashTable, NULL, NULL);
}
