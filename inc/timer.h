/* BEGIN Header */
/**
 ******************************************************************************
 * \file            timer.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ADVUTILS_TIMER_H
#define ADVUTILS_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/* Typedefs ------------------------------------------------------------------*/

typedef volatile struct {
    uint32_t lastTick;
    uint32_t interval;
    uint8_t flag;
    uint16_t eventCnt;
} userTimer_t;

/* Function prototypes -------------------------------------------------------*/

/**
 * \brief           Setup timer object
 *
 * \param[in]       t: pointer to timer object
 * \param[in]       interval: required timer interval
 */
static inline void timerInit(userTimer_t* t, uint32_t interval) {
    t->interval = interval;
    t->flag = 0;
    t->eventCnt = 0;
}

/**
 * \brief           Start timer object
 *
 * \param[in]       t: pointer to timer object
 * \param[in]       currentTick: value of current tick
 */
static inline void timerStart(userTimer_t* t, uint32_t currentTick) {
    t->lastTick = currentTick;
    t->flag = 1;
}

/**
 * \brief           Stop timer object
 *
 * \param[in]       t: pointer to timer object
 */
static inline void timerStop(userTimer_t* t) {
    t->eventCnt = 0;
    t->flag = 0;
}

/**
 * \brief           Reset timer object
 *
 * \param[in]       t: pointer to timer object
 */
static inline void timerClear(userTimer_t* t) { t->eventCnt = 0; }

/**
 * \brief           Process timer object
 *
 * \param[in]       t: pointer to timer object
 * \param[in]       currentTick: value of current tick
 *
 * \attention       to be called on a regular basis to update timers
 */
void timerProcess(userTimer_t* t, uint32_t currentTick);

/**
 * \brief           Check if any timer event exists
 *
 * \param[in]       t: pointer to timer object
 *
 * \return          number of events for the specified timer
 */
static inline uint16_t timerEventExists(const userTimer_t* t) { return t->eventCnt; }

#ifdef __cplusplus
}
#endif

#endif /* ADVUTILS_TIMER_H */
