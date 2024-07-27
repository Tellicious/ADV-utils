/* BEGIN Header */
/**
 ******************************************************************************
 * \file            timer.c
 * \author          Andrea Vivani
 * \brief           Implementation of timer functions
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

/* Includes ------------------------------------------------------------------*/

#include "timer.h"

/* Functions -----------------------------------------------------------------*/

void timerProcess(userTimer_t* t, uint32_t currentTick) {
    uint32_t k, events;
    if (t->flag && currentTick >= t->target_tick) {
        events = (currentTick - t->target_tick + t->interval) / t->interval;
        t->event_cnt += events;
        k = t->target_tick; /* to prevent Warning volatile access in IAR EWARM   */
        t->target_tick = k + t->interval * events;
    }
}
