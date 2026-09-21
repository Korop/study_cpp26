#include "utils.hpp"

#include <iostream>

void Print(std::string_view format, std::format_args args)
{
    std::cout << std::vformat(format, args);
}

void Println(std::string_view format, std::format_args args)
{
    std::cout << std::vformat(format, args) << '\n';
}
