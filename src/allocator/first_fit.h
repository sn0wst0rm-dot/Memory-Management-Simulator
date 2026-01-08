#ifndef FIRST_FIT_H
#define FIRST_FIT_H
#include "../../include/allocation_strategy.h"

class FirstFit : public AllocationStrategy {
public:
    BlockHeader* find_block(BlockHeader* head, size_t size) override;
};
#endif