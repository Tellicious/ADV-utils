/* BEGIN Header */
/**
 ******************************************************************************
 * \file            test_event.c
 * \author          Andrea Vivani
 * \brief           Unit tests for event.c
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
#include "event.h"

#include <cmocka.h>

/* Support functions ---------------------------------------------------------*/

void* ADVUtils_testCalloc(const size_t number_of_elements, const size_t size) {
    if (number_of_elements > 0) {
        return test_calloc(number_of_elements, size);
    } else {
        return NULL;
    }
}

void* ADVUtils_testMalloc(const size_t size) {
    if (size > 0) {
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

static int basic_event_triggered = 0;
static int extended_event_triggered = 0;
static void* extended_event_value = NULL;

void basic_event_handler(void) { basic_event_triggered++; }

void extended_event_handler(void* val) {
    extended_event_triggered++;
    extended_event_value = val;
}

static void test_eventInit(void** state) {
    (void)state; /* unused */
    event_t event;
    assert_int_equal(eventInit(&event, EVENT_TYPE_BASIC, 10), UTILS_STATUS_SUCCESS);
    assert_int_equal(event.type, EVENT_TYPE_BASIC);
    assert_int_equal(event.count, 0);
    assert_int_equal(event.size, 10);
    assert_non_null(event.eventsList);
    eventDelete(&event);
    /* Check extended event initialization */
    assert_int_equal(eventInit(&event, EVENT_TYPE_EXTENDED, 7), UTILS_STATUS_SUCCESS);
    assert_int_equal(event.type, EVENT_TYPE_EXTENDED);
    assert_int_equal(event.count, 0);
    assert_int_equal(event.size, 7);
    assert_non_null(event.eventsList);
    eventDelete(&event);
    /* Check null initialization */
    skipAssert = 0;
    expect_assert_failure(eventInit(&event, EVENT_TYPE_EXTENDED, 0));
    skipAssert = 1;
    assert_int_equal(eventInit(&event, EVENT_TYPE_EXTENDED, 0), UTILS_STATUS_ERROR);
    skipAssert = 0;
}

static void test_eventInitStatic(void** state) {
    (void)state; /* unused */
    event_t event;
    eventExtCBType_t buffer[10];
    eventInitStatic(&event, buffer, EVENT_TYPE_BASIC, 10);
    assert_int_equal(event.type, EVENT_TYPE_BASIC);
    assert_int_equal(event.count, 0);
    assert_int_equal(event.size, 10);
    assert_ptr_equal(event.eventsList, buffer);
}

static void test_eventRegister(void** state) {
    (void)state; /* unused */
    event_t event, eventEx;
    assert_int_equal(eventInit(&event, EVENT_TYPE_BASIC, 2), UTILS_STATUS_SUCCESS);
    assert_int_equal(eventInit(&eventEx, EVENT_TYPE_EXTENDED, 2), UTILS_STATUS_SUCCESS);
    assert_int_equal(eventRegister(&event, basic_event_handler), UTILS_STATUS_SUCCESS);
    assert_int_equal(event.count, 1);
    /* Check registration of wrong event type */
    assert_int_equal(eventRegister(&eventEx, basic_event_handler), UTILS_STATUS_ERROR);
    /* Check registration when full */
    assert_int_equal(eventRegister(&event, basic_event_handler), UTILS_STATUS_SUCCESS);
    assert_int_equal(eventRegister(&event, basic_event_handler), UTILS_STATUS_FULL);
    eventDelete(&event);
    eventDelete(&eventEx);
}

static void test_eventRegisterExt(void** state) {
    (void)state; /* unused */
    event_t event, eventEx;
    assert_int_equal(eventInit(&event, EVENT_TYPE_BASIC, 2), UTILS_STATUS_SUCCESS);
    assert_int_equal(eventInit(&eventEx, EVENT_TYPE_EXTENDED, 2), UTILS_STATUS_SUCCESS);
    assert_int_equal(eventRegisterExt(&eventEx, extended_event_handler), UTILS_STATUS_SUCCESS);
    assert_int_equal(eventEx.count, 1);
    /* Check registration of wrong event type */
    assert_int_equal(eventRegisterExt(&event, extended_event_handler), UTILS_STATUS_ERROR);
    /* Check registration when full */
    assert_int_equal(eventRegisterExt(&eventEx, extended_event_handler), UTILS_STATUS_SUCCESS);
    assert_int_equal(eventRegisterExt(&eventEx, extended_event_handler), UTILS_STATUS_FULL);
    eventDelete(&event);
    eventDelete(&eventEx);
}

static void test_eventDispatch(void** state) {
    (void)state; /* unused */
    event_t event, eventEx;
    assert_int_equal(eventInit(&event, EVENT_TYPE_BASIC, 10), UTILS_STATUS_SUCCESS);
    assert_int_equal(eventInit(&eventEx, EVENT_TYPE_EXTENDED, 10), UTILS_STATUS_SUCCESS);
    eventRegister(&event, basic_event_handler);
    eventRegister(&event, basic_event_handler);
    basic_event_triggered = 0;
    assert_int_equal(eventDispatch(&event), UTILS_STATUS_SUCCESS);
    assert_int_equal(basic_event_triggered, 2);
    assert_int_equal(eventDispatch(&event), UTILS_STATUS_SUCCESS);
    assert_int_equal(basic_event_triggered, 4);
    /* Check dispatch of wrong type of event */
    assert_int_equal(eventDispatch(&eventEx), UTILS_STATUS_ERROR);
    eventDelete(&event);
    eventDelete(&eventEx);
}

static void test_eventDispatchEx(void** state) {
    (void)state; /* unused */
    event_t event, eventEx;
    assert_int_equal(eventInit(&event, EVENT_TYPE_BASIC, 10), UTILS_STATUS_SUCCESS);
    assert_int_equal(eventInit(&eventEx, EVENT_TYPE_EXTENDED, 10), UTILS_STATUS_SUCCESS);
    eventRegisterExt(&eventEx, extended_event_handler);
    eventRegisterExt(&eventEx, extended_event_handler);
    extended_event_triggered = 0;
    int test_value = 42;
    extended_event_value = NULL;
    assert_int_equal(eventDispatchEx(&eventEx, &test_value), UTILS_STATUS_SUCCESS);
    assert_int_equal(extended_event_triggered, 2);
    assert_int_equal(eventDispatchEx(&eventEx, &test_value), UTILS_STATUS_SUCCESS);
    assert_int_equal(extended_event_triggered, 4);
    assert_ptr_equal(extended_event_value, &test_value);
    /* Check dispatch of wrong type of event */
    assert_int_equal(eventDispatchEx(&event, &test_value), UTILS_STATUS_ERROR);
    eventDelete(&event);
    eventDelete(&eventEx);
}

static void test_eventDelete(void** state) {
    (void)state; /* unused */
    event_t event;
    assert_int_equal(eventInit(&event, EVENT_TYPE_BASIC, 10), UTILS_STATUS_SUCCESS);
    assert_int_equal(eventDelete(&event), UTILS_STATUS_SUCCESS);
    /* Check null deletion */
    event.eventsList = NULL;
    assert_int_equal(eventDelete(&event), UTILS_STATUS_ERROR);
}

int main(void) {
    const struct CMUnitTest test_event[] = {cmocka_unit_test(test_eventInit),     cmocka_unit_test(test_eventInitStatic),
                                            cmocka_unit_test(test_eventRegister), cmocka_unit_test(test_eventRegisterExt),
                                            cmocka_unit_test(test_eventDispatch), cmocka_unit_test(test_eventDispatchEx),
                                            cmocka_unit_test(test_eventDelete)};

    return cmocka_run_group_tests(test_event, NULL, NULL);
}
