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

/** \file src/carena_private.h
 * \brief Private header file for the carena library.
 * \details This file contains private functions and struct definitions 
 * for the carena library. */

#ifndef CARENA_PRIVATE_H
#define CARENA_PRIVATE_H

#include "carena.h"
#include <stdalign.h>
#include <stdbool.h>

#define ARENA_SIZE_DEFAULT 128 * 1024LU
#ifndef ARENA_SIZE_MULTIPLIER
#define ARENA_SIZE ARENA_SIZE_DEFAULT
#else
#define ARENA_SIZE ARENA_SIZE_DEFAULT * ARENA_SIZE_MULTIPLIER
#endif

#define MIN_ALLOCATION alignof(max_align_t)
#define ROUNDUP(size)\
	(((size) + MIN_ALLOCATION - 1) & ~(MIN_ALLOCATION - 1))
#define META_SIZE ROUNDUP(sizeof(meta_t))
#define NUM_SIZE_CLASSES\
	((ARENA_SIZE - META_SIZE) / MIN_ALLOCATION)
#define SIZE_CLASS(total_size)\
	(((total_size) - META_SIZE) / MIN_ALLOCATION - 1)
#define TOTAL_SIZE(size)\
	(META_SIZE + ROUNDUP((size)))
#define META(data)\
	((meta_t*)((unsigned char*)(data) - META_SIZE))
#define DATA(meta)\
	((void*)((unsigned char*)(meta) + META_SIZE))

typedef struct meta meta_t;
typedef struct arena arena_t;

struct meta {
	void *data;
	size_t total_size;
	meta_t *next_free;
	meta_t *prev_free;
	meta_t *next;
	meta_t *prev;
	bool is_valid;
};

struct arena {
	alignas(max_align_t) unsigned char buff[ARENA_SIZE];
	meta_t *free_tails[NUM_SIZE_CLASSES];
	meta_t *tail;
	size_t offset;
};

arena_t *get_g_arena();

static inline void add_to_free_list(meta_t *meta, arena_t *arena) {
	meta->is_valid = false;
	meta_t **free_tail = &arena->free_tails[SIZE_CLASS(meta->total_size)];
	if (*free_tail) {
		meta->prev_free = *free_tail;
		(*free_tail)->next_free = meta;
	}
	*free_tail = meta;
}

static inline void remove_from_free_list(meta_t *meta, arena_t *arena) {
	meta->is_valid = true;
	meta_t **free_tail = &arena->free_tails[SIZE_CLASS(meta->total_size)];
	if (*free_tail == meta)
		*free_tail = meta->prev_free;
	if (meta->next_free)
		meta->next_free->prev_free = meta->prev_free;
	if (meta->prev_free)
		meta->prev_free->next_free = meta->next_free;
}

static inline void add_to_list(meta_t *meta, arena_t *arena) {
	meta->prev = arena->tail;
	if (arena->tail)
		arena->tail->next = meta;
	arena->tail = meta;
}

static inline void remove_from_list(meta_t *meta, arena_t *arena) {
	if (arena->tail == meta)
		arena->tail = arena->tail->prev;
	if (meta->next)
		meta->next->prev = meta->prev;
	if (meta->prev)
		meta->prev->next = meta->next;
}

static inline void merge_free_mem(meta_t *meta, arena_t *arena) {
	if (meta->next && !meta->next->is_valid) {
		remove_from_free_list(meta, arena);
		remove_from_free_list(meta->next, arena);
		meta->total_size += meta->next->total_size;
		add_to_free_list(meta, arena);
		remove_from_list(meta->next, arena);
	}
	if (meta->prev && !meta->prev->is_valid) {
		remove_from_free_list(meta, arena);
		remove_from_free_list(meta->prev, arena);
		meta->prev->total_size += meta->total_size;
		add_to_free_list(meta->prev, arena);
		remove_from_list(meta, arena);
	}
}

static inline void *use_free_list(size_t size, arena_t *arena) {
	meta_t *meta = arena->free_tails[SIZE_CLASS(TOTAL_SIZE(size))];
	remove_from_free_list(meta, arena);
	meta->data = DATA(meta);
	return meta->data;
}

static inline void *use_arena(size_t size, arena_t *arena) {
	meta_t *meta = (meta_t*)&arena->buff[arena->offset];
	meta->data = DATA(meta);
	meta->total_size = TOTAL_SIZE(size);
	meta->next_free = NULL;
	meta->prev_free = NULL;
	meta->is_valid = true;
	meta->next = NULL;
	add_to_list(meta, arena);
	arena->offset += TOTAL_SIZE(size);
	return meta->data;
}

#endif
