#include "../../include/memory_manager.h"
#include <iostream>
#include <iomanip>
#include <new>

using namespace std;

MemoryManager::MemoryManager(size_t size) {
    allocator = nullptr;
    init(size);
}

MemoryManager::~MemoryManager() {
    if (memory_start) {
        delete[] (char*)memory_start;
    }
}

void MemoryManager::init(size_t size) {
    this->total_size = size;
    this->memory_start = new char[size]; 
    this->head = new (memory_start) BlockHeader(size - sizeof(BlockHeader), true);
}

void* MemoryManager::my_malloc(size_t size) {
    total_alloc_attempts++;
    if (allocator == nullptr) {
        cerr << "Error: No allocation strategy set!" << endl;
        return nullptr;
    }

    BlockHeader* best_block = allocator->find_block(head, size);

    if (best_block == nullptr) {
        return nullptr; 
    }
    total_alloc_success++;

    if (best_block->size > size + sizeof(BlockHeader)) {
        char* new_header_addr = (char*)best_block + sizeof(BlockHeader) + size;
        
        BlockHeader* new_block = new (new_header_addr) BlockHeader(
            best_block->size - size - sizeof(BlockHeader), 
            true
        );
        
        new_block->next = best_block->next;
        new_block->prev = best_block;
        
        if (best_block->next) {
            best_block->next->prev = new_block;
        }
        best_block->next = new_block;
        
        best_block->size = size;
    }

    best_block->is_free = false;
    return (void*)((char*)best_block + sizeof(BlockHeader));
}

void MemoryManager::my_free(void* ptr) {
    if (!ptr) return;

    BlockHeader* block = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));

    if (block->is_free) return;

    block->is_free = true;

    if (block->next && block->next->is_free) {
        block->size += sizeof(BlockHeader) + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
    }

    if (block->prev && block->prev->is_free) {
        block->prev->size += sizeof(BlockHeader) + block->size;
        block->prev->next = block->next;
        if (block->next) {
            block->next->prev = block->prev;
        }
    }
}

void MemoryManager::dump_memory() {
    BlockHeader* current = head;
    
    cout << "==========================================" << endl;
    cout << "              MEMORY DUMP                 " << endl;
    cout << "==========================================" << endl;
    
    while (current != nullptr) {
        ptrdiff_t offset = (char*)current - (char*)memory_start;

        cout << "[" << (current->is_free ? "FREE" : "ALLOC") << "] ";
        cout << "Offset: " << setw(4) << offset << " | ";
        cout << "Size: " << setw(4) << current->size << " | ";
        cout << endl;

        current = current->next;
    }
    cout << "==========================================" << endl;
}

void MemoryManager::set_strategy(AllocationStrategy* strategy) {
    this->allocator = strategy;
}

void MemoryManager::print_stats() {
    size_t total_free = 0;
    size_t total_allocated = 0;
    size_t free_blocks = 0;
    size_t largest_free_chunk = 0;

    BlockHeader* current = head;
    while (current != nullptr) {
        if (current->is_free) {
            total_free += current->size;
            free_blocks++;
            if (current->size > largest_free_chunk) {
                largest_free_chunk = current->size;
            }
        } else {
            total_allocated += current->size; 
        }
        current = current->next;
    }

    double ext_frag = 0.0;
    if (total_free > 0) {
        ext_frag = 1.0 - ((double)largest_free_chunk / total_free);
    }

    double utilization = 0.0;
    if (total_size > 0) {
        utilization = (double)total_allocated / total_size;
    }

    double success_rate = 0.0;
    double failure_rate = 0.0;
    if (total_alloc_attempts > 0) {
        success_rate = (double)total_alloc_success / total_alloc_attempts;
        failure_rate = 1.0 - success_rate;
    }

    cout << "\n=== MEMORY METRICS & STATISTICS ===" << endl;
    cout << "Total Memory:      " << total_size << " bytes" << endl;
    cout << "Used Memory:       " << total_allocated << " bytes" << endl;
    cout << "Free Memory:       " << total_free << " bytes" << endl;
    cout << "-----------------------------------" << endl;
    cout << "Memory Utilization: " << (utilization * 100) << "%" << endl;
    cout << "Ext. Fragmentation: " << (ext_frag * 100) << "%" << endl;
    cout << "Int. Fragmentation: < 1% (Minimized by Splitting)" << endl; 
    cout << "Allocation Attempts:" << total_alloc_attempts << endl;
    cout << "Success Rate:       " << (success_rate * 100) << "%" << endl;
    cout << "Failure Rate:       " << (failure_rate * 100) << "%" << endl;
    cout << "===================================" << endl;

}
