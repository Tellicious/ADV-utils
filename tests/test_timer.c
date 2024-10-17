/* BEGIN Header */
/**
 ******************************************************************************
 * \file            test_timer.c
 * \author          Andrea Vivani
 * \brief           Unit tests for timer.c
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
#include "timer.h"

#include <cmocka.h>

/* Functions -----------------------------------------------------------------*/

static void test_timerInit(void** state) {
    (void)state; /* unused */
    userTimer_t timer;
    timerInit(&timer, 1000);
    assert_int_equal(timer.interval, 1000);
    assert_int_equal(timer.flag, 0);
    assert_int_equal(timer.event_cnt, 0);
}

static void test_timerStart(void** state) {
    (void)state; /* unused */
    userTimer_t timer;
    timerInit(&timer, 1000);
    timerStart(&timer, 500);
    assert_int_equal(timer.target_tick, 1500);
    assert_int_equal(timer.flag, 1);
}

static void test_timerStop(void** state) {
    (void)state; /* unused */
    userTimer_t timer;
    timerInit(&timer, 1000);
    timerStart(&timer, 500);
    timerStop(&timer);
    assert_int_equal(timer.flag, 0);
}

static void test_timerClear(void** state) {
    (void)state; /* unused */
    userTimer_t timer;
    timerInit(&timer, 1000);
    timer.event_cnt = 5;
    timerClear(&timer);
    assert_int_equal(timer.event_cnt, 0);
}

static void test_timerProcess(void** state) {
    (void)state; /* unused */
    userTimer_t timer;
    timerInit(&timer, 1000);
    timerStart(&timer, 500);

    // Process timer without reaching target tick
    timerProcess(&timer, 1000);
    assert_int_equal(timer.event_cnt, 0);

    // Process timer reaching target tick
    timerProcess(&timer, 1500);
    assert_int_equal(timer.event_cnt, 1);
    assert_int_equal(timer.target_tick, 2500);

    // Process timer reaching multiple intervals
    timerProcess(&timer, 3500);
    assert_int_equal(timer.event_cnt, 3);
    assert_int_equal(timer.target_tick, 4500);

    // Process stopped timer
    timerStop(&timer);
    timerProcess(&timer, 4502);
    assert_int_equal(timer.event_cnt, 0);
}

static void test_timerEventExists(void** state) {
    (void)state; /* unused */
    userTimer_t timer;
    timerInit(&timer, 1000);
    assert_int_equal(timerEventExists(&timer), 0);
    timer.event_cnt = 5;
    assert_int_equal(timerEventExists(&timer), 5);
}

static void test_timerWrapAround(void** state) {
    (void)state; /* unused */
    userTimer_t timer;
    timerInit(&timer, 500);
    timerStart(&timer, UINT32_MAX - 900);

    // Process timer not wrapping-around
    timerProcess(&timer, UINT32_MAX - 399);
    assert_int_equal(timer.event_cnt, 1);
    assert_int_equal(timer.target_tick, 100);

    // Process timer once it has wrapped-around
    timerProcess(&timer, 101);
    assert_int_equal(timer.event_cnt, 1);
    assert_int_equal(timer.target_tick, 600);

    // Test timer with initial wrap-around
    timerStop(&timer);
    timerStart(&timer, UINT32_MAX - 400);
    timerProcess(&timer, 101);
    assert_int_equal(timer.event_cnt, 1);
    assert_int_equal(timer.target_tick, 600);

    // Test timer wrapping-around with multiple events
    timerStop(&timer);
    timerStart(&timer, UINT32_MAX - 900);
    timerProcess(&timer, 601);
    assert_int_equal(timer.event_cnt, 3);
    assert_int_equal(timer.target_tick, 1100);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_timerInit),       cmocka_unit_test(test_timerStart),
        cmocka_unit_test(test_timerStop),       cmocka_unit_test(test_timerClear),
        cmocka_unit_test(test_timerProcess),    cmocka_unit_test(test_timerEventExists),
        cmocka_unit_test(test_timerWrapAround),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
