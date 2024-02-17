#include <atomic>
#include <cstring>
#include <stdlib.h>
#include "atomic_ring_buffer.hpp"

#ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
    using std::hardware_destructive_interference_size;
#else
// 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ...
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

#define STATE_FREE 0
#define STATE_RESERVED_FOR_WRITE 1
#define STATE_READY_FOR_READ 2
#define STATE_RESERVED_FOR_READ 3

static constexpr unsigned int slots_num = 256;
static constexpr unsigned int slot_size = 4*1024;

struct RingBufferSlot {
    alignas(hardware_destructive_interference_size) std::atomic<unsigned char> state;
    alignas(hardware_destructive_interference_size) unsigned int length;
    alignas(hardware_destructive_interference_size) char data[slot_size];
};

bool AtomicRingBuffer::write(const char* data, size_t length)
{
    RingBufferSlot *dest;

    {
        std::lock_guard<std::mutex> guard(this->m);

        if (((slots_num + this->reader - this->writer) % slots_num) == 1)
            return false;

        dest = static_cast<RingBufferSlot *>(this->mem) + writer;
        this->writer = (this->writer + 1) % slots_num;
    }

    unsigned char currState = STATE_FREE;
    while (!dest->state.compare_exchange_weak(currState, STATE_RESERVED_FOR_WRITE,
                                       std::memory_order_acquire,
                                       std::memory_order_relaxed))
        currState = STATE_FREE;

    memcpy(dest->data, data, length);
    dest->length = length;

    currState = STATE_RESERVED_FOR_WRITE;
    while (!dest->state.compare_exchange_weak(currState, STATE_READY_FOR_READ,
                                              std::memory_order_release,
                                              std::memory_order_relaxed))
        currState = STATE_RESERVED_FOR_WRITE;

    return true;
}

bool AtomicRingBuffer::read(size_t max_len, char* data, size_t &length)
{
    RingBufferSlot * source;
    {
        std::lock_guard<std::mutex> guard(this->m);

        if (this->reader == this->writer)
            return false;

        source = static_cast<RingBufferSlot *>(this->mem) + reader;
        this->reader = (this->reader + 1) % slots_num;
    }

    unsigned char currState = STATE_READY_FOR_READ;
    while (!source->state.compare_exchange_weak(currState, STATE_RESERVED_FOR_READ,
                                              std::memory_order_acquire,
                                              std::memory_order_relaxed))
        currState = STATE_READY_FOR_READ;

    memcpy(data, source->data, source->length);
    length = source->length;
    source->length = 0;

    currState = STATE_RESERVED_FOR_READ;
    while (!source->state.compare_exchange_weak(currState, STATE_FREE,
                                              std::memory_order_release,
                                              std::memory_order_relaxed))
        currState = STATE_RESERVED_FOR_READ;

    return true;
}

AtomicRingBuffer::AtomicRingBuffer(): reader(0), writer(0), m()
{
    this->mem = calloc(slots_num, sizeof(RingBufferSlot));
}

AtomicRingBuffer::~AtomicRingBuffer()
{
    free(this->mem);
}