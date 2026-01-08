#ifndef BUDDY_MANAGER_H
#define BUDDY_MANAGER_H

#include <vector>
#include <cmath>
#include <cstddef>
#include "block.h"

class BuddyManager {
private:
    void* memory_start;
    size_t total_size;
    size_t min_block_size;
    
    std::vector<BlockHeader*> free_lists;

    int get_order(size_t size);
    
    size_t next_power_of_2(size_t size);

public:
    BuddyManager(size_t size, size_t min_size = 32);
    ~BuddyManager();

    void* alloc(size_t size);
    void free(void* ptr);
    void dump_memory();

public:
    void print_stats();
};

#endif