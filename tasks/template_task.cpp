#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <type_traits>


// -------------------------------------------------------
// 1. Alias template
//
// T = element type
// N = compile-time size
// -------------------------------------------------------

template<typename T, std::size_t N>
using FixedArray = std::array<T, N>;


// -------------------------------------------------------
// 2. Variadic class template
//
// Ts... = zero or more TYPES
// -------------------------------------------------------

template<typename... Ts>
struct TypeList
{
    // sizeof...(Ts) gives number of types in the pack.

    static constexpr std::size_t count =
        sizeof...(Ts);
};


// -------------------------------------------------------
// 3. Function template
//
// T = one deduced type
// -------------------------------------------------------

template<typename T>
constexpr T Square(T value)
{
    return value * value;
}


// -------------------------------------------------------
// 4. consteval function
//
// MUST execute at compile time.
// -------------------------------------------------------

consteval std::size_t MakeSize(std::size_t n)
{
    return n * 2;
}


int main()
{
    // ---------------------------------------------------
    // Alias template:
    //
    // exactly std::array<int, 3>
    // ---------------------------------------------------

    FixedArray<int, 3> numbers =
    {
        10,
        20,
        30
    };


    // ---------------------------------------------------
    // Parameter pack:
    //
    // Ts = { int, double, std::string }
    // ---------------------------------------------------

    using Types =
        TypeList<
            int,
            double,
            std::string
        >;

    static_assert(Types::count == 3);


    // ---------------------------------------------------
    // constexpr evaluated at compile time because
    // constexpr result requires a compile-time value.
    // ---------------------------------------------------

    constexpr int squared = Square(5);

    static_assert(squared == 25);


    // ---------------------------------------------------
    // consteval MUST run at compile time.
    // ---------------------------------------------------

    constexpr std::size_t size = MakeSize(5);

    // size == 10


    // ---------------------------------------------------
    // nullptr = dedicated null pointer literal.
    // ---------------------------------------------------

    int* pointer = nullptr;

    if (!pointer)
    {
        std::cout << "No object\n";
    }
}