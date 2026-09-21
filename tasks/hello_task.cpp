#include <iostream>
#include <vector>
#include <string>
#include <print>

// A simple C++23/26 compatible file
int main() {
    std::vector<std::string> words = {"Hello", "from", "the", "C++", "study", "monorepo!"};
    for (const auto& word : words) {
        std::print("word {}", word);
    }
    std::println("");
    return 0;
}
