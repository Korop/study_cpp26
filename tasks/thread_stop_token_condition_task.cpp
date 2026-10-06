#include <chrono>
#include <condition_variable>
#include <mutex>
#include <print>
#include <queue>
#include <stop_token>
#include <thread>

using namespace std::chrono_literals;

// Shared state
std::queue<int> jobs;
std::mutex jobsMutex;
std::condition_variable_any jobsCv;

void Worker(std::stop_token token)
{
    while (true)
    {
        // Lock shared state before touching jobs.
        // [1] Worker locks jobsMutex
        std::unique_lock lock{jobsMutex};

        // Wait until:
        //   1. jobs is not empty
        // OR
        //   2. stop is requested
        //
        // While waiting, jobsMutex is UNLOCKED.
        // Before wait() returns, jobsMutex is LOCKED again.
        const bool hasJob = jobsCv.wait(
            lock,
            token,
            []
            {
                return !jobs.empty();
            });

        // false means:
        // stop was requested and predicate is still false.
        if (!hasJob || token.stop_requested())
        {
            std::println("[Worker] Stop requested");
            return;
        }

        // We still own jobsMutex here.
        const int job = jobs.front();
        jobs.pop();

        // We do NOT want to keep the mutex while doing actual work.
        lock.unlock();

        std::println("[Worker] Processing job {}", job);

        // Simulate expensive work.
        std::this_thread::sleep_for(300ms);

        std::println("[Worker] Finished job {}", job);
    }
}

int main()
{
    std::println("[Main] Starting worker");

    std::jthread worker{Worker};

    // Main thread acts as the producer.
    for (int job = 1; job <= 10; ++job)
    {
        {
            // Same mutex the Worker uses.
            std::lock_guard lock{jobsMutex};

            jobs.push(job);

            std::println("[Main] Added job {}", job);
        } // jobsMutex unlocked here

        // Wake one waiting consumer.
        jobsCv.notify_one();

        std::this_thread::sleep_for(100ms);
    }

    std::println("[Main] Producer finished");

    // Give worker some time to process jobs.
    std::this_thread::sleep_for(10000ms);

    std::println("[Main] Requesting stop");

    worker.request_stop();

    // Explicit join only to make this example's output/order obvious.
    // std::jthread would join automatically in its destructor.
    worker.join();

    std::println("[Main] Worker joined, exiting");
}
