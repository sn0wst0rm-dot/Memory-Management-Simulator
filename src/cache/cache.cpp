#include "../../include/cache.h"

CacheLevel::CacheLevel(size_t size, size_t block_size, size_t associativity, ReplacementPolicy* p, int latency)
    : size(size), block_size(block_size), associativity(associativity), policy(p), latency_cycles(latency) {
    
    num_sets = size / (block_size * associativity);
    sets.resize(num_sets);
}

bool CacheLevel::access(uint32_t address) {
    uint32_t block_addr = address / block_size;
    uint32_t index = block_addr % num_sets;
    uint32_t tag = block_addr / num_sets;

    auto& set = sets[index];

    for (auto it = set.begin(); it != set.end(); ++it) {
        if (it->valid && it->tag == tag) {
            hits++;
            policy->on_hit(set, it);
            return true;
        }
    }

    misses++;
    
    CacheLine new_line;
    new_line.valid = true;
    new_line.tag = tag;

    policy->on_miss(set, new_line, associativity);

    return false;
}
void CacheLevel::set_policy(ReplacementPolicy* p) {
    this->policy = p;
}
void CacheLevel::print_stats(std::string level_name) {
    std::cout << "=== " << level_name << " Stats (" << policy->name() << ") ===" << std::endl;
    double total = hits + misses;
    double rate = (total > 0) ? (double)hits / total : 0.0;
    
    std::cout << "Hits:       " << hits << std::endl;
    std::cout << "Misses:     " << misses << std::endl;
    std::cout << "Hit Rate:   " << rate * 100 << "%" << std::endl;
}