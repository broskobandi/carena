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

/** \file src/carena.c
 * \brief Implementation for the carena library
 * \details This file contains the definitions of the public 
 * functions and global variables for the carena library. */

#include "carena_private.h"

_Thread_local static arena_t g_arena;

_Thread_local static const char *g_err;

arena_t *get_g_arena() {
	return &g_arena;
}

/** Allocates a block of memory of 'size' number of bytes in the arena.
 * \param size The number of bytes in the arena that's owned by teh user
 * (the actual allocation size is larger to account for metadata and 
 * padding).
 * \return A pointer to the beginning of the allocated memory block 
 * or NULL on failure. */
void *carena_alloc(size_t size) {
	if (SIZE_CLASS(TOTAL_SIZE(size)) >= NUM_SIZE_CLASSES) {
		g_err = "'size' is too big.";
		return NULL;
	} else if (g_arena.free_tails[SIZE_CLASS(TOTAL_SIZE(size))]) {
		return use_free_list(size, &g_arena);
	} else if (g_arena.offset + TOTAL_SIZE(size) <= ARENA_SIZE) {
		return use_arena(size, &g_arena);
	} else {
		g_err = "Arena is full.";
		return NULL;
	}
}

/** Deallocates a block of memory in the arena.
 * \param ptr A pointer to the beginning of the memory block to be
 * deallocated. */
void carena_free(void *ptr) {
	if (!ptr || !META(ptr)->is_valid) {
		g_err = "Invalid argument.";
		return;
	}
	if (!META(ptr)->next) {
		g_arena.offset -= META(ptr)->total_size;
		remove_from_list(META(ptr), &g_arena);
		return;
	}
	add_to_free_list(META(ptr), &g_arena);
	merge_free_mem(META(ptr), &g_arena);
	return;
}

/** Resizes a memory block to 'size' number of bytes.
 * \param ptr A pointer to the memory block to be resized. 
 * \param size The new size. 
 * \return A pointer to the new allocation (it is the same as 'ptr'
 * if the reallocation could be done in place) or NULL on failure. */
void *carena_realloc(void *ptr, size_t size) {
	if (!ptr || !META(ptr)->is_valid) {
		g_err = "Invalid argument.";
		return NULL;
	}
	carena_free(ptr);
	return carena_alloc(size);
}

/** Return a string containing the latest error information. */
const char *carena_get_error() {
	return g_err;
}
