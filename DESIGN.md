# Memory Allocator Design

## Goal

The overall goal of this project is to improve my problem-solving skills and my ability to make coherent design decisions while exploring my interest in low-level systems programming.

The allocator is being developed through independent research of existing documentation and allocator designs. The purpose is not to reproduce an existing implementation, but to understand the reasoning behind different approaches and make my own design decisions based on the trade-offs I encounter while building the allocator.

### Decision 1 Switched to Lazy initialization

Initially, I planned to pre-partition the heap into fixed-size blocks for O(1) allocation. However, this approach reserved memory for unused size classes, complicated block coalescing, and lost its performance advantage after the first allocation of each size class.

I decided to switch to a lazy initialization approach instead. The heap starts as a single large free block and is split into smaller buddy blocks only when allocation requests arrive. This preserves the buddy allocator's invariants, simplifies merging, and makes memory usage more efficient.

### Decision 2 Single 8 MiB heap chunk + buddy splitting

`heap_init` now returns one contiguous 8 MiB heap chunk. The live allocator (`allocator/my_alloc.c`) treats that chunk as a single max-order buddy block and, on each `my_alloc(size)` request, repeatedly splits blocks in half until it reaches the smallest power-of-two block that can hold `size + metadata`.
