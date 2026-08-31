#include <iostream>
#include <vector>
#include <string>

// A simple C++23/26 compatible file
int main() {
    std::vector<std::string> words = {"Hello", "from", "the", "C++", "study", "monorepo!"};
    for (const auto& word : words) {
        std::cout << word << " ";
    }
    std::cout << "\n";
    return 0;
}
