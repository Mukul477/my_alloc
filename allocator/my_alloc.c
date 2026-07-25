#include "my_alloc.h"

#include "../partition/heap_init.h"

#include <stdint.h>

#define MIN_ORDER 5u
#define MAX_ORDER 23u
#define ORDER_COUNT (MAX_ORDER - MIN_ORDER + 1u)

typedef struct block_header
{
    unsigned int order;
    struct block_header* next;
} block_header;

static block_header* free_lists[ORDER_COUNT] = {0};
static void* heap_base = NULL;

static unsigned int order_to_index(unsigned int order)
{
    return order - MIN_ORDER;
}

static size_t order_to_size(unsigned int order)
{
    return (size_t)1u << order;
}

static void push_free_block(block_header* block)
{
    const unsigned int idx = order_to_index(block->order);
    block->next = free_lists[idx];
    free_lists[idx] = block;
}

static block_header* pop_free_block(unsigned int order)
{
    const unsigned int idx = order_to_index(order);
    block_header* block = free_lists[idx];
    if (block == NULL)
    {
        return NULL;
    }
    free_lists[idx] = block->next;
    block->next = NULL;
    return block;
}

static void allocator_init(void)
{
    if (heap_base != NULL)
    {
        return;
    }

    heap_base = heap_init();
    if (heap_base == NULL)
    {
        return;
    }

    block_header* initial = (block_header*)heap_base;
    initial->order = MAX_ORDER;
    initial->next = NULL;
    free_lists[order_to_index(MAX_ORDER)] = initial;
}

static unsigned int required_order(size_t size)
{
    size_t needed = size + sizeof(block_header);
    unsigned int order = MIN_ORDER;

    while (order <= MAX_ORDER && order_to_size(order) < needed)
    {
        ++order;
    }

    return order;
}

void* my_alloc(size_t size)
{
    if (size == 0u)
    {
        return NULL;
    }

    allocator_init();
    if (heap_base == NULL)
    {
        return NULL;
    }

    unsigned int target_order = required_order(size);
    if (target_order > MAX_ORDER)
    {
        return NULL;
    }

    unsigned int available_order = target_order;
    while (available_order <= MAX_ORDER && free_lists[order_to_index(available_order)] == NULL)
    {
        ++available_order;
    }

    if (available_order > MAX_ORDER)
    {
        return NULL;
    }

    block_header* block = pop_free_block(available_order);
    if (block == NULL)
    {
        return NULL;
    }

    while (available_order > target_order)
    {
        --available_order;

        size_t half_size = order_to_size(available_order);
        block_header* buddy = (block_header*)((char*)block + half_size);
        buddy->order = available_order;
        buddy->next = NULL;
        push_free_block(buddy);

        block->order = available_order;
    }

    return (void*)(block + 1);
}

void my_free(void* ptr)
{
    if (ptr == NULL || heap_base == NULL)
    {
        return;
    }

    block_header* block = ((block_header*)ptr) - 1;
    unsigned int order = block->order;

    while (order < MAX_ORDER)
    {
        size_t block_size = order_to_size(order);
        uintptr_t block_offset = (uintptr_t)((char*)block - (char*)heap_base);
        uintptr_t buddy_offset = block_offset ^ block_size;
        block_header* buddy = (block_header*)((char*)heap_base + buddy_offset);

        block_header** list = &free_lists[order_to_index(order)];
        block_header* prev = NULL;
        block_header* curr = *list;
        int buddy_found = 0;

        while (curr != NULL)
        {
            if (curr == buddy && curr->order == order)
            {
                if (prev == NULL)
                {
                    *list = curr->next;
                }
                else
                {
                    prev->next = curr->next;
                }
                buddy_found = 1;
                break;
            }
            prev = curr;
            curr = curr->next;
        }

        if (!buddy_found)
        {
            break;
        }

        if (buddy < block)
        {
            block = buddy;
        }
        ++order;
        block->order = order;
    }

    block->order = order;
    block->next = NULL;
    push_free_block(block);
}
