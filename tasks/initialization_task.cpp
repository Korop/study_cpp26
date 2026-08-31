#include <array>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <iostream>

enum class GameState
{
    Idle,
    Running,
    Finished
};

class Game
{
private:
    // Scalar defaults.
    std::uint64_t id_{};
    int bet_{1};
    bool enabled_{true};

    // Enum default.
    GameState state_{GameState::Idle};

    // Class types.
    std::string name_{};

    // Dynamic container.
    std::vector<int> prizes_{};

    // Fixed-size container.
    std::array<int, 3> multipliers_{
        1,
        2,
        5
    };

    // Key/value container.
    std::map<int, std::string> symbols_{};

    // Unique sorted values.
    std::set<int> activeLines_{};

public:
    // All field defaults above are used.
    Game() = default;


    // Explicit values override only selected defaults.
    Game(
        std::uint64_t id,
        std::string name,
        int bet
    )
        : id_{id},
          bet_{bet},
          name_{std::move(name)}
    {
        // enabled_     remains true
        // state_       remains Idle
        // prizes_      remains empty
        // multipliers_ remains {1,2,5}
        // symbols_     remains empty
        // activeLines_ remains empty
    }
};
class GameConfig
{
private:
    std::vector<int> bets_{
        1,
        5,
        10
    };

public:
    GameConfig() = default;

    explicit GameConfig(std::vector<int> bets)
        : bets_{std::move(bets)}
    {
    }
};
//----------------------------------------------------
class BufferIncorrect
{
private:
    std::size_t size_{1};
    std::vector<int> data_{1, 2, 3};
    
public:
    BufferIncorrect() = default;
    explicit BufferIncorrect(std::size_t size)
        : size_{size},
          data_(size_)
    {
    }

    int GetDataSize()  const
    {
        return data_.size();
    }

    int GetSize() const
    {
        return size_;
    }

};

//----------------------------------------------------
struct Rectangle
{
    int x{};
    int y{};
    int width{};
    int height{};
};
struct Point
{
    int x{};
    int y{};
};

//----------------------------------------------------
inline double fd() { return 1.0; };

extern double d1;
double d2 = d1;   // unspecified:
                // dynamically initialized to 0.0 if d1 is dynamically initialized, or
                // dynamically initialized to 1.0 if d1 is statically initialized, or
                // statically initialized to 0.0 (because that would be its value
                // if both variables were dynamically initialized)

double d1 = fd(); // may be initialized statically or dynamically to 1.0
//----------------------------------------------------
// A simple C++23/26 compatible file
int main() {
    std::println("Undefined Init: d2={}, d1={}", d2, d1);
    std::vector<int> vec;
    std::println("vec.size={}", vec.size());

    // BufferIncorrect buffer(10);
    BufferIncorrect buffer{};
    std::println("buffer data.size={}, size={}", buffer.GetDataSize(), buffer.GetSize());
    // ivan@Ivans-MacBook-Pro-2 interview_cpp % ./build/tasks/initialization_task               
    // Undefined Init: d2=0, d1=1
    // vec.size=0
    // buffer data.size=-388292272, size=10 - incorrect value for  BufferIncorrect buffer(10);

    Rectangle rect{
        .x = 10,
        .y = 12,
        .height = 600,
        .width = 800
    };
    Point p{
        .y = 20, //?
        .x = 10
    };
    // Rectangle rect{};
    std::println("Rectangle rect x={},y={}, h={}, w={}", rect.x, rect.y, rect.height, rect.width);

    std::map<std::string, int> ages{
        {"Bob", 20},
        {"Bob", 50}
    };
    std::println("map ages[Bob]={}", ages["Bob"]);

    GameConfig customConfig{
        {5, 25, 100}
    };
    // std::println("customConfig={}", customConfig);

    constexpr int size = 10;
    std::array<int, size> arr;
    
    return 0;
}