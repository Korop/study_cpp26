#include <iostream>
#include <vector>
#include <string>
#include <print>

struct Player
{
    int health{};
};

template<typename... Players>
int TotalHealth(const Players&... players)
{
    return (0 + ... + players.health);
}

// C++11 usually required recursive templates.
void Print()
{
    std::println("<Print empty call.>");
}

template<typename T, typename... Rest>
void Print(T first, Rest... rest)
{
    std::cout << first;

    Print(rest...);
}
///

template<typename... Args>
void PrintAll(Args&&... args)
{
    // (init op ... op args) - Binary left fold
    // (std::cout << ... << args) - fold expression
    (std::cout << ... << std::forward<Args>(args));
}

////
template<typename T>
bool IsValid(const T& value)
{
    if constexpr (std::is_integral_v<T>)
    {
        return value >= 0;
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        return value >= 0.0;
    }
    else
    {
        return !value.empty();
    }
}

template<typename... Args>
bool AreAllValid(const Args&... args)
{
    return (IsValid(args) && ...); //(args op ...) - Unary right fold: a op (b op c)
}

// A simple C++23/26 compatible file
int main() {
    std::println("FoldExpression start.");
    Player a{100};
    Player b{70};
    Player c{50};

    int total = TotalHealth(a, b, c);

    //print total=220
    std::println("health total: {}", total);

    // c++11 recursive template: "C++11 old style!""
    Print("C++", 11, " old", " ", "style", "!");
    std::println("");
    // c++17 fold expression there are four forms.
    // (... op args) - Unary left fold: ((a op b) op c)
    // (args op ...) - Unary right fold: a op (b op c)
    // (init op ... op args) - Binary left fold: ((init op a) op b) op c
    // (args op ... op init) - Binary right fold: a op (b op (c op init))
    PrintAll("C++", 11, " old", " ", "style", "!");
    std::println("");
    ///
    bool result = AreAllValid(
        10,
        5.5,
        std::string{"Ivan"}
    );
    std::println("validation result={}", result);

    std::println("");
    return 0;
}
