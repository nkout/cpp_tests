//
// Created by koutsian on 13/2/2024.
//

#include <thread>
#include <cstring>
#include <iostream>
#include "mutex_ring_buffer.hpp"
#include "atomic_ring_buffer.hpp"

#ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
    using std::hardware_destructive_interference_size;
#else
// 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ...
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size = 64;
#endif


inline auto now() noexcept { return std::chrono::high_resolution_clock::now(); }

std::mutex print_m;

void writer(RingBuffer &buffer, int count)
{
    char data[1024];
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    int i;

    for(i=0; i<count; i++)
    {
        memset(data, (char)(i & 0x7F), sizeof(data));
        bool result = false;
        while (!result)
        {
            result = buffer.write(data, sizeof(data));
        }

        if (!i)
            start = now();

    }

    const std::chrono::duration<double, std::milli> elapsed{now() - start};
    {
        std::lock_guard<std::mutex> guard(print_m);
        std::cout << "writer() spent " << elapsed.count() << " ms\n";
    }
}

void reader(RingBuffer &buffer, int &count)
{
    char data[1024];
    int max = count;
    count = 0;

    std::chrono::time_point<std::chrono::high_resolution_clock> start;

    while(count < max) {
        bool result = false;
        size_t len = 0;

        while (!result) {
            result = buffer.read(sizeof(data), data, len);
        }

        if (!count)
            start = now();

        if (len != sizeof(data))
            return;

        int k;
        for (k = 0; k < sizeof(data); k++) {
            if (data[k] != (count & 0x7F))
                return;
        }
        count++;
    }

    const std::chrono::duration<double, std::milli> elapsed{now() - start};
    {
        std::lock_guard<std::mutex> guard(print_m);
        std::cout << "reader() spent " << elapsed.count() << " ms\n";
    }
}

int main(void)
{
    int i;
    for (i=0; i<5; i++) {
        MutexRingBuffer buf;
        AtomicRingBuffer buf2;
        constexpr int tests_num = 1000000;
        int num = tests_num;


        std::thread th_reader(reader, std::ref(buf2), std::ref(num));
        std::thread th_writer(writer, std::ref(buf2), tests_num);

        th_writer.join();
        th_reader.join();

        std::cout << "count " << num << std::endl;
    }
    return 0;
}