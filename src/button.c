/* BEGIN Header */
/**
 ******************************************************************************
 * \file            button.c
 * \author          Andrea Vivani
 * \brief           Implementation of button object with debouncing and multiple
 *                  press types detection
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

#include "button.h"

/* Functions ------------------------------------------------------------------*/

void buttonInit(button_t* button, buttonType_t type, uint32_t debounceTicks, uint32_t resetTicks, uint32_t longPressTicks, uint32_t veryLongPressTicks) {
    button->type = type;
    button->status = BUTTON_RELEASED;
    button->press = BUTTON_NO_PRESS;
    button->validTick[0] = 1;
    button->validTick[1] = 0;
    button->lastTick[0] = 0;
    button->lastTick[1] = 0;
    button->pulses = 0;
    button->event = 1;
    button->debounceTicks = debounceTicks;
    button->resetTicks = (resetTicks < debounceTicks) ? debounceTicks : resetTicks;
    button->longPressTicks = longPressTicks;
    button->veryLongPressTicks = veryLongPressTicks;
}

void buttonEvent(button_t* button, buttonStatus_t status, uint32_t ticks) {
    uint8_t invStatus = (status == BUTTON_PRESSED) ? 0U : 1U;
    if (button->status == status) {
        return;
    }
    button->lastTick[status] = ticks;
    button->status = status;
    if (((button->lastTick[status] - button->lastTick[invStatus]) >= button->debounceTicks)) {
        button->event |= status;
        button->validTick[invStatus] = button->lastTick[invStatus];
        if (invStatus && ((button->type == BUTTON_TYPE_PULSATING) || ((button->lastTick[0] - button->validTick[1]) < button->veryLongPressTicks))) {
            if ((button->validTick[1] - button->validTick[0]) > button->resetTicks) {
                button->pulses = 1U;
            } else {
                button->pulses++;
            }
        }
    }
}

buttonPressType_t buttonGetPress(button_t* button, uint32_t ticks) {
    /* Update button status and pulses */
    button->press = BUTTON_NO_PRESS;

    /* Check button press type */
    if (button->type == BUTTON_TYPE_NORMAL) {
        if ((button->status == BUTTON_PRESSED) && !button->pulses && ((ticks - button->lastTick[1]) > button->veryLongPressTicks) && button->event) {
            button->press = BUTTON_VERYLONG_PRESS;
            button->event = 0;
            button->pulses = 0;
        } else if ((button->pulses == 1U) && ((button->lastTick[0] - button->validTick[1]) > button->longPressTicks)) {
            button->press = BUTTON_LONG_PRESS;
            button->event = 0;
            button->pulses = 0;
        } else if ((button->status == BUTTON_RELEASED) && !button->pulses && ((ticks - button->lastTick[0]) > button->debounceTicks) && !button->event) {
            button->press = BUTTON_RELEASE_PRESS;
            button->event = 1;
        } else if (((ticks - button->validTick[1]) > button->resetTicks) && button->pulses) {
            switch (button->pulses) {
                case 1: button->press = BUTTON_SHORT_PRESS; break;
                case 2: button->press = BUTTON_DOUBLE_PRESS; break;
                case 3: button->press = BUTTON_TRIPLE_PRESS; break;
                default: button->press = BUTTON_MULTIPLE_PRESS; break;
            }
            button->event = 0;
            button->pulses = 0;
        } else {
            /* no action required (MISRA 15.7) */
        }
    } else {
        if (button->pulses && (button->status == BUTTON_RELEASED) && ((ticks - button->lastTick[0]) > button->resetTicks)) {
            button->press = BUTTON_RELEASE_PRESS;
            button->pulses = 0;
        } else if (button->pulses > 0U) {
            button->press = BUTTON_PULSATING_PRESS;
        } else {
            /* no action required (MISRA 15.7) */
        }
    }
    return button->press;
}
