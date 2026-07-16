/* BEGIN Header */
/**
 ******************************************************************************
 * \file            test_list.c
 * \author          Andrea Vivani
 * \brief           Unit tests for list.c
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
#include <string.h>
#include "list.h"

#include <cmocka.h>

/* Support functions ---------------------------------------------------------*/

static uint8_t mallocFail = 0;
static uint8_t callocFail = 0;

void* ADVUtils_testCalloc(const size_t number_of_elements, const size_t size) {
    if ((number_of_elements > 0) && !callocFail) {
        return test_calloc(number_of_elements, size);
    } else {
        return NULL;
    }
}

void* ADVUtils_testMalloc(const size_t size) {
    if ((size > 0) && !mallocFail) {
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

/* Functions -----------------------------------------------------------------*/

static void test_listInit(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    assert_int_equal(list.size, 10);
    assert_int_equal(list.items, 0);
    assert_null(list._front);
    assert_null(list._rear);
}

static void test_listPush(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value = 42;
    assert_int_equal(listPush(&list, &value), UTILS_STATUS_SUCCESS);
    assert_int_equal(list.items, 1);
    assert_non_null(list._front);
    assert_non_null(list._rear);
    int out_value;
    memcpy(&out_value, list._front->data, sizeof(int));
    assert_int_equal(out_value, value);
    listFlush(&list);
}

static void test_listPushNullGuards(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value = 42;
    assert_int_equal(listPush(NULL, &value), UTILS_STATUS_ERROR);
    assert_int_equal(listPush(&list, NULL), UTILS_STATUS_ERROR);
    assert_int_equal(listPush(NULL, NULL), UTILS_STATUS_ERROR);
    listFlush(&list);
}

static void test_listPushMemoryFail(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value = 42;

    skipAssert = 1;
    mallocFail = 1;
    assert_int_equal(listPush(&list, &value), UTILS_STATUS_ERROR);
    assert_int_equal(list.items, 0);
    assert_null(list._front);
    assert_null(list._rear);
    mallocFail = 0;

    callocFail = 1;
    assert_int_equal(listPush(&list, &value), UTILS_STATUS_ERROR);
    assert_int_equal(list.items, 0);
    assert_null(list._front);
    assert_null(list._rear);
    callocFail = 0;
    skipAssert = 0;

    listFlush(&list);
}

static void test_listPushFull(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 2);
    int value = 42;
    listPush(&list, &value);
    listPush(&list, &value);
    assert_int_equal(listPush(&list, &value), UTILS_STATUS_FULL);
    listFlush(&list);
}

static void test_listPushFront(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value1 = 42;
    int value2 = 43;
    listPushFront(&list, &value1);
    listPushFront(&list, &value2);
    assert_int_equal(list.items, 2);
    int out_value;
    memcpy(&out_value, list._front->data, sizeof(int));
    assert_int_equal(out_value, value2);
    listFlush(&list);
}

static void test_listPushFrontNullGuards(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value = 42;
    assert_int_equal(listPushFront(NULL, &value), UTILS_STATUS_ERROR);
    assert_int_equal(listPushFront(&list, NULL), UTILS_STATUS_ERROR);
    assert_int_equal(listPushFront(NULL, NULL), UTILS_STATUS_ERROR);
    listFlush(&list);
}

static void test_listPushFrontMemoryFail(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value = 42;

    skipAssert = 1;
    mallocFail = 1;
    assert_int_equal(listPushFront(&list, &value), UTILS_STATUS_ERROR);
    assert_int_equal(list.items, 0);
    assert_null(list._front);
    assert_null(list._rear);
    mallocFail = 0;

    callocFail = 1;
    assert_int_equal(listPushFront(&list, &value), UTILS_STATUS_ERROR);
    assert_int_equal(list.items, 0);
    assert_null(list._front);
    assert_null(list._rear);
    callocFail = 0;
    skipAssert = 0;

    listFlush(&list);
}

static void test_listPushFrontFull(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 2);
    int value = 42;
    listPush(&list, &value);
    listPush(&list, &value);
    assert_int_equal(listPushFront(&list, &value), UTILS_STATUS_FULL);
    listFlush(&list);
}

static void test_listInsert(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 5);
    int value1 = 42;
    int value2 = 43;
    int value3 = 44;
    int value4 = 45;
    int value5 = 46;
    assert_int_equal(listInsert(&list, &value1, 0), UTILS_STATUS_SUCCESS);
    assert_int_equal(listInsert(&list, &value2, 0), UTILS_STATUS_SUCCESS);
    assert_int_equal(listInsert(&list, &value3, 2), UTILS_STATUS_SUCCESS);
    listPush(&list, &value4);
    assert_int_equal(listInsert(&list, &value5, 6), UTILS_STATUS_ERROR);
    assert_int_equal(listInsert(&list, &value5, 2), UTILS_STATUS_SUCCESS);
    assert_int_equal(list.items, 5);
    assert_int_equal(listInsert(&list, &value5, 2), UTILS_STATUS_FULL);
    int out_value;
    memcpy(&out_value, list._front->data, sizeof(int));
    assert_int_equal(out_value, value2);
    memcpy(&out_value, list._front->next->data, sizeof(int));
    assert_int_equal(out_value, value1);
    memcpy(&out_value, list._front->next->next->data, sizeof(int));
    assert_int_equal(out_value, value5);
    memcpy(&out_value, list._front->next->next->next->data, sizeof(int));
    assert_int_equal(out_value, value3);
    memcpy(&out_value, list._front->next->next->next->next->data, sizeof(int));
    assert_int_equal(out_value, value4);
    listFlush(&list);
}

static void test_listInsertNullGuards(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value = 42;
    assert_int_equal(listInsert(NULL, &value, 0), UTILS_STATUS_ERROR);
    assert_int_equal(listInsert(&list, NULL, 0), UTILS_STATUS_ERROR);
    assert_int_equal(listInsert(NULL, NULL, 0), UTILS_STATUS_ERROR);
    listFlush(&list);
}

static void test_listInsertMemoryFail(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value = 42;

    skipAssert = 1;
    mallocFail = 1;
    assert_int_equal(listInsert(&list, &value, 0), UTILS_STATUS_ERROR);
    assert_int_equal(list.items, 0);
    assert_null(list._front);
    assert_null(list._rear);
    mallocFail = 0;

    callocFail = 1;
    assert_int_equal(listInsert(&list, &value, 0), UTILS_STATUS_ERROR);
    assert_int_equal(list.items, 0);
    assert_null(list._front);
    assert_null(list._rear);
    callocFail = 0;
    skipAssert = 0;

    listFlush(&list);
}

static void test_listUpdate(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value1 = 42;
    int value2 = 43;
    int out_value;
    listPush(&list, &value1);
    listPush(&list, &value1);
    listPush(&list, &value1);
    assert_int_equal(listUpdate(&list, &value2, 0), UTILS_STATUS_SUCCESS);
    memcpy(&out_value, list._front->data, sizeof(int));
    assert_int_equal(out_value, value2);
    assert_int_equal(listUpdate(&list, &value2, 1), UTILS_STATUS_SUCCESS);
    memcpy(&out_value, list._front->next->data, sizeof(int));
    assert_int_equal(out_value, value2);
    assert_int_equal(listUpdate(&list, &value2, 2), UTILS_STATUS_SUCCESS);
    memcpy(&out_value, list._front->next->next->data, sizeof(int));
    assert_int_equal(out_value, value2);
    assert_int_equal(listUpdate(&list, &value2, 4), UTILS_STATUS_ERROR);
    assert_int_equal(list.items, 3);
    listFlush(&list);
}

static void test_listPop(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value = 42;
    listPush(&list, &value);
    int out_value;
    assert_int_equal(listPop(&list, &out_value), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value);
    assert_int_equal(list.items, 0);
    assert_null(list._front);
    assert_null(list._rear);
}

static void test_listPopEmpty(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int out_value;
    assert_int_equal(listPop(&list, &out_value), UTILS_STATUS_EMPTY);
}

static void test_listPopBack(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value1 = 42;
    int value2 = 43;
    int value3 = 44;
    int value4 = 45;
    int value5 = 46;
    listPush(&list, &value1);
    listPush(&list, &value2);
    listPush(&list, &value3);
    listPush(&list, &value4);
    listPush(&list, &value5);
    int out_value;
    assert_int_equal(listPopBack(&list, &out_value), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value5);
    assert_int_equal(list.items, 4);
    assert_int_equal(listPopBack(&list, &out_value), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value4);
    assert_int_equal(list.items, 3);
    assert_int_equal(listPopBack(&list, &out_value), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value3);
    assert_int_equal(list.items, 2);
    assert_int_equal(listPopBack(&list, &out_value), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value2);
    assert_int_equal(list.items, 1);
    assert_int_equal(listPopBack(&list, &out_value), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value1);
    assert_int_equal(list.items, 0);
    listFlush(&list);
}

static void test_listPopBackEmpty(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int out_value;
    assert_int_equal(listPopBack(&list, &out_value), UTILS_STATUS_EMPTY);
}

static void test_listRemove(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value1 = 42;
    int value2 = 43;
    int value3 = 44;
    int value4 = 45;
    int value5 = 46;
    int out_value;
    assert_int_equal(listRemove(&list, &out_value, 0), UTILS_STATUS_EMPTY);
    listPush(&list, &value1);
    listPush(&list, &value2);
    listPush(&list, &value3);
    listPush(&list, &value4);
    listPush(&list, &value5);
    assert_int_equal(listRemove(&list, &out_value, 0), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value1);
    assert_int_equal(list.items, 4);
    assert_int_equal(listRemove(&list, &out_value, 3), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value5);
    assert_int_equal(list.items, 3);
    assert_int_equal(listRemove(&list, &out_value, 1), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value3);
    assert_int_equal(list.items, 2);
    assert_int_equal(listRemove(&list, &out_value, 4), UTILS_STATUS_ERROR);
    assert_int_equal(listRemove(&list, &out_value, 1), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value4);
    assert_int_equal(list.items, 1);
    assert_int_equal(listRemove(&list, &out_value, 0), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value2);
    assert_int_equal(list.items, 0);
    listFlush(&list);
}

static void test_listPeek(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value = 42;
    int out_value;
    assert_int_equal(listPeek(&list, &out_value), UTILS_STATUS_EMPTY);
    listPush(&list, &value);
    assert_int_equal(listPeek(&list, &out_value), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value);
    assert_int_equal(list.items, 1);
    listFlush(&list);
}

static void test_listPeekBack(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value1 = 42;
    int value2 = 43;
    int out_value;
    assert_int_equal(listPeekBack(&list, &out_value), UTILS_STATUS_EMPTY);
    listPush(&list, &value1);
    listPush(&list, &value2);
    assert_int_equal(listPeekBack(&list, &out_value), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value2);
    assert_int_equal(list.items, 2);
    listFlush(&list);
}

static void test_listPeekAtPos(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value1 = 42;
    int value2 = 43;
    int value3 = 44;
    int out_value;
    assert_int_equal(listPeekAtPos(&list, &out_value, 0), UTILS_STATUS_EMPTY);
    listPush(&list, &value1);
    listPush(&list, &value2);
    listPush(&list, &value3);
    assert_int_equal(listPeekAtPos(&list, &out_value, 4), UTILS_STATUS_ERROR);
    assert_int_equal(listPeekAtPos(&list, &out_value, 0), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value1);
    assert_int_equal(listPeekAtPos(&list, &out_value, 1), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value2);
    assert_int_equal(listPeekAtPos(&list, &out_value, 2), UTILS_STATUS_SUCCESS);
    assert_int_equal(out_value, value3);
    listFlush(&list);
}

static void test_listInfo(void** state) {
    (void)state; /* unused */
    list_t list;
    uint16_t items = 0, size = 0;
    listInit(&list, sizeof(int), 10);
    int values[] = {1, 2, 3};
    for (int i = 0; i < 3; ++i) {
        listPush(&list, &values[i]);
    }
    listInfo(&list, &size, NULL);
    assert_int_equal(size, 10);
    size = 0;
    listInfo(&list, NULL, &items);
    assert_int_equal(items, 3);
    items = 0;
    listInfo(&list, &size, &items);
    assert_int_equal(size, 10);
    assert_int_equal(items, 3);
    listFlush(&list);
}

static void test_listFlush(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int value = 42;
    listPush(&list, &value);
    assert_int_equal(listFlush(&list), UTILS_STATUS_SUCCESS);
    assert_int_equal(list.items, 0);
    assert_null(list._front);
    assert_null(list._rear);
}

static void test_listIterator(void** state) {
    (void)state; /* unused */
    list_t list;
    listInit(&list, sizeof(int), 10);
    int values[] = {1, 2, 3};
    for (int i = 0; i < 3; ++i) {
        listPush(&list, &values[i]);
    }
    listIterator_t it;
    listIt(&it, &list);
    int value;
    assert_int_equal(listItNext(&it), UTILS_STATUS_SUCCESS);
    memcpy(&value, it.ptr->data, sizeof(int));
    assert_int_equal(value, 1);
    assert_int_equal(listItNext(&it), UTILS_STATUS_SUCCESS);
    memcpy(&value, it.ptr->data, sizeof(int));
    assert_int_equal(value, 2);
    assert_int_equal(listItNext(&it), UTILS_STATUS_SUCCESS);
    memcpy(&value, it.ptr->data, sizeof(int));
    assert_int_equal(value, 3);
    assert_int_equal(listItNext(&it), UTILS_STATUS_ERROR); // No more items
    listFlush(&list);
}

int main(void) {
    const struct CMUnitTest test_list[] = {
        cmocka_unit_test(test_listInit),
        cmocka_unit_test(test_listPush),
        cmocka_unit_test(test_listPushFull),
        cmocka_unit_test(test_listPushFront),
        cmocka_unit_test(test_listPushFrontFull),
        cmocka_unit_test(test_listInsert),
        cmocka_unit_test(test_listUpdate),
        cmocka_unit_test(test_listPop),
        cmocka_unit_test(test_listPopEmpty),
        cmocka_unit_test(test_listPopBack),
        cmocka_unit_test(test_listPopBackEmpty),
        cmocka_unit_test(test_listRemove),
        cmocka_unit_test(test_listPeek),
        cmocka_unit_test(test_listPeekBack),
        cmocka_unit_test(test_listPeekAtPos),
        cmocka_unit_test(test_listInfo),
        cmocka_unit_test(test_listFlush),
        cmocka_unit_test(test_listIterator),
        cmocka_unit_test(test_listPushNullGuards),
        cmocka_unit_test(test_listPushMemoryFail),
        cmocka_unit_test(test_listPushFrontNullGuards),
        cmocka_unit_test(test_listPushFrontMemoryFail),
        cmocka_unit_test(test_listInsertNullGuards),
        cmocka_unit_test(test_listInsertMemoryFail),
    };

    return cmocka_run_group_tests(test_list, NULL, NULL);
}
