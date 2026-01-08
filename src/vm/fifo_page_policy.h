#ifndef FIFO_PAGE_POLICY_H
#define FIFO_PAGE_POLICY_H

#include "../../include/vm/page_replacement_policy.h"
#include <deque>

class FIFO_PagePolicy : public PageReplacementPolicy {
private:
    std::deque<uint32_t> queue;

public:
    void on_hit(uint32_t vpn) override {
        
    }

    int32_t on_miss(uint32_t new_vpn, size_t max_frames, size_t current_used) override {
        int32_t victim = -1;

        if (queue.size() >= max_frames) {
            victim = queue.front();
            queue.pop_front();
        }

        queue.push_back(new_vpn);
        
        return victim;
    }

    std::string name() const override { return "FIFO"; }
};

#endif