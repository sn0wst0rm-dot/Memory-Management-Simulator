#ifndef CACHE_LINE_H
#define CACHE_LINE_H

#include <cstdint>

struct CacheLine {
    bool valid;
    uint32_t tag;
    
    CacheLine() : valid(false), tag(0) {}
};

#endif