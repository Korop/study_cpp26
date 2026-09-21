#include <iostream>
#include <vector>
#include <string>
#include <print>
#include <functional>
#include <variant>
#include <type_traits>


//-------------------
template<typename T>
void Inspect()
{
    if constexpr (std::is_pointer_v<T>)
    {
        std::cout << "pointer\n";
    }
    else if constexpr (std::is_integral_v<T>)
    {
        std::cout << "integer\n";
    }
}
struct Player
{
    Player(int id, std::string name)
    {
    }
};
//---- 
class Connection
{
public:
    Connection() = default;
    Connection(Connection&) = delete;

    Connection(const Connection&) = delete;

    Connection(Connection&&) = default;
};
//------
template<typename T>
void Describe(T&& value)
{
    using RawT = std::decay_t<T>;
    // using RawT = std::remove_cvref_t<>;

    if constexpr ( std::is_integral_v<RawT> )
    {
        std::cout << "Describe integral: " << value << '\n';
    }
    else if constexpr ( std::is_floating_point_v<RawT> )
    {
        std::cout << "Describe floating: " << value << '\n';
    }
    else if constexpr ( std::is_same_v< RawT, std::string > )
    {
        std::cout << "Describe string: " << value << '\n';
    }
    else
    {
        std::cout << "Describe other type\n";
    }
};
// Describe(const char*) -> Describe<std::string>; // ❌ not a thing, only possible define the deduction guide for a class template
//---------------

// cannot create the first Astral object normally.
class Astral
{
public:
    // which requires an already existing Astral:
    Astral(const Astral&) = default;
    // no move constructor
    // need some constructor to allow creat an object instance of Astral
    //  Astral() = default;
};

//---------------

// A simple C++23/26 compatible file
int main() {
    static_assert(std::is_integral_v<int>);     // true
    static_assert(!std::is_integral_v<double>);  // false

    static_assert(std::is_same_v<int, int>); // true
    static_assert(!std::is_same_v<int, long>); // false
    static_assert(!std::is_same_v<int, const int>); // false
    static_assert(!std::is_same_v<int, int&>); // false

    Inspect<int>();   // integer
    Inspect<int*>();  // pointer
    
    //
    int x = 10;
    decltype(x) y = 20; // y type -> int
    decltype((x)) z = x; // z type -> int&
    z += 1;
    std::println("int& z={}", z);
    static_assert(std::is_same_v<int&, decltype(z)>); // true
    static_assert(std::is_same_v<decltype(z), int&>); // true

    // remove_reference
    using A = int&;

    using B =
        std::remove_reference_t<A>;

    static_assert(
        std::is_same_v<B, int>
    );

    {
        using T = const int;
        using U = std::remove_const_t<T>;
        static_assert(!std::is_same_v<U, int&>); // false
        static_assert(std::is_same_v<U, int>); // true

        using TV = volatile int;
        using TT = std::remove_volatile_t<TV>;
        static_assert(std::is_same_v<TT, int>);

        using TVC = const volatile int;
        using TTT = std::remove_cv_t<TVC>;
        static_assert(std::is_same_v<TTT, int>);
    }

    {
    // add_pointer
        using T = int;
        using P = std::add_pointer_t<T>; // P -> int*
        static_assert(std::is_same_v<P, int*>);
    }

    // add_lvalue_reference_t
    {
        using T = int;
        using TLVR = std::add_lvalue_reference_t<T>;
        static_assert(std::is_same_v<TLVR, int&>);
            
        using TRVR = std::add_rvalue_reference_t<T>;
        static_assert(std::is_same_v<TRVR, int&&>);
    }
    // decay_t
    // decay roughly removes:
    //     references
    //     const
    //     volatile
    // and also converts arrays/functions similarly to normal parameter passing.
    {
        int intValue = 20;
        const int& value = intValue;
        std::decay_t<decltype(value)> intValue2 = 20;
        using INT = std::decay_t<decltype(value)>;
        static_assert(std::is_same_v<INT, int>);
    }

    int someOutValue = 10;
    std::variant<int, double, std::string> variant2 = someOutValue;
    std::visit(
        [](const auto& value)
        {
            using T = std::decay_t<decltype(value)>;
            // using T = decltype(value);
            if constexpr (
                std::is_same_v<T, int>
            )
            {
                std::println("variant is int value={}", value);
            } else {
                std::println("Error: variant is not int");
            }
        },
        variant2
    );

    std::println("someOutValue={}", someOutValue);

    std::variant<
        std::reference_wrapper<const int>,
        double,
        std::string
    > variant3 = std::cref(someOutValue);

    someOutValue += 10;

    std::visit(
        [](const auto& value)
        {
            // using T = std::decay_t<decltype(value)>;
            using T = std::remove_cvref_t<decltype(value)>;
            if constexpr ( std::is_same_v< T, std::reference_wrapper<const int> > )
            {
                std::println("original variant is int value={}", value.get());
            } else {
                std::println("Error: variant is not int");
            }
        },
        variant3
    );

    std::println("after visit someOutValue={}", someOutValue);
    // we can use 'const int*` pointer  instead of reference_wrapper (which also really wrapper around pointer to the value)
    std::variant<const int*, double> variant4 = &someOutValue;
    std::visit(
        [](const auto& value)
        {
            // using T = std::remove_cvref_t<decltype(value)>;
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, const int*>)
            {
                std::println("variant4: value={}", *value);
            }
        },
        variant4
    );

    // is_constructible_v
    static_assert( std::is_constructible_v<Player, int, std::string> );

    static_assert( !std::is_constructible_v<Player, int> );

    // Copy/move traits
    // std::is_copy_constructible_v<T>
    // std::is_move_constructible_v<T>

    // std::is_copy_assignable_v<T>
    // std::is_move_assignable_v<T>

    static_assert( !std::is_copy_constructible_v< Connection > );
    
    
    //-------
    static_assert( std::is_move_constructible_v< Connection > );
    Connection conn{};
    Connection& connRef = conn; // there is no an actual Connection(...) costructor call here 
    // “Can a value of type Connection& be initialized from the corresponding copy source type?”
    // Connection(Connection&) = delete; is irrelevant for that test, because you're not constructing a Connection
    static_assert( std::is_copy_constructible_v< Connection& > );
    static_assert(std::is_constructible_v<Connection&, Connection&>);
    // Connection conn2(connRef); // could not compile, error: call to deleted constructor of 'Connection'

    //--------
    static_assert( !std::is_constructible_v<Connection, Connection&> ); //? -> std::is_constructible_v<Connection&, Connection&>
    Connection c;
    Connection& r1 = c;
    // Connection(Connection&) = delete; is irrelevant for that test, because you're not constructing a Connection
    Connection& r2 = r1;
    const Connection& r3 = Connection{};
    const Player& r4 = Player{2, "Ivan"};
    // Player& p5 = Player{3, "Jora"}; // could not compile, error: non-const lvalue reference to type 'Player' cannot bind to a temporary of type 'Player'
    Player&& p5 = Player{3, "Jora"}; // could not compile

    {
        using TConnRef = Connection&;

        static_assert(
            std::is_same_v< std::add_const_t<TConnRef>, Connection& >
        );

        static_assert(
            std::is_same_v< std::add_lvalue_reference_t<std::add_const_t<TConnRef>>, Connection& >
        );
        
    }


    // common
    {
        int x{10};

        const double d{3.14};

        std::string name{"Ivan"};

        Describe(x);
        Describe(d);
        Describe(name);
        Describe(std::string{"Anna"});
        // std::is_same_v<const char*, std::string> // false
        // const char (&)[5]
        Describe("Jora");
        static_assert( std::is_same_v< std::decay_t<decltype("Jora")>, const char* > );
    }

    /// move
    // is_move_constructible_v<T> == true не гарантує, що у T реально існує move constructor.
    static_assert(std::is_move_constructible_v<Astral>);
    // Astral a100;    // ERROR: no Astral()
    // Astral a100{};  // ERROR: no Astral()
    Astral aOfA(); // NOTE: it is function defenition !!! Not an object of Astral


    // std::add_const_t
    // -------- You cannot make the reference itself const.
    // A reference is already permanently bound to one object after initialization.
    // The binding never changes.
    // int& const - is not valid type
    // const int& - valid type
    int valInt = 10;
    const int& testRefInt = valInt; // where testRefInt - is reference to const int, not const reference to int
    // add_const_t<int&> does not become const int&

    static_assert(
        std::is_same_v< std::add_const_t<int>, const int >
    );

    static_assert(
        std::is_same_v< std::add_const_t<int*>, int* const >
    );

    static_assert(
        std::is_same_v< std::add_const_t<int&>, int& >
    );

    static_assert(
        std::is_same_v< std::add_const_t<const int&>, const int& >
    );
    
    // Same reason. int&& const is not a valid reference type.
    static_assert(
        std::is_same_v< std::add_const_t<int&&>, int&& >
    );
    {
        using T = int;
        using R = std::add_lvalue_reference_t< std::add_const_t<T> >; // R is const int&
        // 1. T                       // int
        // 2. std::add_const_t<T>     // const int
        // 3. std::add_lvalue_reference_t<...> //const int&
        static_assert(std::is_same_v<R, const int&>);
    }

    std::println("");
    return 0;
}
