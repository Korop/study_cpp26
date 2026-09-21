#pragma once

#include <format>
#include <string_view>

void Print(std::string_view format, std::format_args args);
void Println(std::string_view format, std::format_args args);
