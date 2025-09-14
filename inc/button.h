/* BEGIN Header */
/**
 ******************************************************************************
 * \file            button.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTON_H__
#define __BUTTON_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/* Macros --------------------------------------------------------------------*/

/* Typedefs ------------------------------------------------------------------*/

/**
 * Button status
 */
typedef enum { BUTTON_RELEASED = 0, BUTTON_PRESSED = 1 } buttonStatus_t;

/**
 * Button type
 */
typedef enum { BUTTON_TYPE_NORMAL = 0, BUTTON_TYPE_PULSATING = 1 } buttonType_t;

/**
 * Button press type
 */
typedef enum {
    BUTTON_NO_PRESS = 0,
    BUTTON_SHORT_PRESS = 1,
    BUTTON_DOUBLE_PRESS = 2,
    BUTTON_TRIPLE_PRESS = 3,
    BUTTON_MULTIPLE_PRESS = 4,
    BUTTON_PULSATING_PRESS = 5,
    BUTTON_LONG_PRESS = 8,
    BUTTON_VERYLONG_PRESS = 9,
    BUTTON_RELEASE_PRESS = 10
} buttonPressType_t;

/**
 * Button struct
 */
typedef struct {
    buttonType_t type       : 1;
    buttonStatus_t status   : 1;
    buttonPressType_t press : 5;
    uint8_t event           : 1;
    uint8_t pulses;
    uint16_t debounceTicks, resetTicks, longPressTicks, veryLongPressTicks;
    uint32_t validTick[2], lastTick[2];
} __attribute__((packed)) button_t;

/* Function prototypes -------------------------------------------------------*/

/**
 * \brief           Init button structure
 *
 * \param[in]       button: pointer to button object
 * \param[in]       type: type of button, normal (BUTTON_TYPE_NORMAL) or pulsating (BUTTON_TYPE_PULSATING)
 * \param[in]       debounceTicks: number of ticks used for debouncing. Use 20ms as a starting point
 * \param[in]       resetTicks: number of ticks to reset pulse count. Use 400ms as a starting point. It must be greater than debounceTicks
 * \param[in]       longPressTicks: number of ticks that the button needs to be pressed (and then released) to detect long-press
 * \param[in]       veryLongPressTicks: number of ticks that the button needs to be pressed to detect very long-press
 */
void buttonInit(button_t* button, buttonType_t type, uint32_t debounceTicks, uint32_t resetTicks, uint32_t longPressTicks, uint32_t veryLongPressTicks);

/**
 * \brief           Button event update, to be called either in an EXT interrupt or in a timer
 *
 * \param[in]       button: pointer to button object
 * \param[in]       status: current button status
 * \param[in]       ticks: current system ticks
 */
void buttonEvent(button_t* button, buttonStatus_t status, uint32_t ticks);

/**
 * \brief           Get button press, to be called in the main loop
 *
 * \param[in]       button: pointer to button object
 * \param[in]       ticks: current system ticks
 * 
 * \return          press types according to buttonPressType_t. BUTTON_TYPE_PULSATING returns only BUTTON_NO_PRESS, BUTTON_PULSATING_PRESS and BUTTON_RELEASE_PRESS
 */
buttonPressType_t buttonGetPress(button_t* button, uint32_t ticks);

/**
 * \brief           Get button status, to be called in the main loop
 *
 * \param[in]       button: pointer to button object
 * \param[in]       ticks: current system ticks
 *
 * \return          BUTTON_PRESSED, BUTTON_RELEASED according to current button status
 */

static inline buttonStatus_t buttonGetStatus(button_t* button, uint32_t ticks) {
    return (((button->validTick[button->status] > button->validTick[!button->status]) || (ticks - button->lastTick[button->status] > button->debounceTicks))
                ? button->status
                : (buttonStatus_t)!button->status);
}

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H__ */
