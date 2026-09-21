#include <bit>
#include <print>
#include <string>
#include <utility>

class Base
{
public:
    virtual ~Base() = default;
};

class Derived : public Base
{
};

void Process(std::string& lVal)
{
    std::println("lvalue={}", lVal);
}

void Process(std::string&& rVal)
{
    std::println("rvalue={}", rVal);
}

void Test(std::string&& value)
{
    Process(value);
}

// A simple C++23/26 compatible file
int main() {
    std::println("Cast Start");
    float x = 10.7f;

    auto a = static_cast<std::uint32_t>(x);
    // numeric value: 10
    std::println("static_cast<std::uint32_t>: a={}", a);

    auto b = std::bit_cast<std::uint32_t>(x);
    // representation bits of float 10.0
    std::println("bit_cast<std::uint32_t>: b={}", b);


    // Does std::move actually move the object?
    // No. It is essentially a cast that enables move semantics. The actual move is performed by the move constructor or move assignment operator.
    std::string str1 = "hello";
    std::string str2 = std::move(str1);
    std::println("move: str1={}, str2={}", str1, str2);
    // Base is polymorphic class as has at least one virtual method
    Base* base = new Derived();

    Derived* derived =
        dynamic_cast<Derived*>(base);
    ///
    int i[] = {42};
    double d = 3.14;

    void* aIntVoid = &i;
    void* aDoubleVoid = static_cast<void*>(&d);

    auto wrongDouble = static_cast<double*>(aIntVoid);
    // every time different value as more memmory need for double then int
    // you can add more element in array i to stabilize memory value
    std::println("cast wrongDouble={}", *wrongDouble);

    auto wrongInt = static_cast<int*>(aDoubleVoid);
    std::println("cast wrongInt={}", *wrongInt);

    // void* doesn't mean nullptr
    void* p1 = nullptr; // p1 points nowhere
    int xx = 42;
    void* p2 = &xx; //p2 points somewhere, but the type is unknown


    // rvalue=rvalue temporary
    Process(std::string("rvalue temporary"));
    // lvalue=temporary
    Test(std::string("temporary"));

    std::println("");
    return 0;
}
