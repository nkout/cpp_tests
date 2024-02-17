#include <cstring>
#include "mutex_ring_buffer.hpp"

static constexpr unsigned int slots_num = 256;
static constexpr unsigned int slot_size = 4*1024;

struct RingBufferSlot
{
    char data[slot_size];
};

bool MutexRingBuffer::write(const char *data, size_t length) {
    std::lock_guard<std::mutex> guard(this->m);

    if (((slots_num + this->reader - this->writer) % slots_num) == 1)
        return false;

    RingBufferSlot * dest = static_cast<RingBufferSlot*>(this->mem) + writer;

    memcpy(dest->data, data, length);
    this->slots[writer] = length;

    this->writer = (this->writer + 1) % slots_num;
    return true;
}

bool MutexRingBuffer::read(size_t max_len, char *data, size_t &length) {
    std::lock_guard<std::mutex> guard(this->m);

    if (this->reader == this->writer)
        return false;

    RingBufferSlot * source = static_cast<RingBufferSlot*>(this->mem) + reader;

    memcpy(data, source->data, this->slots[reader]);
    length = this->slots[reader];
    this->reader = (this->reader + 1) % slots_num;
    return true;
}

MutexRingBuffer::MutexRingBuffer(): reader(0), writer(0), m() {
    this->slots = static_cast<size_t*>(calloc(slots_num, sizeof(size_t)));
    this->mem = calloc(slots_num, sizeof(RingBufferSlot));
}

MutexRingBuffer::~MutexRingBuffer() {
    free(this->slots);
    free(this->mem);
}
