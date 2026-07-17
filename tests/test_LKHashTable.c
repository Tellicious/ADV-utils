/* BEGIN Header */
/**
 ******************************************************************************
 * \file            test_LKHashTable.c
 * \author          Andrea Vivani
 * \brief           Unit tests for LKHashTable.c
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
#include "LKHashTable.h"
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

static void test_lkHashTableInit(void** state) {
    (void)state; /* unused */
    lkHashTable_t lkht;
    assert_int_equal(lkHashTableInit(&lkht, sizeof(int), 10), UTILS_STATUS_SUCCESS);
    assert_int_equal(lkht.size, 10);
    assert_int_equal(lkht.items, 0);
    assert_non_null(lkht.entries);
    for (uint32_t i = 0; i < lkht.size; i++) {
        assert_int_equal(lkht.entries[i].size, LKHT_LIST_SIZE);
        assert_int_equal(lkht.entries[i].items, 0);
    }
    test_free(lkht.entries);
    /* Check null initialization */
    skipAssert = 0;
    expect_assert_failure(lkHashTableInit(&lkht, 0, 0));
    skipAssert = 1;
    assert_int_equal(lkHashTableInit(&lkht, 0, 0), UTILS_STATUS_ERROR);
    skipAssert = 0;
}

static void test_lkHashTablePutAndGet(void** state) {
    (void)state; /* unused */
    lkHashTable_t lkht;
    assert_int_equal(lkHashTableInit(&lkht, sizeof(int), 3), UTILS_STATUS_SUCCESS);
    int value1 = 42;
    int value2 = 43;
    int value3 = 44;
    int out_value;
    assert_int_equal(lkHashTablePut(&lkht, NULL, &value1), UTILS_STATUS_ERROR);
    assert_int_equal(lkHashTablePut(&lkht, "key1", NULL), UTILS_STATUS_ERROR);
    assert_int_equal(lkHashTablePut(&lkht, "key1", &value1), UTILS_STATUS_SUCCESS);
    assert_int_equal(lkHashTablePut(&lkht, "key2", &value2), UTILS_STATUS_SUCCESS);
    assert_int_equal(lkHashTablePut(&lkht, "key3", &value3), UTILS_STATUS_SUCCESS);
    assert_int_equal(lkHashTablePut(&lkht, "key4", &value3), UTILS_STATUS_FULL);
    assert_int_equal(lkht.items, 3);
    assert_int_equal(lkHashTableGet(&lkht, "key1", &out_value, LKHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value1);
    assert_int_equal(lkht.items, 2);
    assert_int_equal(lkHashTableGet(&lkht, "key2", &out_value, LKHT_DO_NOT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value2);
    assert_int_equal(lkht.items, 2);
    assert_int_equal(lkHashTablePut(&lkht, "key2", &value3), UTILS_STATUS_SUCCESS);
    assert_int_equal(lkht.items, 2);
    assert_int_equal(lkHashTableGet(&lkht, "key2", &out_value, LKHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value3);
    assert_int_equal(lkht.items, 1);
    assert_int_equal(lkHashTableGet(&lkht, "key2", &out_value, LKHT_DO_NOT_REMOVE_ITEM), UTILS_STATUS_BUCKET_EMPTY);
    assert_int_equal(lkHashTableGet(&lkht, "key4", &out_value, LKHT_DO_NOT_REMOVE_ITEM), UTILS_STATUS_ERROR);
    assert_int_equal(lkHashTableGet(&lkht, "key3", &out_value, LKHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value3);
    assert_int_equal(lkht.items, 0);
    assert_int_equal(lkHashTableGet(&lkht, "key3", &out_value, LKHT_REMOVE_ITEM), UTILS_STATUS_EMPTY);
    /* Check malloc / calloc fail  */
    callocFail = 0;
    skipAssert = 1;
    mallocFail = 1;
    assert_int_equal(lkHashTablePut(&lkht, "key2", &value3), UTILS_STATUS_ERROR);
    mallocFail = 0;
    callocFail = 1;
    assert_int_equal(lkHashTablePut(&lkht, "key2", &value3), UTILS_STATUS_ERROR);
    callocFail = 0;
    skipAssert = 0;
    /* Check listPush error */
    mockHash = 1;
    assert_int_equal(lkHashTablePut(&lkht, "key2", &value3), UTILS_STATUS_SUCCESS);
    assert_int_equal(lkHashTablePut(&lkht, "key3", &value3), UTILS_STATUS_SUCCESS);
    assert_int_equal(lkHashTablePut(&lkht, "key4", &value3), UTILS_STATUS_ERROR);
    assert_int_equal(lkHashTableGet(&lkht, "key2", &out_value, LKHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    assert_int_equal(lkHashTableGet(&lkht, "key3", &out_value, LKHT_REMOVE_ITEM), UTILS_STATUS_SUCCESS);
    mockHash = 0;
    test_free(lkht.entries);
}

static void test_lkHashTableInfo(void** state) {
    (void)state; /* unused */
    lkHashTable_t lkht;
    assert_int_equal(lkHashTableInit(&lkht, sizeof(int), 10), UTILS_STATUS_SUCCESS);
    uint32_t size, items;
    lkHashTableInfo(&lkht, &size, &items);
    assert_int_equal(size, 10);
    assert_int_equal(items, 0);
    int value = 42;
    lkHashTablePut(&lkht, "key1", &value);
    lkHashTableInfo(&lkht, &size, &items);
    assert_int_equal(items, 1);
    lkHashTableDelete(&lkht);
}

static void test_lkHashTableFlush(void** state) {
    (void)state; /* unused */
    lkHashTable_t lkht;
    assert_int_equal(lkHashTableInit(&lkht, sizeof(int), 10), UTILS_STATUS_SUCCESS);
    int value1 = 42;
    int value2 = 43;
    assert_int_equal(lkHashTablePut(&lkht, "key1", &value1), UTILS_STATUS_SUCCESS);
    assert_int_equal(lkHashTablePut(&lkht, "key2", &value2), UTILS_STATUS_SUCCESS);
    assert_int_equal(lkht.items, 2);
    assert_int_equal(lkHashTableFlush(&lkht), UTILS_STATUS_SUCCESS);
    assert_int_equal(lkht.items, 0);
    for (uint32_t i = 0; i < lkht.size; i++) {
        assert_int_equal(lkht.entries[i].items, 0);
    }
    test_free(lkht.entries);
}

static void test_lkHashTableDelete(void** state) {
    (void)state; /* unused */
    lkHashTable_t lkht;
    assert_int_equal(lkHashTableInit(&lkht, sizeof(int), 10), UTILS_STATUS_SUCCESS);
    assert_int_equal(lkHashTableDelete(&lkht), UTILS_STATUS_SUCCESS);
    /* Check null deletion */
    lkht.entries = NULL;
    assert_int_equal(lkHashTableDelete(&lkht), UTILS_STATUS_ERROR);
}

int main(void) {
    const struct CMUnitTest test_LKHashTable[] = {cmocka_unit_test(test_lkHashTableInit), cmocka_unit_test(test_lkHashTablePutAndGet),
                                                  cmocka_unit_test(test_lkHashTableInfo), cmocka_unit_test(test_lkHashTableFlush),
                                                  cmocka_unit_test(test_lkHashTableDelete)};

    return cmocka_run_group_tests(test_LKHashTable, NULL, NULL);
}
