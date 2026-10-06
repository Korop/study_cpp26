#include <iostream>
#include <vector>
#include <string>
#include <print>
#include <thread>
#include <functional> 
#include <cassert>
#include <atomic>


void Increment(int& value)
{
    ++value;
}
///
template<class T>
class ReferenceWrapper
{
    T* ptr;

public:
    ReferenceWrapper(T& value)
        : ptr(&value)
    {
    }

    operator T&() const
    {
        return *ptr;
    }
};
///


int counterBad{0};
std::atomic<int> counterAtomic2{0};

void IncrementBad()
{
    for (int i = 0; i < 100'000; ++i)
    {
        ++counterBad;
        ++counterAtomic2;
    }
}

// A simple C++23/26 compatible file
int main() {

    int x{10};

    std::thread thread{
        Increment,
        // x //will not compile, std::thread support only copy/move value but Increment requires reference int&
        std::ref(x) //return std::reference_wrapper<int>, not int&.
        // std::reference_wrapper is a normal copyable object that represents a reference to another object
        // And reference_wrapper<int> is convertible back to:
    };


    thread.join();

    std::println("x={}", x);


    std::thread threadLambda{ [&x] { Increment(x); } };
    threadLambda.join();
    std::println("x={}", x);

    int y{20};
    std::thread threadLambda2{
        [&x, &y]
        {
            Increment(x);
            Increment(y);
        }
    };
    threadLambda2.join();
    std::println("x={}, y={}", x, y);

    ///Race condition
    {
        std::jthread a{IncrementBad};
        std::jthread b{IncrementBad};
    };
    std::println("counterBad={}", counterBad);
    assert(counterBad != 200000);
    std::println("counterAtomic2={}", counterAtomic2.load());
    assert(counterAtomic2 == 200000);
    
    std::println("");
    return 0;
}
