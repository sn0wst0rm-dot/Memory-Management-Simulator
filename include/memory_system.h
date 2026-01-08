#ifndef MEMORY_SYSTEM_H
#define MEMORY_SYSTEM_H

#include <iostream>
#include "memory_manager.h"
#include "cache_hierarchy.h"
#include "vm/page_table.h"

class MemorySystem {
private:
    MemoryManager* p_mem;
    CacheHierarchy* caches;
    PageTable* vm;

public:
    MemorySystem(MemoryManager* m, CacheHierarchy* c, PageTable* v) 
        : p_mem(m), caches(c), vm(v) {}

    void access(uint32_t virtual_address) {
        std::cout << "[CPU] Request VA: " << virtual_address;
        
        uint32_t physical_address = vm->translate(virtual_address);
        
        std::cout << " -> PA: " << physical_address << " -> ";
        
        int cycles = caches->access(physical_address);

        std::cout << "Done. (Latency: " << cycles << " cycles)" << std::endl;
    }
    
    void print_all_stats() {
        vm->print_stats();
        caches->print_stats();
    }
};

#endif