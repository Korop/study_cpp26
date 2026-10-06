#include <chrono>
#include <iostream>
#include <thread>
#include <print>
#include <cassert>

using namespace std::chrono_literals;

void Worker(std::stop_token stopToken)
{
    int iteration{};

    while (!stopToken.stop_requested())
    {
        std::cout << "Work " << iteration++ << '\n';
        std::this_thread::sleep_for(100ms);
    }

    std::cout << "Worker stopped\n";
}

int main()
{
    std::println("Start stop token task");
    std::jthread worker{Worker};

    std::this_thread::sleep_for(500ms);

    worker.request_stop();
    //// stop_source
    std::stop_source source;

    std::stop_token t1 = source.get_token();
    std::stop_token t2 = source.get_token();

    source.request_stop();

    assert(t1.stop_requested()); // true
    t2.stop_requested(); // true

    std::println("End");

    // no explicit join required
}