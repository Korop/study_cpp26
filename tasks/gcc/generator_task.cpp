#include <iostream>
#include <vector>
#include <string>
#include <print>
#include <generator>


std::generator<int> Numbers()
{
    for (int i = 0; i < 5; ++i)
        co_yield i;
}

// C#
// IEnumerable<int> Numbers()
// {
//     for (int i = 0; i < 5; ++i)
//         yield return i;
// }

/////
#include <generator>
#include <iostream>
#include <string>
#include <string_view>

struct Player
{
    int id;
    std::string name;
    int score;
};

// Simulates some expensive / streamed source.
std::generator<Player> LoadPlayers()
{
    co_yield Player{1, "Ivan", 120};
    co_yield Player{2, "Anna", 80};
    co_yield Player{3, "John", 150};
    co_yield Player{4, "Kate", 95};
    co_yield Player{5, "Bob", 200};
}

// Generator consumes another generator and filters values.
std::generator<Player> FilterByScore(
    std::generator<Player> players,
    int minScore)
{
    for (auto&& player : players)
    {
        if (player.score >= minScore)
        {
            co_yield player;
        }
    }
}

// Another generator in the pipeline.
// Converts Player -> std::string.
std::generator<std::string> GetPlayerDescriptions(
    std::generator<Player> players)
{
    for (auto&& player : players)
    {
        co_yield
            std::to_string(player.id)
            + ": "
            + player.name
            + " score="
            + std::to_string(player.score);
    }
}
////////
std::generator<int> Range2(int begin, int end, int step = 1)
{
    for (int value = begin;
        value < end;
        value += step)
    {
        co_yield value;
    }
};
//////--------
std::generator<int> Test()
{
    std::cout << "A\n";

    co_yield 10;

    std::cout << "B\n";

    co_yield 20;

    std::cout << "C\n";
}

// A simple C++23/26 compatible file
int main() {

    // std::generator<T> is a C++23 lazy sequence implemented using C++ coroutines. co_yield produces one value and suspends 
    // the coroutine. The coroutine resumes when the consumer requests the next element. This allows processing large
    // or even infinite sequences without storing the entire sequence in memory.

    //     C#                          C++

    // IEnumerable<T>              std::generator<T>

    // yield return x              co_yield x

    // foreach                     range-for

    // MoveNext()                  iterator ++ / coroutine resume

    // lazy execution              lazy execution

    

    for (int value : Numbers()) {
        std::println("{}", value);
    }
    
    ///-----
    // Execution is suspended around: co_yield
    // Conceptually
    // main()
    // |
    // v
    // LoadPlayers()
    // |
    // | co_yield Player #1
    // v
    // FilterByScore()
    // |
    // | score >= 100
    // | co_yield Player #1
    // v
    // GetPlayerDescriptions()
    // |
    // | co_yield string
    // v
    // main() prints it
    auto players = LoadPlayers();

    auto filtered =
        FilterByScore(
            std::move(players),
            100
        );

    auto descriptions =
        GetPlayerDescriptions(
            std::move(filtered)
        );

    for (const auto& text : descriptions)
    {
        std::cout << text << '\n';
    }

    // interesting co_yield loop
    //// A generator does not need one explicit co_yield per item:


    for (int value : Range2(10, 30, 5))
    {
        std::cout << value << '\n';
    }

    /////
    // co_yield value;
    // approximately as:
    // 1. expose value to caller
    // 2. save current function state
    // 3. suspend function
    // 4. caller continues
    // 5. next iterator increment resumes function
    // 6. continue exactly after co_yield
    for (int x : Test())
    {
        std::cout << "value=" << x << '\n';
    }

    std::println("");
    return 0;
}
