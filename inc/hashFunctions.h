/* BEGIN Header */
/**
 ******************************************************************************
 * \file            hashFunctions.h
 * \author          Andrea Vivani
 * \brief           Collection of different hash functions
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
#ifndef __HASHFUNCTIONS_H__
#define __HASHFUNCTIONS_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/* Macros --------------------------------------------------------------------*/

/* Typedefs ------------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/**
 * \brief           64-bit FNV-1a hash for key (NUL-terminated)
 *
 * \param[in]       key: pointer to key string
 * 
 * \return          hash
 */

uint32_t hash_FNV1A(const char* key);

/**
 * \brief           Dan Bernstein hash for key (from comp.lang.c)
 *
 * \param[in]       key: pointer to key string
 * 
 * \return          hash
 */
uint32_t hash_djb(const char* key);

/**
 * \brief           sdbm hash for key (from sdbm database library)
 *
 * \param[in]       key: pointer to key string
 * 
 * \return          hash
 */
uint32_t hash_sdbm(const char* key);

#ifdef __cplusplus
}
#endif

#endif /* __HASHFUNCTIONS_H__ */
