#include <iostream>
#include <thread>
#include <random>
#include <print>
#include <chrono>


thread_local int counter = 0;

void work()
{
    ++counter;
    ++counter;
    std::println("thread={}, counter={}", std::this_thread::get_id(), counter);
    // std::cout
    //     << "thread = " << std::this_thread::get_id()
    //     << ", counter = " << counter
    //     << '\n';
}

int random_value()
{
    thread_local std::mt19937 generator{
        std::random_device{}()
    };

    std::uniform_int_distribution<int> distribution{1, 1000};

    return distribution(generator);
}

thread_local int localThreadId = 0;
void print_10_randoms_work(int id) {    
    localThreadId = id;
    for (size_t i = 0; i < 10; i++)
    {
        auto rv = random_value();
        std::println("ThreadId={}: {}, random value[{}]={}", std::this_thread::get_id(), localThreadId, i, rv);
        std::this_thread::sleep_for(std::chrono::milliseconds(rv));
    }
}
///
// But thread_local doesn't magically give you the total. You still need an aggregation mechanism.
thread_local std::uint64_t local_counter = 0;
std::atomic<std::uint64_t> counterAtomic = 0;

void process_atomic()
{
    counterAtomic.fetch_add(1);
}
void process_local()
{
    ++local_counter;
}
////
struct ParserCache
{
    // expensive structures
};

ParserCache& get_cache()
{
    thread_local ParserCache cache;
    return cache;
}

void parse()
{
    auto& cache = get_cache();

    // safely use this thread's cache
}
///
int main()
{
    std::thread t1(work);
    std::thread t2(work);

    t1.join();
    t2.join();

    std::vector<std::thread> threads;
    std::thread randomThread1(print_10_randoms_work, 1);
    // Помилка виникає тому, що клас std::thread є move-only типом. Його не можна копіювати, а можна лише переміщувати (move).
    // threads.push_back(randomThread1); // compiler error
    threads.push_back(std::move(randomThread1));
    std::thread randomThread2(print_10_randoms_work, 2);
    threads.push_back(std::move(randomThread2));    
    threads.emplace_back(print_10_randoms_work, 3);

    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

}