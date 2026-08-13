#include <iostream>
#include <thread>
#include <vector>

void worker(int id) {
    std::cout << "Worker " << id << " is running.\n";
}

int main() {
    std::cout << "Starting complex task with threads...\n";
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(worker, i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    std::cout << "Complex task finished.\n";
    return 0;
}
