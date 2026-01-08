#ifndef FIFO_POLICY_H
#define FIFO_POLICY_H
#include "../../include/replacement_policy.h"

class FIFO_Policy : public ReplacementPolicy {
public:
    void on_hit(std::list<CacheLine>& set, std::list<CacheLine>::iterator it) override {
    }

    void on_miss(std::list<CacheLine>& set, const CacheLine& new_line, size_t associativity) override {
        if (set.size() >= associativity) {
            set.pop_back();
        }
        set.push_front(new_line);
    }
    
    std::string name() const override { return "FIFO"; }
};
#endif