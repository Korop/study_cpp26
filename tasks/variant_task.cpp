#include <iostream>
#include <vector>
#include <string>
#include <print>

struct Printer
{
    void operator()(int value) const
    {
        std::println("{}", value);
    }
};
//------------------
struct A
{
    void operator()(int x) const {
        std::println("A int: {}", x);
    }
};

struct B
{
    void operator()(double x) const {
        std::println("B douible: {}", x);
    }
    void operator()(long long x) const {
        std::println("B long: {}", x);
    }
};

struct E
{
    void operator()(std::string x) const {
        std::println("E douible: {}", x);
    }
};

struct C : A, B
{
    // Bring all inherited operator() functions into one overload set.
    using A::operator();
    // That's because you're importing the name, including all overloads of that name.
    using B::operator();
};

struct D : A, B
{
    void operator()(int valueInt) const { 
        A::operator()(valueInt);
    };
    void  operator()(double valueDouble) const {
        B::operator()(valueDouble);
    };
};

struct AE : A, E
{
};

struct G
{
    void Process() {}
};

struct AG : A, G
{
};
//------------------
template<class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};


// A simple C++23/26 compatible file
int main() {

    C c;
    c(10);
    c(3.14);
    c((long long)400000);
    c.A::operator()(11);
    c.B::operator()(4.14);

    D d;
    d(30.33);
    d(15);

    // "AE::operator()" is ambiguous withtou using or direct redefinition of operator() in AE
    // The reason is subtle and important for interviews: C++ does name lookup first, then overload resolution.
    // C++ first searches for the name: operator()
    // At that point, member lookup is ambiguous. C++ does not yet ask: "Which signature better matches int?"
    // So overload resolution never gets the chance to save us. A charming little language-lawyer trap.
    AE ae;
    // ae("Test AE"); // compliation error 

    // If only one base class has operator(), then you don't need using:
    AG ag;
    ag(100);

    Printer p{};
    p(42);
    // Looks like a function call, but C++ translates it conceptually to:
    p.operator()(42);
    
    // Every lambda object has an operator() member function.
    auto lambdaSimple = [](int x)
    {
        std::println("{}", x);
    };
    lambdaSimple(10);
    lambdaSimple.operator()(10);
    
    // There are two lambda types.
    // overloaded<lambda [](int x)->void, lambda [](double x)->void> visitor
   auto visitor = overloaded{
        [](int x)
        {
            std::println("int: {}", x);
        },

        [](double x)
        {
            std::println("double: {}", x);
        }
    };

    ///----------- direct types for lambda
    auto intLambda = [](int x)
    {
        std::println("int: {}", x);
    };

    auto doubleLambda = [](double x)
    {
        std::println("double: {}", x);
    };
    using IntLambda = decltype(intLambda);
    using DoubleLambda = decltype(doubleLambda);
    overloaded<IntLambda, DoubleLambda> visitorStrongTyped = overloaded{
        intLambda,
        doubleLambda
    };

    overloaded<
        decltype(intLambda),
        decltype(doubleLambda)
    > visitorStrongShotForm{
        intLambda,
        doubleLambda
    };
    ////
    using Visitor = overloaded<
        decltype([](int x)
        {
            std::println("int: {}", x);
        }),

        decltype([](double x)
        {
            std::println("double: {}", x);
        })
    >;

    Visitor visitorFromType{};

    std::println("");
    return 0;
}
