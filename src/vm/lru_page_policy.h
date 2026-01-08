#ifndef LRU_PAGE_POLICY_H
#define LRU_PAGE_POLICY_H

#include "../../include/vm/page_replacement_policy.h"
#include <list>
#include <algorithm>

class LRU_PagePolicy : public PageReplacementPolicy {
private:
    std::list<uint32_t> lru_list;

public:
    void on_hit(uint32_t vpn) override {
        auto it = std::find(lru_list.begin(), lru_list.end(), vpn);
        if (it != lru_list.end()) {
            lru_list.erase(it);
            lru_list.push_front(vpn);
        }
    }

    int32_t on_miss(uint32_t new_vpn, size_t max_frames, size_t current_used) override {
        int32_t victim = -1;

        if (lru_list.size() >= max_frames) {
            victim = lru_list.back();
            lru_list.pop_back();
        }

        lru_list.push_front(new_vpn);
        return victim;
    }

    std::string name() const override { return "LRU"; }
};

#endif