#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <random>


class Player
{
private:
    int _health=10;

public:
    Player(): _health(15)
    {

    }
    void Damage(int damageValue)
    {
        _health -= damageValue;
        std::print("health={}, damageValue={}\n" , _health, damageValue);
    }

    void Damage() const
    {
        std::print("health={}\n" , _health);
    }
};

// ----------------------------------//
// virtual
class Animal
{
public:
    virtual void Speak()
    {
        std::cout << "Animal\n";
    }
};

// Derived
class Cat : public Animal
{
public:
    void Speak() override
    {
        std::cout << "Cat\n";
    }
};
// ----------------------------------//

enum ColorEnum
{
    Red,
    Green,
    Blue
};
///

enum class ColorEnumClass
{
    Red,
    Green,
    Blue
};

enum struct ColorEnumStruct
{
    Red,
    Green,
    Blue
};
//---------------------------------
//Lambda


//---------------------------------
void print_num(int i)
{
    std::cout << i << '\n';
}

//---------------------------------
//constexpr
constexpr int Square(int x)
{
    return x * x;
}

// A simple C++23/26 compatible file
int main() {
    std::cout << "Start CppCore\n";

    const Player player = Player();
    player.Damage();

    auto playerMutable = const_cast<Player*>(&player);
    playerMutable->Damage(30);
    

    // move reference T&& - refference to an rval object
    std::string a = "Hello A";
    std::cout << "Before move: a=" << a << "\n";
    std::string b = std::move(a);

    std::cout << "b="<< b <<", after move: a=" << a << "\n";


    // virtual
    std::cout << "CppCore: virtual\n";
    Cat cat;
    Animal& animal = cat;
    animal.Speak();

    // enum
    if ((int)ColorEnum::Red == (int)ColorEnumClass::Red) {
        std::println("Enum color Ok");
    }

    std::cout << "CppCore: Lambda\n";
    // lambda 1
    auto add = [](int a, int b)
    {
        return a + b;
    };
    int result = add(10, 20);
    std::println("Lambda result={}", result);
    // lambda 2 Lambda empty capture
    double  mult = 1.5;
    auto divideEmptyCapture = [](double a, double b) -> double
    {
        // return a / b * mult; // error
        return a / b;
    };

    // lambda 3 Lambda copy capture
    
    auto dividecCopyCapture = [mult](double a, double b) -> double
    {
        // mult = 3.0; // error
        return a / b * mult;
    };

    // lambda 4 Capture by reference:
    int counter = 0;
    auto increment = [&counter]()
    {
        ++counter;
    };
    increment();
    increment();
    std::println("Lambda increment counter={}", counter);

    // 
    auto lambdaEverythingByRef = [&, mult]()
    {
        // everything by reference
        counter++;
        // multiplier specifically by value
        // mult = 10.0; // error
        counter *= mult;
    };

    lambdaEverythingByRef();
    std::println("lambdaEverythingByRef: increment counter={}, mult={}", counter, mult);
    // This modifies the lambda's copy:
    int x = 10;

    auto lambdaMutable = [x]() mutable
    {
        x++;
        std::println("LambdaMutable: internal coly x={}", x);
    };

    lambdaMutable();
    std::println("LambdaMutable external x={}", x); // still 10
    // 
    std::function<int(int)> lambdaFunction =
    [](int value)
    {
        return value * 2;
    };
    int funcRestult = lambdaFunction(50);
    std::println("LambdaFunction function={}", funcRestult);
    
    unsigned long long int val = 10;
    

    // constant expression and is available at compile time
    // A constexpr variable is also effectively const
    // constexpr
    //    compile time if required/possible
    //             OR
    //    runtime when used normally
    constexpr int mySize = 10;
    // const int mySize = 10; // also allowed
    std::array<int, mySize> my10Values;
    
    // int mySize20 = 20;
    // note: read of non-const variable 'mySize20' is not allowed
    // std::array<int, mySize20> my20Values;

    //    runtime when used normally
    int runtimeValue = (int) std::rand()  % 10;
    int squareResult = Square(runtimeValue);
    std::println("Constexpr: squareResult={}", squareResult);
    // const
    //      cannot modify

    // constexpr
    //     compile-time constant value

    // consteval
    //     function must execute at compile time

    // constinit
    //     variable initialization must happen statically



    std::cout << "Stop CppCore\n";

    return 0;
}
