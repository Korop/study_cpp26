#include <iostream>
#include <vector>
#include <string>
#include <print>
#include <ranges>


// MakeView()
// │
// ├── vector values [1,2,3,4]
// │
// ├── filter_view
// │      └── ref_view
// │             └── points to values
// │
// └── return filter_view
//        ↓
// values destroyed
//        ↓
// filter_view now dangling

auto MakeView()
{
    std::vector<int> values{
        1,2,3,4
    };

    return values
        | std::views::filter(
            [](int x)
            {
                return x > 2;
            }
        );
};
//
auto good()
{
    std::vector v{1, 2, 3};

    // return std::move(v) | std::views::filter([](int x){return x > 2;});
    return std::move(v) | std::views::filter([](int){return true;});
};

///
struct Player
{
    std::string name;
    int score{};
};

// A simple C++23/26 compatible file
int main() {
    std::vector<int> values{
        1, 2, 3, 4
    };
    

    auto even = values | std::views::filter( [](int x) { return x % 2 == 0; } );
    
    values.push_back(6);

    std::print("Evens: ");
    for (int x : even)
    {
        std::print("{} ", x);
    }
    //print all evens, including 6, Evens: 2 4 6 

    std::println("\nDoubled: ");
    auto doubled = values | std::views::transform( [](int value) { return value * 2; } );
    for (int value : doubled)
    {
        std::cout << value << ',';
    }
    // Pipeline composition
    std::vector<int> values2{
    1, 2, 3, 4, 5, 6
    };

    auto result2 = values2
        | std::views::filter(
            [](int value)
            {
                return value % 2 == 0;
            }
        )

        | std::views::transform(
            [](int value)
            {
                return value * 10;
            }
        );
    std::println("\nResult2 pipeline:");
    for (int x : result2)
    {
        std::cout << x << '\n';
    }
    ///Generate numbers lazily:
    for (int x : std::views::iota(1, 5))
    {
        std::cout << x << '\n';
    }
    auto numbers = std::views::iota(1, 10);
    static_assert(std::is_same_v<decltype(numbers), std::ranges::iota_view<int,int>>);
    std::println("Numbers: ");
    for (int x : numbers)
    {
        std::cout << x << ',';
    }
    //
    auto badView = MakeView();
    std::println("\nbadViews: ");
    // Undefined Behavior
    // ↓
    // maybe garbage
    // maybe works
    // maybe crash
    // maybe zsh: segmentation fault
    // for (int x : badView)
    // {
    //     std::cout << x << ',';
    // }
    // try/catch cannot catch undefined behavior, access violations, segmentation faults, or arbitrary hardware faults as standard C++ exceptions.

    auto badViewInline = []()
    {
        std::vector v{1, 2, 3};

        return v | std::views::filter([](int x) {
            return x > 1;
        });
    }();
    // Good
    auto viewGood = []()
    {
        std::vector v{1, 2, 3};

        return std::move(v)
            | std::views::filter([](int x) {
                return x > 1;
            });
    }();
    // or
    auto viewGood2 = std::vector{1, 2, 3} | std::views::filter([](int x) { return x > 1; });

    ///
    std::vector<Player> players{
        {"Ivan", 120},
        {"Anna", 80},
        {"Bob", 150},
        {"Kate", 95},
        {"Alex", 200}
    };

    auto topNames =
        players

        | std::views::filter(
            [](const Player& player)
            {
                return player.score >= 100;
            }
        )

        | std::views::transform(
            [](const Player& player) -> const std::string& // No string copies are required.
            {
                
                return player.name;
            }
        )

        | std::views::take(3);

        //Also, for top 3 specifically, materializing + sorting everything is not optimal. Later we can look at partial_sort / top-K, which is a better algorithmic fit.

    for (const auto& name : topNames)
    {
        // Here the view yields references to each player's existing name.
        // No string copies are required.
        std::cout << name << '\n';
    }
    std::println("End");
    return 0;
}



