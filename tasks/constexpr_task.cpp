#include <iostream>
#include <string>
#include <type_traits>

template<typename T>
void Describe(const T& value)
{
    if constexpr (std::is_integral_v<T>)
    {
        std::cout
            << "Integral value: "
            << value
            << '\n';
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        std::cout
            << "Floating point: "
            << value
            << '\n';
    }
    else
    {
        std::cout
            << "Other type\n";
    }
}

int main()
{
    int i = 15;
    const int& ii = i;
    Describe(i);
    Describe(ii);
    Describe(&i); //Other type
    Describe(std::move(i));
    Describe(10);
    Describe(3.14);
    Describe(std::string{"Ivan"}); //Other type
}

// Integral value: 15
// Integral value: 15
// Other type
// Integral value: 15
// Integral value: 10
// Floating point: 3.14
// Other type