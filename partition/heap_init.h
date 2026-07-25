#pragma once

#include <stddef.h>

#define HEAP_CHUNK_SIZE (1024u * 1024u * 8u)

size_t heap_chunk_size(void);


void* heap_init(void);





