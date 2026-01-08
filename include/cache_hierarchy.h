#ifndef CACHE_HIERARCHY_H
#define CACHE_HIERARCHY_H

#include "cache.h"

class CacheHierarchy {
private:
    CacheLevel* l1;
    CacheLevel* l2;
    CacheLevel* l3;
    int memory_latency;

    long long total_cycles = 0;
    long long total_accesses = 0;

public:
    CacheHierarchy(CacheLevel* c1, CacheLevel* c2, CacheLevel* c3, int mem_latency = 200) 
        : l1(c1), l2(c2), l3(c3), memory_latency(mem_latency) {}

    int access(uint32_t address) {
        total_accesses++;
        int current_latency = 0;

        current_latency+= l1->get_latency();
        if (l1->access(address)){
            total_cycles+=current_latency;
            return current_latency;
        }
        
        current_latency+= l2->get_latency();
        if (l2->access(address)){
            total_cycles+=current_latency;
            return current_latency;
        }

        current_latency+=l3->get_latency();
        if (l3) {
            if (l3->access(address)){
                total_cycles+=current_latency;
                return current_latency;
            }
        }

        current_latency+=memory_latency;
        total_cycles+=current_latency;
        return current_latency;
    }

    void print_stats() {
        if (l1) l1->print_stats("L1 Cache");
        if (l2) l2->print_stats("L2 Cache");
        if (l3) l3->print_stats("L3 Cache");

        std::cout << "\n=== PERFORMANCE METRICS ===" << std::endl;
        std::cout << "Total Accesses: " << total_accesses << std::endl;
        std::cout << "Total Cycles:   " << total_cycles << std::endl;
        
        double amat = (total_accesses > 0) ? (double)total_cycles / total_accesses : 0.0;
        std::cout << "Avg Access Time (AMAT): " << amat << " cycles" << std::endl;
    }
};

#endif