
#define _GNU_SOURCE

#include "heap_init.h"
#include <sys/mman.h>

static void* g_heap_base = NULL;

void* heap_init(void)
{
    if (g_heap_base != NULL)
    {
        return g_heap_base;
    }

    void* heap = mmap(NULL, HEAP_CHUNK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (heap == MAP_FAILED)
    {
        return NULL;
    }

    g_heap_base = heap;
    return g_heap_base;
}

size_t heap_chunk_size(void)
{
    return HEAP_CHUNK_SIZE;
}
