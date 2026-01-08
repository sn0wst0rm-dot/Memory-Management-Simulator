#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <cstddef>
#include "block.h"
#include "allocation_strategy.h"

class MemoryManager {
private:
    void* memory_start;       
    size_t total_size;        
    BlockHeader* head;       

    AllocationStrategy* allocator;

    size_t total_alloc_attempts = 0;
    size_t total_alloc_success = 0;

public:
    MemoryManager(size_t size);
    ~MemoryManager();

    void init(size_t size);
    void* my_malloc(size_t size);
    void my_free(void* ptr);
    void dump_memory();      

    void set_strategy(AllocationStrategy* strategy);
    void print_stats();
};

#endif 