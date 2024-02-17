#pragma once

#include <mutex>
#include "ring_buffer.hpp"

class AtomicRingBuffer: public RingBuffer {
public:
    AtomicRingBuffer();
    ~AtomicRingBuffer() override;
    bool write(const char* data, size_t length) override;
    bool read(size_t max_len, char* data, size_t &length) override;
private:
    void *mem;
    size_t reader;
    size_t writer;
    std::mutex m;
};

