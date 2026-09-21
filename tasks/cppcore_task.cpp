#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <random>
#include "utils.hpp"
#include <span>


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
        Print("health={}, damageValue={}\n" , std::make_format_args(_health, damageValue));
    }

    void Damage() const
    {
        Print("health={}\n" , std::make_format_args(_health));
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
class AnimalBad
{
public:
    virtual void Speak();
};
// Derived
class CatBad : public AnimalBad
{
public:
    void Speak() override
    {
        std::cout << "Cat\n";
    }
};
////

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
////////
// void FooInt(int* values){};
// void FooInt(int values[]){};
//The compiler does not enforce the 5.
void FooInt(int values[5]) {
     values[0] = 42;
};

// In modern C++26, I'd usually prefer std::span
void FooIntSpan(std::span<int, 5> values)
{
    values[1] = 42;
}
// They are all treated as: void Foo(int* values);

/////////////////////
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
        Println("Enum color Ok", std::make_format_args());
    }

    std::cout << "CppCore: Lambda\n";
    // lambda 1
    auto add = [](int a, int b)
    {
        return a + b;
    };
    int result = add(10, 20);
    Println("Lambda result={}", std::make_format_args(result));
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
    Println("Lambda increment counter={}", std::make_format_args(counter));

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
    Println("lambdaEverythingByRef: increment counter={}, mult={}", std::make_format_args(counter, mult));
    // This modifies the lambda's copy:
    int x = 10;

    auto lambdaMutable = [x]() mutable
    {
        x++;
        Println("LambdaMutable: internal coly x={}", std::make_format_args(x));
    };

    lambdaMutable();
    Println("LambdaMutable external x={}", std::make_format_args(x)); // still 10
    // 
    std::function<int(int)> lambdaFunction =
    [](int value)
    {
        return value * 2;
    };
    int funcRestult = lambdaFunction(50);
    Println("LambdaFunction function={}", std::make_format_args(funcRestult));
    
    unsigned long long int val = 10;
    
    std::function<void(int)> lambdaAction = [](int value) {
        Println("lambdaAction action={}", std::make_format_args(value));
    };
    lambdaAction(55);

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
    Println("Constexpr: squareResult={}", std::make_format_args(squareResult));
    // const
    //      cannot modify

    // constexpr
    //     compile-time constant value

    // consteval
    //     function must execute at compile time

    // constinit
    //     variable initialization must happen statically
    {
        int aaa[5]{};
        FooInt(aaa);
        FooIntSpan(aaa);
        
        int bbb[2]{};
        FooInt(bbb); // compiles
        // FooIntSpan(bbb); // non compiled, need exact size 5
    }
    std::cout << "Stop CppCore\n";

    return 0;
}
