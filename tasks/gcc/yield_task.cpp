#include <generator>
#include <print>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

struct Player
{
    int id;
    std::string name;
    int score;
};

// C# yield return analog:
// IEnumerable<Player> -> std::generator<Player>
// yield return x       -> co_yield x
std::generator<Player> LoadPlayers()
{
    std::println("LoadPlayers: start");

    co_yield Player{1, "Ivan", 120}; // Lazy: execution suspends here.
    co_yield Player{2, "Anna", 80};
    co_yield Player{3, "John", 150};

    std::println("LoadPlayers: end");
}

// Source owns temporary Player values produced by the coroutine.
std::generator<Player> FilterByScore(
    std::generator<Player> players,
    int minScore)
{
    for (auto&& player : players)
    {
        static_assert(std::is_same_v<decltype(player), Player&&>);

        if (player.score >= minScore)
        {
            // `player` has type Player&&, but a named variable is an lvalue expression.
            //
            // co_yield player;                    // lvalue -> may create an extra Player copy
            // co_yield std::forward<decltype(player)>(player); // also OK here
            co_yield std::move(player);            // explicit: yield as rvalue
        }
    }
}

// IMPORTANT: std::move is NOT universally correct for every range.
//
// This generator yields references to Players owned by an external vector.
// We must preserve them as lvalues.
std::generator<Player&> PlayersFromVector(std::vector<Player>& players)
{
    for (auto& player : players)
    {
        co_yield player;
    }
}

std::generator<Player&> FilterVectorPlayers(
    std::generator<Player&> players,
    int minScore)
{
    for (auto&& player : players)
    {
        // Here decltype(player) is Player&, not Player&&.
        if (player.score >= minScore)
        {
            // co_yield std::move(player); // BAD idea: forces external vector element to rvalue
            co_yield std::forward<decltype(player)>(player); // preserves Player&
        }
    }
}

int main()
{
    // ------------------------------------------------------------------
    // 1. generator is lazy: no Player sequence is materialized here.
    // ------------------------------------------------------------------
    auto players = LoadPlayers();

    // ------------------------------------------------------------------
    // 2. std::move(players) moves generator/coroutine ownership.
    //    It does NOT move all Player objects somewhere.
    //
    //    main::players becomes moved-from.
    //    FilterByScore now owns the source generator.
    // ------------------------------------------------------------------
    auto filtered = FilterByScore(std::move(players), 100);

    std::println("--- generator<Player> ---");

    for (auto&& player : filtered)
    {
        std::println("{}: {}", player.name, player.score);
    }

    // Output:
    // LoadPlayers: start
    // Ivan: 120
    // John: 150
    // LoadPlayers: end
    //
    // Anna is generated, checked, rejected, then discarded.
    // There is never a vector like [Ivan, John] inside `filtered`.

    // ------------------------------------------------------------------
    // 3. Different case: generator<Player&> over external vector storage.
    // ------------------------------------------------------------------
    std::vector<Player> storedPlayers{
        {10, "Bob", 200},
        {11, "Kate", 50},
        {12, "Mike", 180}
    };

    auto referenced = PlayersFromVector(storedPlayers);
    auto referencedFiltered = FilterVectorPlayers(std::move(referenced), 100);

    std::println("--- generator<Player&> ---");

    for (auto&& player : referencedFiltered)
    {
        std::println("{}: {}", player.name, player.score);
    }

    // Rule:
    //
    // std::move(x)
    //     -> "I intentionally want x treated as an rvalue."
    //
    // std::forward<decltype(x)>(x)
    //     -> "Preserve whether x originally was T&, const T&, or T&&."
    //
    // Therefore:
    //
    // generator<Player>   + transient values -> std::move(player) is clear.
    // generator<Player&>  + external objects  -> preserve reference category.
}

// The whole theme in four lines
// auto g = LoadPlayers();                         // lazy coroutine, not a container
// auto f = FilterByScore(std::move(g), 100);     // moves generator ownership
// co_yield std::move(player);                    // transient Player: yield as rvalue
// co_yield std::forward<decltype(player)>(player); // generic/reference case: preserve category

// does not imply that std::move(player) is always correct. If player ultimately refers to an object stored in a std::vector,
// moving from it can leave that vector element in a moved-from state. For generic forwarding code,
//  std::forward<decltype(player)>(player) preserves what the source actually yielded.