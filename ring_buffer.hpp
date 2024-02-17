#pragma once
#include <cstddef>

class RingBuffer {
public:
    virtual ~RingBuffer() = default;
    virtual bool write(const char* data, size_t length) = 0;
    virtual bool read(size_t max_len, char* data, size_t &length) = 0;
};