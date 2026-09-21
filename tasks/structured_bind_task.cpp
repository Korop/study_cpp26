
#include <iostream>
#include <vector>
#include <string>
#include <print>
#include <map>
// A simple C++23/26 compatible file
int main() {
    std::println("Strutured binding: start");
    std::pair<int, std::string> player{
        42,
        "IvanPair"
    };

    auto& [id, name] = player;
    
    std::println("id={}, name={}", id, name);

    // No copy, no modification.
    const auto& [idConstRef, nameConstRef] = player;
    //-----
    std::map<std::string, int> ages{
        {"Ivan", 40},
        {"Anna", 30}
    };

    for (const auto& [key, value] : ages)
    {
        std::cout << key << ": " << value << '\n';
    }

    return 0;
}
