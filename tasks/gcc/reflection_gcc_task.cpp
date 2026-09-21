#include <meta>
#include <iostream>
#include <type_traits>
#include <print>

struct Player
{
    int score;
    double health;
};

template<typename T>
class BoxD
{
public:
    T Value{};

    BoxD(T value)
        : Value{value}
    {
        // static_assert(
        //     std::meta::is_same_type(^^T, ^^std::string)
        // );
       std::cout << "BoxD T param deduction: " << std::meta::display_string_of(^^T) << '\n'; 
    }
};

// Sometimes the compiler cannot deduce exactly what you want. You can provide a deduction guide:
// Deduction guides ->
// Note: that syntax is only for class template argument deduction (CTAD), no method allowd
BoxD(const char*) -> BoxD<std::string>;

// ---------------------------------------------------------------------

int globalValue = 10;

int& Get()
{
    return globalValue;
}

template<typename T>
void FuncLikeAuto(T value);


///
int main()
{
    constexpr auto r1 = ^^Player;
    constexpr auto r2 = ^^int;
    constexpr auto r3 = ^^Player::score;
    static_assert(std::meta::is_type(^^Player));
    static_assert(std::meta::is_type(^^int));

    static_assert(
        std::meta::is_nonstatic_data_member(^^Player::score)
    );

    static_assert(
        std::meta::identifier_of(^^Player) == "Player"
    );

    static_assert(
        std::meta::identifier_of(^^Player::score) == "score"
    );

    constexpr std::meta::info r = ^^Player;

    std::cout << std::meta::display_string_of(r) << '\n';

    const char* nameBox = "Ivan";
    BoxD boxIvanString{nameBox};
    // decltype(boxIvanString) sssd{"d"};


    constexpr auto boxChar = ^^BoxD<char*>::Value;
    constexpr auto boxStringValue = ^^BoxD<char*>::Value;
    constexpr auto typeValue = std::meta::type_of(^^boxIvanString);

    std::cout << std::meta::display_string_of(typeValue) << '\n';
    
    // reflect variable
    constexpr std::meta::info boxIvanStringInfo = ^^boxIvanString;
    // reflect variable type
    constexpr std::meta::info boxIvanStringTypeInfo = std::meta::type_of(^^boxIvanString);
    // reflect type
    constexpr std::meta::info expectedTypeInfo = ^^BoxD<std::string>;

    static_assert(std::meta::is_same_type(boxIvanStringTypeInfo, expectedTypeInfo));

    //-------------------------
    int x = 10;
    decltype(x) y = 20; // y type -> int
    decltype((x)) z = x; // z type -> int&
    z += 1;
    std::println("int& z={}", z);
    static_assert(
        std::meta::is_same_type(^^int&, std::meta::type_of(^^z))
    );

    static_assert(
        std::meta::is_same_type(^^decltype(z), ^^int&)
    );
    //without reflection
    static_assert(std::is_same_v<decltype(z), int&>);

    // ----------------------- auto deduction type
    // Because plain auto and decltype(auto) follow different deduction rules
    // auto uses template-like type deduction. With auto x = expr, references and top-level cv-qualifiers
    // are removed because the variable is deduced as a value. decltype(auto) instead applies decltype rules
    // to the initializer, which preserve the expression's value category. Since a function returning int&
    // produces an lvalue expression, decltype(Get()) is int&, so decltype(auto) preserves the reference.


    // Plain auto uses value-like template deduction
    // but plain auto deduction behaves roughly like template deduction for a by-value parameter:
    // FuncLikeAuto(Get()) int& -> T -> int
    // You get a copy of the referred value.
    // auto x = expression; // Deduce the type for a new value/object.
    // References and top-level const are normally stripped.

    auto x1 = Get();
    // x1 type -> int, why?
    x1++;
    std::println("globalValue={}, x1={}", globalValue, x1); //print globalValue=10, x1=11

    //but you have refference to globalValue
    // decltype(auto) x = Get(); -> decltype(Get()) x = Get(); where Get() is int&
    // decltype(auto) x = expression; // Determine the type using decltype(expression) rules.
    // That preserves the expression's reference/value-category information.
    decltype(auto) x2 = Get();
    // x2 type -> int&
    x2++;
    std::println("globalValue={}, x2={}", globalValue, x2); // print globalValue=11, x2=11

    auto a = Get();           // int
    auto& b = Get();          // int&
    auto&& c = Get();         // int&   because reference collapsing
    decltype(auto) d = Get(); // int&

    


}