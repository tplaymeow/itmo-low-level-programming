#define _DEFAULT_SOURCE

#include "mem.h"
#include "mem_internals.h"
#include "test.h"
#include "util.h"

// Helpers

void debug(const char* fmt, ... );

static void test_assert_with_message(bool result, const char *msg) { if (!result) err(msg); }

#define test_log_header() _test_log_header(__func__)
static void _test_log_header(char const *test_name) {
    debug("-----------------------------------\n");
    debug(test_name);
    debug("\n-----------------------------------\n\n");
}

static void test_debug_heap_with_message(struct block_header *heap, const char *msg) {
    debug("%s\n", msg);
    debug_heap(stderr, heap);
    debug("\n");
}

static struct block_header *test_get_block_header (void *data) {
    return (struct block_header *) ((uint8_t *) data - offsetof(struct block_header, contents));
}

static void* test_block_after( struct block_header const* block ) {
  return  (void*) (block->contents + block->capacity.bytes);
}

// Tests

void test_mem_allocation(struct block_header *heap) {
    test_log_header();
    test_debug_heap_with_message(heap, "Начальное состояние кучи:");

    debug("Выделяем память под массив на 150 чисел типа `int64_t`:\n");
    size_t expected_capacity = sizeof(int64_t) * 150;
    int64_t *array = _malloc(expected_capacity);
    test_assert_with_message(array != NULL, "Память не выделилась =(\n");
    test_assert_with_message(!heap->is_free, "Блок не помечен как занятый\n");
    test_assert_with_message(heap->capacity.bytes == expected_capacity, "Capacity не равняется ожидаемому =(\n");
    debug("Успешно!\n\n");

    test_debug_heap_with_message(heap, "Конечное состояние кучи:");

    _free(array);
}

void test_free_without_merging(struct block_header *heap) {
    test_log_header();
    test_debug_heap_with_message(heap, "Начальное состояние кучи:");

    debug("Выделяем 2 блока памяти:\n");
    void *data1 = _malloc(3000); struct block_header *data1_block = test_get_block_header(data1);
    void *data2 = _malloc(1500); struct block_header *data2_block = test_get_block_header(data2);
    test_assert_with_message(data1 != NULL && data2 != NULL, "Память не выделилась =(\n");
    test_assert_with_message(!data1_block->is_free && !data2_block->is_free, "Блок не помечен как занятый\n");
    test_assert_with_message(data1_block->capacity.bytes == 3000 && data2_block->capacity.bytes == 1500, "Capacity не равняется ожидаемому =(\n");
    debug("Успешно!\n\n");

    test_debug_heap_with_message(heap, "Промежуточное состояние кучи:");
    
    debug("Освобождем первый блок памяти:\n");
    _free(data1);
    test_assert_with_message(data1_block->is_free, "Очищенный блок не помечен как свободный\n");
    test_assert_with_message(!data2_block->is_free, "Не очищенный блок помечен как свободный\n");
    debug("Успешно!\n\n");

    test_debug_heap_with_message(heap, "Конечное состояние кучи:");

    _free(data2);
}

void test_free_with_merging(struct block_header *heap) {
    test_log_header();
    test_debug_heap_with_message(heap, "Начальное состояние кучи:");

    debug("Выделяем 2 блока памяти:\n");
    void *data1 = _malloc(3000); struct block_header *data1_block = test_get_block_header(data1);
    void *data2 = _malloc(1500); struct block_header *data2_block = test_get_block_header(data2);
    test_assert_with_message(data1 != NULL && data2 != NULL, "Память не выделилась =(\n");
    test_assert_with_message(!data1_block->is_free && !data2_block->is_free, "Блок не помечен как занятый\n");
    test_assert_with_message(data1_block->capacity.bytes == 3000 && data2_block->capacity.bytes == 1500, "Capacity не равняется ожидаемому =(\n");
    debug("Успешно!\n\n");

    test_debug_heap_with_message(heap, "Промежуточное состояние кучи:");
    
    debug("Освобождем второй блок памяти:\n");
    _free(data2);
    test_assert_with_message(!data1_block->is_free, "Очищенный блок не помечен как свободный\n");
    test_assert_with_message(data2_block->is_free, "Не очищенный блок помечен как свободный\n");
    test_assert_with_message(data2_block->capacity.bytes > 1500, "Capacity не равняется ожидаемому, последующие блоки не объеденились\n");
    debug("Успешно!\n\n");

    test_debug_heap_with_message(heap, "Конечное состояние кучи:");

    _free(data1);
}

void test_free_two_blocks(struct block_header *heap) {
    test_log_header();
    test_debug_heap_with_message(heap, "Начальное состояние кучи:");

    debug("Выделяем 3 блока памяти:\n");
    void *data1 = _malloc(3000); struct block_header *data1_block = test_get_block_header(data1);
    void *data2 = _malloc(1500); struct block_header *data2_block = test_get_block_header(data2);
    void *data3 = _malloc(1700); struct block_header *data3_block = test_get_block_header(data3);
    test_assert_with_message(data1 != NULL && data2 != NULL && data3 != NULL, "Память не выделилась =(\n");
    test_assert_with_message(
        !data1_block->is_free && 
        !data2_block->is_free &&
        !data3_block->is_free, "Блок не помечен как занятый\n");
    test_assert_with_message(
        data1_block->capacity.bytes == 3000 && 
        data2_block->capacity.bytes == 1500 &&
        data3_block->capacity.bytes == 1700, "Capacity не равняется ожидаемому =(\n");
    debug("Успешно!\n\n");

    test_debug_heap_with_message(heap, "Промежуточное состояние кучи:");
    
    debug("Освобождем два блок памяти:\n");
    _free(data1);
    _free(data3);
    test_assert_with_message(data1_block->is_free, "Очищенный блок не помечен как свободный\n");
    test_assert_with_message(data3_block->is_free, "Очищенный блок не помечен как свободный\n");
    test_assert_with_message(!data2_block->is_free, "Не очищенный блок помечен как свободный\n");
    debug("Успешно!\n\n");

    test_debug_heap_with_message(heap, "Конечное состояние кучи:");

    _free(data2);
}

void test_allocate_big_blocks(struct block_header *heap) {
    test_log_header();
    test_debug_heap_with_message(heap, "Начальное состояние кучи:");

    debug("Выделяем 3 больших блока памяти:\n");
    void *data1 = _malloc(10000); struct block_header *data1_block = test_get_block_header(data1);
    void *data2 = _malloc(5000); struct block_header *data2_block = test_get_block_header(data2);
    void *data3 = _malloc(10000); struct block_header *data3_block = test_get_block_header(data3);
    test_assert_with_message(
        data1 != NULL &&
        data2 != NULL &&
        data3 != NULL, "Память не выделилась =(\n");
    test_assert_with_message(
        !data1_block->is_free && 
        !data2_block->is_free &&
        !data3_block->is_free, "Блок не помечен как занятый\n");
    test_assert_with_message(
        data1_block->capacity.bytes == 10000 && 
        data2_block->capacity.bytes == 5000 &&
        data3_block->capacity.bytes == 10000, "Capacity не равняется ожидаемому =(\n");
    test_assert_with_message(
        (uint8_t *)data1_block->contents + data1_block->capacity.bytes == (uint8_t *)data2_block &&
        (uint8_t *)data2_block->contents + data2_block->capacity.bytes == (uint8_t *)data3_block, "Блоки выделены не рядом");
    debug("Успешно!\n\n");

    test_debug_heap_with_message(heap, "Конечное состояние кучи:");
    
    _free(data3);
    _free(data2);
    _free(data1);
}

void test_allocate_big_blocks_region_in_other_range(struct block_header *heap) {
    test_log_header();
    test_debug_heap_with_message(heap, "Начальное состояние кучи:");

    debug("Выделяем большой блок памяти:\n");
    void *data1 = _malloc(10000); struct block_header *data1_block = test_get_block_header(data1);
    test_assert_with_message(!data1_block->is_free, "Блок не помечен как занятый\n");
    test_assert_with_message(data1_block->capacity.bytes == 10000, "Capacity не равняется ожидаемому =(\n");
    debug("Успешно!\n\n");

    struct block_header *last_block = heap;
    while (last_block->next != NULL) last_block = last_block->next;
    void *map_res = mmap(test_block_after(last_block), 1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    test_assert_with_message(map_res != MAP_FAILED, "mmap возвращает `MAP_FAILED`");

    debug("Выделяем ОЧЕНЬ большой блок памяти:\n");
    void *data2 = _malloc(40000); struct block_header *data2_block = test_get_block_header(data2);
    test_assert_with_message(!data2_block->is_free, "Блок не помечен как занятый\n");
    test_assert_with_message(data2_block->capacity.bytes == 40000, "Capacity не равняется ожидаемому =(\n");
    test_assert_with_message(
        (uint8_t *)data1_block->contents + data1_block->capacity.bytes != (uint8_t *)data2_block, "Блоки выделились рядом");
    debug("Успешно!\n\n");

    test_debug_heap_with_message(heap, "Конечное состояние кучи:");

    _free(data2);
    _free(data1);
}

void run_tests() {
    struct block_header *heap = (struct block_header *)heap_init(10000);
    test_assert_with_message(heap != NULL, "Не удалось инициализировать кучу");

    test_mem_allocation(heap);
    test_free_without_merging(heap);
    test_free_with_merging(heap);
    test_free_two_blocks(heap);
    test_allocate_big_blocks(heap);
    test_allocate_big_blocks_region_in_other_range(heap);
}
