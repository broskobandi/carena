#include <ctest.h>
#include "carena_private.h"
#include <string.h>

void test_add_to_and_remove_from_free_list() {
	arena_t arena = {0};
	size_t size = 32;
	meta_t meta1 = {
		.data = NULL,
		.is_valid = true,
		.next = NULL,
		.prev = NULL,
		.next_free = NULL,
		.prev_free = NULL,
		.total_size = TOTAL_SIZE(size)
	};
	meta_t meta2 = meta1;
	meta_t meta3 = meta1;

	add_to_free_list(&meta1, &arena);
	add_to_free_list(&meta2, &arena);
	add_to_free_list(&meta3, &arena);

	CTEST(meta1.next_free == &meta2);
	CTEST(meta2.prev_free == &meta1);
	CTEST(meta2.next_free == &meta3);
	CTEST(meta3.prev_free == &meta2);
	CTEST(arena.free_tails[SIZE_CLASS(TOTAL_SIZE(size))] == &meta3);
	CTEST(!meta1.is_valid);
	CTEST(!meta2.is_valid);
	CTEST(!meta3.is_valid);

	remove_from_free_list(&meta2, &arena);
	CTEST(meta2.is_valid);
	CTEST(meta1.next_free == &meta3);
	CTEST(meta3.prev_free == &meta1);
	CTEST(arena.free_tails[SIZE_CLASS(TOTAL_SIZE(size))] == &meta3);

	remove_from_free_list(&meta3, &arena);
	CTEST(meta3.is_valid);
	CTEST(arena.free_tails[SIZE_CLASS(TOTAL_SIZE(size))] == &meta1);
	CTEST(!meta1.next_free);

	remove_from_free_list(&meta1, &arena);
	CTEST(meta1.is_valid);
	CTEST(!arena.free_tails[SIZE_CLASS(TOTAL_SIZE(size))]);
}

void test_use_free_list() {
	arena_t arena = {0};
	size_t size = 32;
	meta_t meta1 = {
		.data = NULL,
		.is_valid = true,
		.next = NULL,
		.prev = NULL,
		.next_free = NULL,
		.prev_free = NULL,
		.total_size = TOTAL_SIZE(size)
	};
	meta_t meta2 = meta1;
	meta_t meta3 = meta1;

	add_to_free_list(&meta1, &arena);
	add_to_free_list(&meta2, &arena);
	add_to_free_list(&meta3, &arena);

	void *data1 = use_free_list(size, &arena);
	CTEST(META(data1) == &meta3);
	void *data2 = use_free_list(size, &arena);
	CTEST(META(data2) == &meta2);
	void *data3 = use_free_list(size, &arena);
	CTEST(META(data3) == &meta1);
}

void test_use_arena() {
	arena_t arena = {0};
	size_t size = 32;
	CTEST(!arena.tail);
	void *data1 = use_arena(size, &arena);
	CTEST(arena.tail == META(data1));
	void *data2 = use_arena(size, &arena);
	CTEST(arena.tail == META(data2));
	void *data3 = use_arena(size, &arena);
	CTEST(arena.tail == META(data3));

	CTEST(META(data1)->next == META(data2));
	CTEST(META(data2)->prev == META(data1));
	CTEST(META(data2)->next == META(data3));
	CTEST(META(data3)->prev == META(data2));
}

void test_carena_alloc() {
	arena_t *arena = get_g_arena();
	memset(arena, 0, sizeof(arena_t));
	size_t size = 32;
	CTEST(!arena->tail);

	void *data1 = carena_alloc(size);
	CTEST(data1 == META(data1)->data);
	CTEST(arena->tail == META(data1));

	void *data2 = carena_alloc(size);
	CTEST(data2 == META(data2)->data);
	CTEST(arena->tail == META(data2));

	void *data3 = carena_alloc(size);
	CTEST(data3 == META(data3)->data);
	CTEST(arena->tail == META(data3));

	CTEST(META(data1)->next == META(data2));
	CTEST(META(data2)->prev == META(data1));
	CTEST(META(data2)->next == META(data3));
	CTEST(META(data3)->prev == META(data2));

	memset(arena, 0, sizeof(arena_t));
}

void test_remove_from_list() {
	arena_t arena = {0};
	size_t size = 32;
	void *data1 = use_arena(size, &arena);
	void *data2 = use_arena(size, &arena);
	void *data3 = use_arena(size, &arena);

	remove_from_list(META(data2), &arena);
	CTEST(META(data1)->next == META(data3));
	CTEST(META(data3)->prev == META(data1));
	CTEST(arena.tail = META(data3));

	remove_from_list(META(data3), &arena);
	CTEST(!META(data1)->next);
	CTEST(arena.tail = META(data1));

	remove_from_list(META(data1), &arena);
	CTEST(!arena.tail);
}

void test_merge_free_mem() {
	arena_t *arena = get_g_arena();
	memset(arena, 0, sizeof(arena_t));
	size_t size = 32;
	void *data1 = carena_alloc(size);
	void *data2 = carena_alloc(size);
	void *data3 = carena_alloc(size);
	void *data4 = carena_alloc(size);
	void *data5 = carena_alloc(size);

	add_to_free_list(META(data2), arena);
	add_to_free_list(META(data3), arena);
	add_to_free_list(META(data4), arena);

	merge_free_mem(META(data3), arena);

	CTEST(META(data1)->next == META(data2));
	CTEST(META(data2)->prev == META(data1));
	CTEST(META(data2)->next == META(data5));
	CTEST(META(data5)->prev == META(data2));
	CTEST(META(data2)->total_size == TOTAL_SIZE(size) * 3);

	memset(arena, 0, sizeof(arena_t));
}

void test_carena_free() {
	arena_t *arena = get_g_arena();
	memset(arena, 0, sizeof(arena_t));

	size_t size = 32;
	void *data1 = carena_alloc(size);
	void *data2 = carena_alloc(size);
	void *data3 = carena_alloc(size);
	void *data4 = carena_alloc(size);
	void *data5 = carena_alloc(size);

	CTEST(arena->tail == META(data5));
	CTEST(!arena->free_tails[SIZE_CLASS(TOTAL_SIZE(size))]);

	carena_free(data2);
	CTEST(arena->free_tails[SIZE_CLASS(TOTAL_SIZE(size))] == META(data2));
	
	carena_free(data4);
	CTEST(arena->free_tails[SIZE_CLASS(TOTAL_SIZE(size))] == META(data4));

	carena_free(data3);
	CTEST(arena->free_tails[SIZE_CLASS(META(data2)->total_size)] == META(data2));
	CTEST(META(data2)->next == META(data5));
	CTEST(META(data5)->prev == META(data2));

	carena_free(data1);
	CTEST(arena->free_tails[SIZE_CLASS(META(data1)->total_size)] == META(data1));
	CTEST(META(data1)->next == META(data5));
	CTEST(META(data5)->prev == META(data1));

	carena_free(data5);
	CTEST(arena->free_tails[SIZE_CLASS(META(data1)->total_size)] == META(data1));
	CTEST(!META(data1)->next);
	CTEST(!META(data1)->next_free);

	memset(arena, 0, sizeof(arena_t));
}

void test_carena_alloc_with_free_list() {
	arena_t *arena = get_g_arena();
	memset(arena, 0, sizeof(arena_t));
	size_t size = 32;

	void *data1 = carena_alloc(size);

	carena_free(data1);

	void *data2 = carena_alloc(size);
	CTEST(data1 == data2);

	memset(arena, 0, sizeof(arena_t));
}

int main(void) {
	test_add_to_and_remove_from_free_list();
	test_use_free_list();
	test_use_arena();
	test_carena_alloc();
	test_remove_from_list();
	test_merge_free_mem();
	test_carena_free();
	test_carena_alloc_with_free_list();
	ctest_print_results();
	return 0;
}
