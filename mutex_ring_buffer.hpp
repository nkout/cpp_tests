#pragma once

#include <mutex>
#include "ring_buffer.hpp"

class MutexRingBuffer: public RingBuffer {
public:
    MutexRingBuffer();
    bool write(const char* data, size_t length) override;
    bool read(size_t max_len, char* data, size_t &length) override;
    ~MutexRingBuffer() override;
private:
    std::mutex m;
    size_t *slots;
    void *mem;

    size_t reader;
    size_t writer;
};

