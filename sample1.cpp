#include <thread>
#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>

int64_t
not_atomic(int64_t num_threads, int64_t add_count) {
    std::chrono::system_clock::time_point start, end;
    std::int64_t sum(0);
    auto f = [&](size_t count) {
        for (size_t i = 0; i < count; ++i) {
            sum++;
        }
    };

    std::vector<std::thread> threads{};
    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back(f, add_count);
    }
    start = std::chrono::system_clock::now();
    for (auto &&t : threads) t.join();
    end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    
    std::cout << "- not using atomic variables" << std::endl;
    std::cout << "value: " << sum << ", "; // 11827319
    std::cout << "elapsed time: " << elapsed << " [ms]" << std::endl; // 451 [ms]

    return 0;
}

int64_t
use_atomic_1(int64_t num_threads, int64_t add_count) {
    std::chrono::system_clock::time_point start, end;
    std::atomic_int64_t sum(0);

    auto f = [&](size_t count) {
        for (size_t i = 0; i < count; ++i) {
            sum++;
        }
    };

    std::vector<std::thread> threads{};
    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back(f, add_count);
    }
    start = std::chrono::system_clock::now();
    for (auto &&t : threads) t.join();
    end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    
    std::cout << "- using atomic variables" << std::endl;
    std::cout << "value: " << sum << ", "; // 80000000
    std::cout << "elapsed time: " << elapsed << " [ms]" << std::endl; // 2812 [ms]

    return 0;
}

int64_t
use_atomic_2(int64_t num_threads, int64_t add_count) {
    std::chrono::system_clock::time_point start, end;
    std::atomic_int64_t sum(0);

    auto f = [&](size_t count) {
        for (size_t i = 0; i < count; ++i) {
            auto cur_val = sum.load();
            cur_val++;
            sum.store(cur_val);
        }
    };

    std::vector<std::thread> threads{};
    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back(f, add_count);
    }
    start = std::chrono::system_clock::now();
    for (auto &&t : threads) t.join();
    end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    
    std::cout << "- using atomic variables and assign to a temporary variable" << std::endl;
    std::cout << "value: " << sum << ", "; // 20129319
    std::cout << "elapsed time: " << elapsed << " [ms]" << std::endl; // 3459 [ms]

    return 0;
}

int64_t
use_cas_weak(int64_t num_threads, int64_t add_count) {
    std::chrono::system_clock::time_point start, end;
    std::atomic_int64_t sum(0);

    auto f = [&](size_t count) {
        for (size_t i = 0; i < count; ++i) {
            auto cur_val = sum.load();
            while (!sum.compare_exchange_weak(cur_val, cur_val + 1)) {
            }
        }
    };

    std::vector<std::thread> threads{};
    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back(f, add_count);
    }
    start = std::chrono::system_clock::now();
    for (auto &&t : threads) t.join();
    end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    
    std::cout << "- using cas(weak)" << std::endl;
    std::cout << "value: " << sum << ", "; // 80000000
    std::cout << "elapsed time: " << elapsed << " [ms]" << std::endl; // 19011 [ms]

    return 0;
}

int64_t
use_cas_strong(int64_t num_threads, int64_t add_count) {
    std::chrono::system_clock::time_point start, end;
    std::atomic_int64_t sum(0);

    auto f = [&](size_t count) {
        for (size_t i = 0; i < count; ++i) {
            auto cur_val = sum.load();
            while (!sum.compare_exchange_strong(cur_val, cur_val + 1)) {
            }
        }
    };

    std::vector<std::thread> threads{};
    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back(f, add_count);
    }
    start = std::chrono::system_clock::now();
    for (auto &&t : threads) t.join();
    end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    
    std::cout << "- using cas(strong)" << std::endl;
    std::cout << "value: " << sum << ", "; // 80000000
    std::cout << "elapsed time: " << elapsed << " [ms]" << std::endl; // 19813 [ms]

    return 0;
}

int64_t
use_fetch_add(int64_t num_threads, int64_t add_count) {
    std::chrono::system_clock::time_point start, end;
    std::atomic_int64_t sum(0);

    auto f = [&](size_t count) {
        for (size_t i = 0; i < count; ++i) {
            sum.fetch_add(1);
        }
    };

    std::vector<std::thread> threads{};
    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back(f, add_count);
    }
    start = std::chrono::system_clock::now();
    for (auto &&t : threads) t.join();
    end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    
    std::cout << "- using atomic fetch add" << std::endl;
    std::cout << "value: " << sum << ", "; // 80000000
    std::cout << "elapsed time: " << elapsed << " [ms]" << std::endl; // 3170 [ms]

    return 0;
}

int
main() {
    std::cout << "=== multi thread add test ===" << std::endl;

    std::int32_t num_threads = 8;
    std::int64_t add_count = 1e7;

    std::cout << "expect value: " << num_threads * add_count << std::endl;
    std::cout << std::endl;

    not_atomic(num_threads, add_count);
    use_atomic_1(num_threads, add_count);
    use_atomic_2(num_threads, add_count);
    use_cas_weak(num_threads, add_count);
    use_cas_strong(num_threads, add_count);
    use_fetch_add(num_threads, add_count);
}
