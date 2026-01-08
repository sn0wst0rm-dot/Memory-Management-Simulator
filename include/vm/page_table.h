#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <map>
#include <vector>
#include <iostream>
#include "page_replacement_policy.h"

struct PageEntry {
    bool valid;
    uint32_t frame_number;
    PageEntry() : valid(false), frame_number(0) {}
};

class PageTable {
private:
    size_t page_size;
    size_t num_frames;
    
    std::map<uint32_t, PageEntry> table;
    std::vector<bool> frame_bitmap;     
    
    PageReplacementPolicy* policy;

    size_t faults = 0;
    size_t hits = 0;
    size_t evictions = 0;

public:
    PageTable(size_t page_size_bytes, size_t phys_mem_size, PageReplacementPolicy* p)
        : page_size(page_size_bytes), policy(p) {
        
        num_frames = phys_mem_size / page_size;
        frame_bitmap.resize(num_frames, false);
    }

    void set_policy(PageReplacementPolicy* p) {
        policy = p;
    }

    uint32_t translate(uint32_t virtual_addr) {
        uint32_t vpn = virtual_addr / page_size;
        uint32_t offset = virtual_addr % page_size;

        if (table.count(vpn) && table[vpn].valid) {
            hits++;
            policy->on_hit(vpn);
            return (table[vpn].frame_number * page_size) + offset;
        }

        faults++;
        
        size_t used = 0;
        for(bool b : frame_bitmap) if(b) used++;

        int32_t victim_vpn = policy->on_miss(vpn, num_frames, used);

        if (victim_vpn != -1) {
            evictions++;
            uint32_t recycled_frame = table[victim_vpn].frame_number;
            table[victim_vpn].valid = false;
            
            table[vpn].valid = true;
            table[vpn].frame_number = recycled_frame;
            std::cout << " [Page Fault] Evicted VPN " << victim_vpn << " -> Loaded VPN " << vpn << std::endl;
        } 
        else {
            int free_frame = -1;
            for(size_t i=0; i<num_frames; i++) {
                if (!frame_bitmap[i]) { free_frame = i; break; }
            }
            
            if (free_frame != -1) {
                frame_bitmap[free_frame] = true;
                table[vpn].valid = true;
                table[vpn].frame_number = free_frame;
                std::cout << " [Page Fault] Loaded VPN " << vpn << " into Frame " << free_frame << std::endl;
            } else {
                std::cerr << "CRITICAL ERROR: Policy said no eviction, but no frames free!" << std::endl;
            }
        }

        return (table[vpn].frame_number * page_size) + offset;
    }

    void print_stats() {
        std::cout << "\n=== VIRTUAL MEMORY STATS (" << policy->name() << ") ===" << std::endl;
        std::cout << "Page Hits:   " << hits << std::endl;
        std::cout << "Page Faults: " << faults << std::endl;
        std::cout << "Evictions:   " << evictions << std::endl;
        std::cout << "Hit Rate:    " << (hits+faults > 0 ? (double)hits/(hits+faults)*100.0 : 0) << "%" << std::endl;
    }
};

#endif