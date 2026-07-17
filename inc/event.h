/* BEGIN Header */
/**
 ******************************************************************************
 * \file            event.h
 * \author          Andrea Vivani
 * \brief           Implementation of event handler
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ADVUTILS_EVENT_H
#define ADVUTILS_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "commonTypes.h"

/* Typedefs ------------------------------------------------------------------*/

/**
 * Event type 
 */
typedef enum {
    EVENT_TYPE_BASIC,
    EVENT_TYPE_EXTENDED,
} eventType_t;

/**
 * Basic event callback type 
 */
typedef void (*eventCBType_t)(void);

/**
 * Extended event callback type 
 */
typedef void (*eventExtCBType_t)(void* val);

/**
 * Event
 */
typedef struct {
    eventExtCBType_t* eventsList;
    eventType_t type;
    uint16_t count;
    uint16_t size;
} event_t;

/* Function prototypes -------------------------------------------------------*/

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Init event structure
 *
 * \param[in]       event: pointer to event object
 * \param[in]       type: type of event callback, either EVENT_TYPE_BASIC or EVENT_TYPE_EXTENDED
 * \param[in]       size: maximum number of event callbacks
 * 
 * \return          UTILS_STATUS_SUCCESS if event is initialized, UTILS_STATUS_ERROR if data was not allocated correctly
 */
utilsStatus_t eventInit(event_t* event, eventType_t type, uint16_t size);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

/**
 * \brief           Init event structure with static memory allocation
 *
 * \param[in]       event: pointer to event object
 * \param[in]       data: pointer to data array with size = size * sizeof(eventCBType_t or eventExtCBType_t)
 * \param[in]       type: type of event callback, either EVENT_TYPE_BASIC or EVENT_TYPE_EXTENDED
 * \param[in]       size: maximum number of event callbacks
 */
void eventInitStatic(event_t* event, eventExtCBType_t* data, eventType_t type, uint16_t size);

/**
 * \brief           Register basic callback to a specific event
 *
 * \param[in]       event: pointer to event object
 * \param[in]       callback: pointer to callback with type void(fun)(void)
 *  
 * \return          UTILS_STATUS_SUCCESS if callback is registered correctly, UTILS_STATUS_FULL if event is full, UTILS_STATUS_ERROR otherwise
 */
utilsStatus_t eventRegister(event_t* event, eventCBType_t callback);

/**
 * \brief           Register extended callback to a specific event
 *
 * \param[in]       event: pointer to event object
 * \param[in]       callback: pointer to callback with type void(fun)(void* val)
 *  
 * \return          UTILS_STATUS_SUCCESS if callback is registered correctly, UTILS_STATUS_FULL if event is full, UTILS_STATUS_ERROR otherwise
 */
utilsStatus_t eventRegisterEx(event_t* event, eventExtCBType_t callback);

/**
 * \brief           Dispatch an event with basic callback
 *
 * \param[in]       event: pointer to event object
 *  
 * \return          UTILS_STATUS_SUCCESS if event is dispatched correctly, UTILS_STATUS_ERROR otherwise
 */
utilsStatus_t eventDispatch(event_t* event);

/**
 * \brief           Dispatch an event with extended callback
 *
 * \param[in]       event: pointer to event object
 * \param[in]       val: pointer to value to be passed to callback functions
 *  
 * \return          UTILS_STATUS_SUCCESS if event is dispatched correctly, UTILS_STATUS_ERROR otherwise
 */
utilsStatus_t eventDispatchEx(event_t* event, void* val);

#ifdef ADVUTILS_USE_DYNAMIC_ALLOCATION
/**
 * \brief           Delete event handler
 *
 * \param[in]       event: pointer to event object
 *
 * \return          UTILS_STATUS_SUCCESS if event data is deleted, UTILS_STATUS_ERROR if data was already deleted
 */
utilsStatus_t eventDelete(event_t* event);
#endif /* ADVUTILS_USE_DYNAMIC_ALLOCATION */

#ifdef __cplusplus
}
#endif

#endif /* ADVUTILS_EVENT_H */
