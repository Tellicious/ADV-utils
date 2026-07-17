/* BEGIN Header */
/**
 ******************************************************************************
 * \file            ADVUtilsAssert.h
 * \author          Andrea Vivani
 * \brief           Implementation of a simple assert macro
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
#ifndef ADVUTILS_ASSERT_H
#define ADVUTILS_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#ifdef ADVUTILS_ASSERT_HEADER
#include ADVUTILS_ASSERT_HEADER
#endif /* ADVUTILS_ASSERT_HEADER */

/* Macros --------------------------------------------------------------------*/

#ifndef ADVUTILS_ASSERT
#ifdef DEBUG
#define ADVUTILS_ASSERT(x)                                                                                                                                     \
    if ((x) == 0) {                                                                                                                                            \
        for (;;)                                                                                                                                               \
            ;                                                                                                                                                  \
    }
#else
#define ADVUTILS_ASSERT(x)
#endif /* DEBUG */
#endif /* ADVUTILS_ASSERT */

#ifdef __cplusplus
}
#endif

#endif /* ADVUTILS_ASSERT_H */
