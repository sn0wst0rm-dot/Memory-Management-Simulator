#ifndef REPLACEMENT_POLICY_H
#define REPLACEMENT_POLICY_H

#include <list>
#include <string>
#include "cache_line.h"

class ReplacementPolicy {
public:
    virtual ~ReplacementPolicy() {}
    virtual void on_hit(std::list<CacheLine>& set, std::list<CacheLine>::iterator it) = 0;
    virtual void on_miss(std::list<CacheLine>& set, const CacheLine& new_line, size_t associativity) = 0;
    virtual std::string name() const = 0;
};

#endif