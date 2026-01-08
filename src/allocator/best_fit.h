#ifndef BEST_FIT_H
#define BEST_FIT_H
#include "../../include/allocation_strategy.h"

class BestFit : public AllocationStrategy {
public:
    BlockHeader* find_block(BlockHeader* head, size_t size) override;
};
#endif