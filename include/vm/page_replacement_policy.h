#ifndef PAGE_REPLACEMENT_POLICY_H
#define PAGE_REPLACEMENT_POLICY_H

#include <list>
#include <string>
#include <cstdint>
#include <iostream>

class PageReplacementPolicy {
public:
    virtual ~PageReplacementPolicy() {}

    virtual void on_hit(uint32_t vpn) = 0;

    virtual int32_t on_miss(uint32_t new_vpn, size_t max_frames, size_t current_used) = 0;
    
    virtual std::string name() const = 0;
};

#endif