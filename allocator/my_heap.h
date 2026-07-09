#pragma once 

#define MY_HEAP_SIZE 1024 * 1024 * 10 // 10 MB
#define MY_BITMAP_SIZE (MY_HEAP_SIZE / 8) // 1 bit per byte
#define MY_BLOCK_SIZE 8 // 8 bytes per block

#include <stddef.h>

typedef struct Free_list {
    size_t size;
    struct Free_list* next;
} Free_list;

void* my_heap(size_t size);

void* my_bitmap(void);
