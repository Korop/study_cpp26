#include <iostream>
#include <vector>
#include <string>
#include <print>

template<typename T>
void Process(T&& value)
{
    std::string movedName{
        std::forward<T>(value)
    };
    // auto movedName{std::move(value)};
    std::println("Process: movedName='{}', value='{}'", movedName, value);
}

class Player
{
private:
    std::string name_;

public:
    explicit Player(std::string name)
        : name_{std::move(name)}
    {
    }
};
//---------
struct User
{
    int id_;
    std::string name_;
    User(int id, std::string name): id_{id}, name_{name}
    {
    }
    int size() {
        return id_ * 100;
    }
};
////////----
template<typename T>
void ProcessIntergralType(T&& value)
{
    using ValueType = std::remove_cvref_t<T>;

    if constexpr (std::is_integral_v<ValueType>)
    // if constexpr (std::is_integral_v<T>) // incorrect integral type detection
    {
        std::cout << "Integral\n";
    } else {
        std::cout << "Not Integral\n";
    }
}

template<std::integral T>
void ProcessIntegralOnly(T value)
{
}

template<typename T>
requires std::integral<T>
void ProcessIntegralRequire(T value)
{
}

template<typename T>
concept Integer =
    std::integral<T> &&
    !std::same_as<std::remove_cv_t<T>, bool>;
template<Integer T>
T Square(T value)
{
    return value * value;
}    
////
template<typename T>
void Print(T value)
{
    if constexpr (std::is_integral_v<T>)
    {
        std::cout << value + 1 << "\n";
    }
    else
    {
        std::cout << "size=" << value.size()  << "\n";
    }
}
///
template<typename T>
void ByValue(T x);     // strips reference

template<typename T>
void ByReference(T& x); // preserves reference
int globalValue = 10;

int& Get()
{
    return globalValue;
}
/////////

void FooArrayGlobal(int values[5]) {

}

void FooPointerGlobal(int* values) {

}
//////////////////////

// A simple C++23/26 compatible file
int main() {
    std::println("Reference: start");

    std::string nameSource{"House of fun"};
    // std::string nameNewOwner = std::move(nameSource);
    std::string nameNewOwner{std::move(nameSource)};

    std::println("Reference: nameSource='{}'(should be empty), nameNewOwner='{}'", nameSource, nameNewOwner);

    // Reference collapsing rules
    // &  + &  → &
    // &  + && → &
    // && + &  → &
    // && + && → &&
    
    const std::string name1{"Ivan"};
    std::string name2{"Ivan"};

    //Therefore the function becomes effectively:
    //void Process(std::string& value);
    Process(name1);                  // OK
    std::println("Reference: after template Process(name1) name1={}", name1);
    Process(std::string{"Natalia"}); // OK

    Process(std::move(name2));
    std::println("Reference: after template Process(std::move(name2)) name2={}", name2);

    //
    std::string name3{"Ivan"};
    Player p{std::move(name3)};
    // name3 is empty
    std::println("Reference: after Player p{{std::move(name3)}} name3='{}'", name3);

    //
    std::string name4{"Ivan"};
    Player p2{name4};
    // name4 is not empty
    std::println("Reference: after Player p{{name4}}={}", name4);

    ////
    std::vector<User> users;
    User userVasyl{41, "Vasyl"};

    users.push_back(
        User{42, "Ivan"}
    );
    users.push_back(userVasyl);
    std::println("userVasyl: id={}, name={}", userVasyl.id_, userVasyl.name_);

    userVasyl.id_ = 43;

    users.push_back(std::move(userVasyl));
    std::println("userVasyl std::move: id={}, name={}", userVasyl.id_, userVasyl.name_);
    for (auto&& user : users) {
        std::println("Vector1.1: users id={}, name={}", user.id_, user.name_);
        user.name_ += "_updated";
        auto userLocal = std::move(user);
        std::println("Vector2: users id={}, name={}", user.id_, user.name_);
        std::println("Vector3: users moved to local id={}, name={}", userLocal.id_, userLocal.name_);
    }
   
    for (auto&& user : users) {
        std::println("Vector3: moved users id={}, name={}", user.id_, user.name_);
    }
    ////
    std::vector<bool> flags1{
        true,
        false,
        true
    };
    for (std::vector<bool>::reference  flag : flags1)
    {
        flag = false;
    }
    ///
    int xx = 10;
    const int& bb = xx;
    ProcessIntergralType(xx);
    ProcessIntergralType(bb);
    ProcessIntergralType("bb")
    ;
    // int          → integral
    // const int    → integral
    // int&         → NOT integral
    // int*         → NOT integral
    static_assert(std::is_integral_v<int>);
    static_assert(std::is_integral_v<const int>);
    static_assert(std::is_integral_v<volatile int>);
    static_assert(std::is_integral_v<const volatile int>);
    static_assert(!std::is_integral_v<int&>);
    static_assert(!std::is_integral_v<int*>);

    ProcessIntegralOnly(10);      // OK
    ProcessIntegralOnly(20u);     // OK
    ProcessIntegralOnly(true);     // OK
    ProcessIntegralOnly('A');     // OK
    ProcessIntegralOnly(wchar_t{'S'});     // OK
    // ProcessIntegralOnly(10.5);    // compile error
    // ProcessIntegralOnly("Hello!"); 

    int square = Square(100);
    // auto sqBool = Square(true); // compile error, allow any intergral type except bool

    Print(20); // print 21
    Print(true); // print 2
    Print(std::vector<int>(3)); // print size=3
    Print(userVasyl); // print size=4300
    auto nonIntergralPlayer = Player("NonIntergral");
    // Print(Player(nonIntergralPlayer)); // compile error: no member named 'size' in 'Player'
    std::println("");
    std::println("Reference: end");

    // auto uses template-like type deduction. With auto x = expr, references and top-level cv-qualifiers
    // are removed because the variable is deduced as a value. decltype(auto) instead applies decltype rules
    // to the initializer, which preserve the expression's value category. Since a function returning int&
    // produces an lvalue expression, decltype(Get()) is int&, so decltype(auto) preserves the reference.
    auto a = Get();           // int
    auto& b = Get();          // int&
    auto&& c = Get();         // int&   because reference collapsing
    decltype(auto) d = Get(); // int&
    // decltype(auto&) and decltype(auto&&), decltype(auto)&, decltype(auto)&& are not valid C++ syntax.
    decltype(Get())& a2 = Get();   // int&
    decltype(Get())&& b2 = Get();  // int&

    //It does not remove the const from the pointee:
    static_assert(
        std::is_same_v<
            std::remove_cvref_t<const int* const&>,
            const int*
        >
    );

    // remove_cvref_t vs decay_t
    static_assert(std::is_same_v< std::remove_cvref_t<const int&>, int >);       // int
    static_assert(std::is_same_v< std::decay_t<const int&>, int >);              // int

    static_assert(std::is_same_v<std::remove_cvref_t<const int* const&>, const int*>); // const int*
    static_assert(std::is_same_v<std::decay_t<const int* const&> , const int*>);       // const int*

    //But they differ for arrays and functions:
    using A = int[5];

    static_assert(std::is_same_v<std::remove_cvref_t<A>, int[5]>); // int[5]
    static_assert(std::is_same_v<std::decay_t<A>, int*>);         // int*

     // Array: HERE they differ
    using Array = int[5];

    static_assert(
        std::is_same_v<
            std::remove_cvref_t<Array>,
            int[5]
        >
    );

    static_assert(
        std::is_same_v<
            std::decay_t<Array>,
            int*
        >
    );

    // Function type: HERE they differ too
    using Function = void(int);

    static_assert(
        std::is_same_v<
            std::remove_cvref_t<Function>,
            void(int)
        >
    );

    static_assert(
        std::is_same_v<
            std::decay_t<Function>,
            void(*)(int)
        >
    );
    // remove_cvref_t:
    //     remove reference
    //     remove top-level const/volatile
    //     STOP

    // decay_t:
    //     remove reference
    //     remove top-level const/volatile
    //     +
    //     array -> pointer
    //     function -> function pointer

    using TFuncArray = void(int values[5]);
    using TFuncPointer = void(int* values);

    // The [5] disappears from the function parameter type.
    static_assert(
        std::is_same_v<
            TFuncArray,
            void(int*)
        >
    );

    // remove_cvref_t does not decay the function itself.
    static_assert(
        std::is_same_v<
            std::remove_cvref_t<TFuncArray>,
            void(int*)
        >
    );

    // decay_t DOES decay a function type into a function pointer.
    static_assert(
        std::is_same_v<
            std::decay_t<TFuncArray>,
            void(*)(int*)
        >
    );

    using F1 = void(int*);
    using F2 = void(int[]);
    using F3 = void(int[5]);

    static_assert(std::is_same_v<F1, F2>);
    static_assert(std::is_same_v<F1, F3>);

    return 0;
}


