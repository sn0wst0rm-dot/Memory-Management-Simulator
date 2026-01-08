#ifndef ALLOCATION_STRATEGY_H
#define ALLOCATION_STRATEGY_H

#include <cstddef>
#include "block.h"

class AllocationStrategy {
public:
    virtual ~AllocationStrategy() {}
    virtual BlockHeader* find_block(BlockHeader* head, size_t size) = 0;
};

#endif