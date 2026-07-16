/* BEGIN Header */
/**
 ******************************************************************************
 * \file            hashFunctions.c
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

/* Includes ------------------------------------------------------------------*/

#include "hashFunctions.h"
#include "ADVUtilsAssert.h"
#include "stddef.h"

/* Functions -----------------------------------------------------------------*/

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME  1099511628211UL

/* 
 * Return 32-bit FNV-1a hash for key (NUL-terminated). See description:
 * https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
 */
uint32_t hash_FNV1A(const char* key) {

    uint64_t hash = FNV_OFFSET;
    uint8_t p;

    ADVUTILS_ASSERT(key != NULL);

    while ((p = (uint8_t)*key++)) {
        hash ^= (uint64_t)p;
        hash *= FNV_PRIME;
    }

    /* limit to 32 bit */

    hash &= UINT32_MAX;

    return (uint32_t)hash;
}

/*
* This algorithm (k=33) was first reported by dan bernstein many years ago in comp.lang.c. another version of this algorithm (now favored by bernstein) uses xor: hash(i) = 
* hash(i - 1 * 33 ^ str[i]; the magic of number 33 (why it works better than many other constants, prime or not) has never been adequately explained.
*/
uint32_t hash_djb(const char* key) {
    uint32_t hash = 5381;
    uint8_t p;

    ADVUTILS_ASSERT(key != NULL);

    while ((p = (uint8_t)*key++)) {
        hash = ((hash << 5) + hash) + p; /* hash * 33 + c */
    }

    return hash;
}

/* 
* This algorithm was created for sdbm (a public-domain reimplementation of ndbm) database library. it was found to do well in scrambling bits, causing better distribution of the keys
* and fewer splits. it also happens to be a good general hashing function with good distribution. the actual function is hash(i) = hash(i - 1) * 65599 + str[i]; what is included below
* is the faster version used in gawk. [there is even a faster, duff-device version] the magic constant 65599 was picked out of thin air while experimenting with different constants,
* and turns out to be a prime. this is one of the algorithms used in berkeley db (see sleepycat) and elsewhere.
*/
uint32_t hash_sdbm(const char* key) {
    uint32_t hash = 0;
    uint8_t p;

    ADVUTILS_ASSERT(key != NULL);

    while ((p = (uint8_t)*key++)) {
        hash = p + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}
