/*
MIT License

Copyright (c) 2025 broskobandi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/** \file include/carena.h
 * \brief Public header file for the carean library.
 * \details This file contains forward declarations of the public 
 * functions for the carena library. */

#ifndef CARENA_H
#define CARENA_H

#include <stddef.h> /* For size_t */

/** Allocates a block of memory of 'size' number of bytes in the arena.
 * \param size The number of bytes in the arena that's owned by teh user
 * (the actual allocation size is larger to account for metadata and 
 * padding).
 * \return A pointer to the beginning of the allocated memory block 
 * or NULL on failure.
 * This function sets cerror on failure. */
void *carena_alloc(size_t size);

/** Deallocates a block of memory in the arena.
 * \param ptr A pointer to the beginning of the memory block to be
 * deallocated.
 * This function sets cerror on failure. */
void carena_free(void *ptr);

/** Resizes a memory block to 'size' number of bytes.
 * \param ptr A pointer to the memory block to be resized. 
 * \param size The new size. 
 * \return A pointer to the new allocation (it is the same as 'ptr'
 * if the reallocation could be done in place) or NULL on failure.
 * This function sets cerror on failure. */
void *carena_realloc(void *ptr, size_t size);

/** Return a string containing the latest error information. */
const char *carena_get_error();

#endif
