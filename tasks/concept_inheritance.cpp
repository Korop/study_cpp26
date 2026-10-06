#include <concepts>
#include <iostream>
#include <string>

struct Player
{
    int id{};
    std::string name{};

    Player(int id, std::string name): id{id}, name{std::move(name)}
    {
    }

    int GetId() const
    {
        return id;
    }
};

struct Enemy
{
    int GetId() const
    {
        return 100;
    }
};

struct Object
{
};

template<typename T>
concept Identifiable =
    requires(const T& value)
    {
        {
            value.GetId()
        } -> std::same_as<int>;
    };


template<Identifiable T>
void PrintId(const T& value)
{
    std::cout << value.GetId() << '\n';
}

// Modern factory example
template<
    typename T,
    typename... Args
>
requires std::constructible_from<T, Args...>
std::unique_ptr<T>
Create(Args&&... args)
{
    return std::make_unique<T>(
        std::forward<Args>(args)...
    );
}

int main() {
    Player player{
        1,
        "Ivan"
    };

    Enemy enemy;

    PrintId(player); // OK
    PrintId(enemy);  // OK   

    auto playerVitalik = Create<Player>(42, std::string{"Vitalik"});
}