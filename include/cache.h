#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <list>
#include <string>
#include <cstdint>
#include <iostream>
#include "cache_line.h"      
#include "replacement_policy.h" 

class CacheLevel {
private:
    size_t size;
    size_t block_size;
    size_t associativity;
    ReplacementPolicy* policy; 
    size_t num_sets;
    size_t hits = 0;
    size_t misses = 0;
    std::vector<std::list<CacheLine>> sets;
    int latency_cycles;

public:
    CacheLevel(size_t size, size_t block_size, size_t associativity, ReplacementPolicy* p, int latency);
    bool access(uint32_t address);
    void print_stats(std::string level_name);
    void set_policy(ReplacementPolicy* p);
    int get_latency() const{return latency_cycles;}
};

#endif