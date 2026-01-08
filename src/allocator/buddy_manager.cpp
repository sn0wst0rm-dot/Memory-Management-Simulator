#include "../../include/buddy_manager.h"
#include <iostream>
#include <new>
#include <iomanip>

using namespace std;

BuddyManager::BuddyManager(size_t size, size_t min_size) {
    this->total_size = next_power_of_2(size);
    this->min_block_size = min_size;
    this->memory_start = new char[this->total_size];

    int max_order = (int)log2(this->total_size);
    free_lists.resize(max_order + 1, nullptr);

    BlockHeader* initial_block = new (memory_start) BlockHeader(this->total_size - sizeof(BlockHeader), true);
    
    free_lists[max_order] = initial_block;
    initial_block->next = nullptr;
    initial_block->prev = nullptr;
}

BuddyManager::~BuddyManager() {
    delete[] (char*)memory_start;
}

int BuddyManager::get_order(size_t size) {
    return (int)ceil(log2(size));
}

size_t BuddyManager::next_power_of_2(size_t size) {
    return pow(2, ceil(log2(size)));
}

void* BuddyManager::alloc(size_t size) {
    size_t total_req = size + sizeof(BlockHeader);
    
    if (total_req < min_block_size) total_req = min_block_size;
    total_req = next_power_of_2(total_req);

    int order = get_order(total_req);
    int max_order = free_lists.size() - 1;

    int current_order = order;
    while (current_order <= max_order && free_lists[current_order] == nullptr) {
        current_order++;
    }

    if (current_order > max_order) {
        return nullptr;
    }

    BlockHeader* block = free_lists[current_order];
    
    if (block->next) block->next->prev = nullptr;
    free_lists[current_order] = block->next;

    while (current_order > order) {
        current_order--;

        size_t new_size = (size_t)1 << current_order; 

        BlockHeader* buddy = (BlockHeader*)((char*)block + new_size);
        
        new (buddy) BlockHeader(new_size - sizeof(BlockHeader), true);
        
        buddy->next = free_lists[current_order];
        if (free_lists[current_order]) free_lists[current_order]->prev = buddy;
        free_lists[current_order] = buddy;
        
        block->size = new_size - sizeof(BlockHeader);
    }

    block->is_free = false;
    return (void*)((char*)block + sizeof(BlockHeader));
}

void BuddyManager::free(void* ptr) {
    if (!ptr) return;
    
    BlockHeader* block = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
    if (block->is_free) return;

    block->is_free = true;
    
    size_t block_total_size = block->size + sizeof(BlockHeader);
    
    long offset = (char*)block - (char*)memory_start;

    while (block_total_size < total_size) {
        long buddy_offset = offset ^ block_total_size;
        BlockHeader* buddy = (BlockHeader*)((char*)memory_start + buddy_offset);

        size_t buddy_internal_size = block_total_size - sizeof(BlockHeader);
        
        if (!buddy->is_free || buddy->size != buddy_internal_size) {
            break;
        }

        int order = get_order(block_total_size);
        if (buddy->prev) buddy->prev->next = buddy->next;
        if (buddy->next) buddy->next->prev = buddy->prev;
        if (free_lists[order] == buddy) free_lists[order] = buddy->next;

        if (buddy_offset < offset) {
            block = buddy;
            offset = buddy_offset;
        }

        block_total_size *= 2;
        block->size = block_total_size - sizeof(BlockHeader);
        
    }

    int order = get_order(block_total_size);
    block->next = free_lists[order];
    if (free_lists[order]) free_lists[order]->prev = block;
    free_lists[order] = block;
    block->prev = nullptr;
}

void BuddyManager::dump_memory() {
    cout << "=== BUDDY SYSTEM DUMP ===" << endl;
    for (size_t i = 0; i < free_lists.size(); i++) {
        size_t size = (size_t)1 << i;
        if (size < min_block_size) continue;

        cout << "Order " << i << " (" << size << " bytes): ";
        BlockHeader* curr = free_lists[i];
        if (!curr) {
            cout << "[Empty]" << endl;
        } else {
            while (curr) {
                cout << "[Free @" << ((char*)curr - (char*)memory_start) << "] -> ";
                curr = curr->next;
            }
            cout << "NULL" << endl;
        }
    }
    cout << "=========================" << endl;
}

void BuddyManager::print_stats() {
    size_t total_free = 0;
    size_t free_blocks_count = 0;

    for (size_t i = 0; i < free_lists.size(); i++) {
        BlockHeader* curr = free_lists[i];
        size_t block_size = (size_t)1 << i;
        
        if (block_size < min_block_size) continue;

        while (curr != nullptr) {
            total_free += block_size;
            free_blocks_count++;
            curr = curr->next;
        }
    }

    size_t total_allocated = total_size - total_free;
    double utilization = (total_size > 0) ? (double)total_allocated / total_size : 0.0;

    std::cout << "\n=== BUDDY SYSTEM STATS ===" << std::endl;
    std::cout << "Total Memory:    " << total_size << " bytes" << endl;
    std::cout << "Used Memory:     " << total_allocated << " bytes" << endl;
    std::cout << "Free Memory:     " << total_free << " bytes" << endl;
    std::cout << "Free Blocks:     " << free_blocks_count << endl;
    std::cout << "Utilization:     " << (utilization * 100) << "%" << endl;
    std::cout << "==========================" << std::endl;

}
