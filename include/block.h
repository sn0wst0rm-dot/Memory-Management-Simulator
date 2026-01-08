#ifndef BLOCK_H
#define BLOCK_H

#include <cstddef>

struct BlockHeader {
    size_t size;
    bool is_free;
    BlockHeader* next;
    BlockHeader* prev;

    BlockHeader(size_t s, bool f = true)
        : size(s), is_free(f), next(nullptr), prev(nullptr) {}
};

#endif