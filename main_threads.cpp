#include <atomic>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <new>
#include <thread>

#ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
    using std::hardware_destructive_interference_size;
#else
// 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ...
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

std::mutex cout_mutex;

constexpr int max_write_iterations{10'000'000}; // the benchmark time tuning

struct alignas(hardware_constructive_interference_size)
OneCacheLiner // occupies one cache line
{
    std::atomic_uint64_t x{};
    std::atomic_uint64_t y{};
    std::atomic_uint64_t z{};
    std::atomic_uint64_t o{};
};

struct TwoCacheLiner // occupies two cache lines
{
    alignas(hardware_destructive_interference_size) std::atomic_uint64_t x{};
    alignas(hardware_destructive_interference_size) std::atomic_uint64_t y{};
    alignas(hardware_destructive_interference_size) std::atomic_uint64_t z{};
    alignas(hardware_destructive_interference_size) std::atomic_uint64_t o{};
};

inline auto now() noexcept { return std::chrono::high_resolution_clock::now(); }

template<int field_idx, typename T>
void cacheLinerThread(T& t)
{
    const auto start{now()};
    uint64_t value;

    for (uint64_t count = 1; count <= max_write_iterations; ++count) {
        if constexpr (field_idx == 0)
            value = t.x.fetch_add(1, std::memory_order_relaxed);
        else if constexpr (field_idx == 1)
            value = t.y.fetch_add(1, std::memory_order_relaxed);
        else if constexpr (field_idx == 2)
            value = t.z.fetch_add(1, std::memory_order_relaxed);
        else if constexpr (field_idx == 3)
            value = t.o.fetch_add(1, std::memory_order_relaxed);

        if (value != (count -1))
        {
            std::cout << "value " << value << std::endl;
        }
    }

    if constexpr (field_idx == 0)
        value = t.x.load(std::memory_order_acquire);
    else if constexpr (field_idx == 1)
        value = t.y.load(std::memory_order_acquire);
    else if constexpr (field_idx == 2)
        value = t.z.load(std::memory_order_acquire);
    else if constexpr (field_idx == 3)
        value = t.o.load(std::memory_order_acquire);

    if (value != max_write_iterations)
    {
        std::cout << "value " << value << std::endl;
    }

    const std::chrono::duration<double, std::milli> elapsed{now() - start};
    std::lock_guard lk{cout_mutex};
    std::cout << "cacheLinerThread() spent " << elapsed.count() << " ms\n";

    if constexpr (field_idx == 0)
        t.x = elapsed.count();
    else if constexpr (field_idx == 1)
        t.y = elapsed.count();
    else if constexpr (field_idx == 2)
        t.z = elapsed.count();
    else if constexpr (field_idx == 3)
        t.o = elapsed.count();
}

int main()
{
    std::cout << "__cpp_lib_hardware_interference_size "
                 #   ifdef __cpp_lib_hardware_interference_size
                 "= " << __cpp_lib_hardware_interference_size << '\n';
                 #   else
                 "is not defined, use " << hardware_destructive_interference_size
              << " as fallback\n";
#   endif

    std::cout << "hardware_destructive_interference_size == "
              << hardware_destructive_interference_size << '\n'
              << "hardware_constructive_interference_size == "
              << hardware_constructive_interference_size << "\n\n"
              << std::fixed << std::setprecision(2)
              << "sizeof( OneCacheLiner ) == " << sizeof(OneCacheLiner) << '\n'
              << "sizeof( TwoCacheLiner ) == " << sizeof(TwoCacheLiner) << "\n\n";

    constexpr int max_runs{4};
    int oneCacheLiner_average{0};
    int twoCacheLiner_average{0};

    for (auto i{0}; i != max_runs; ++i)
    {
        OneCacheLiner oneCacheLiner{};
        std::thread th1{cacheLinerThread<0,OneCacheLiner>,std::ref(oneCacheLiner)};
        std::thread th2{cacheLinerThread<1,OneCacheLiner>,std::ref(oneCacheLiner)};
        std::thread th3{cacheLinerThread<2,OneCacheLiner>,std::ref(oneCacheLiner)};
        std::thread th4{cacheLinerThread<3,OneCacheLiner>,std::ref(oneCacheLiner)};
        th1.join();
        th2.join();
        th3.join();
        th4.join();
        oneCacheLiner_average += oneCacheLiner.x + oneCacheLiner.y;
        oneCacheLiner_average += oneCacheLiner.o + oneCacheLiner.z;
    }
    std::cout << "Average T1 time: "
              << (oneCacheLiner_average / max_runs / 4) << " ms\n\n";

    for (auto i{0}; i != max_runs; ++i)
    {
        TwoCacheLiner twoCacheLiner{};
        std::thread th1{cacheLinerThread<0,TwoCacheLiner>,std::ref(twoCacheLiner)};
        std::thread th2{cacheLinerThread<1,TwoCacheLiner>,std::ref(twoCacheLiner)};
        std::thread th3{cacheLinerThread<2,TwoCacheLiner>,std::ref(twoCacheLiner)};
        std::thread th4{cacheLinerThread<3,TwoCacheLiner>,std::ref(twoCacheLiner)};
        th1.join();
        th2.join();
        th3.join();
        th4.join();
        twoCacheLiner_average += twoCacheLiner.x + twoCacheLiner.y;
        twoCacheLiner_average += twoCacheLiner.z + twoCacheLiner.o;
    }
    std::cout << "Average T2 time: "
              << (twoCacheLiner_average / max_runs / 4) << " ms\n\n"
              << "Ratio T1/T2:~ "
              << 1.0 * oneCacheLiner_average / twoCacheLiner_average << '\n' ;
}