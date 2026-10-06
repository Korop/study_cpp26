#include <concepts>
#include <iostream>
#include <vector>
#include <string>
#include <print>

template<typename T>
concept Container =
    requires(T container)
    {
        // Nested type "value_type" must exist.
        typename T::value_type;
        // “Can an object of T be legally and non-throwingly destroyed here?”
        requires std::destructible<T>;

        container.begin();
        container.end();

        {
            // Nested requirement
            container.size()
            
        } -> std::convertible_to<std::size_t>;
        // Expression must exist and satisfy return-type constraint.
    };

template<Container T>
void PrintContainer(const T& container)
{
    for (const auto& value : container)
    {
        std::cout << value << '\n';
    }
};

struct BedContainer 
{
    using value_type = int;

    int data[4]{10, 20, 30, 40};

    const int* begin() const
    {
        return data;
    }

    const int* end() const
    {
        return data + 4;
    }

    // std::size_t size() const noexcept
    std::size_t size() const
    {
        return 4;
    }

        static BedContainer* Create()
    {
        return new BedContainer{};
    }

    static void Destroy(BedContainer* value)
    {
        delete value;
    }

    private:
        BedContainer() = default;
        ~BedContainer() noexcept(true) = default;
};
//--------------
//Don't create custom concepts where the standard already describes the semantic requirement you need.
template<std::ranges::range R>
void Print(const R& range)
{
    for (const auto& value : range)
    {
        std::cout << value;
    }
};
///---------------------------
template<typename R>    
requires std::ranges::range<const R>
void PrintRange(const R& range)
{
    for (const auto& value : range)
    {
        std::println("{}", value);
    }
}
////
template<typename T>
concept ContainerDestructible =
    std::destructible<T> &&
    requires(T container)
    {
        typename T::value_type;

        container.begin();
        container.end();

        {
            container.size()
        } -> std::convertible_to<std::size_t>;
    };
template<typename T>
concept ContainerDestructible2 =
    requires(T container)
    {
        // nested requirement
        //  Воно перевіряє значення constraint: std::destructible<T> == true
        requires std::destructible<T>;

        typename T::value_type;

        container.begin();
        container.end();

        // Чому container.size() у { ... }?
        // Тому що ти перевіряєш не лише існування expression, а ще й тип результату.
        {
            container.size()
        } -> std::convertible_to<std::size_t>;
    };

template<typename T>
concept ContainerGood =
    requires(T container)
    {
        // 1. Type requirement
        // Does this nested type exist?
        typename T::value_type;

        // 2. Simple requirement
        // Can this expression compile?
        container.begin();
        container.end();

        // 3. Compound requirement with return type check
        // Can this expression compile AND
        // is its result convertible to size_t?
        {
            container.size()
        } -> std::convertible_to<std::size_t>;

        {
            container.size()
        } noexcept;

        // 4. Nested requirement
        // Is this constraint actually true?
        requires std::destructible<T>; // check expression std::destructible<T> == true
    };

template<ContainerGood T>
void PrintContainerGood(const T& container)
{
    for (const auto& value : container)
    {
        std::cout << value << '\n';
    }
};
//// Any type supporting +
template<typename T>
concept Addable =
    requires(T a, T b)
    {
        a + b;
    };

template<Addable T>
T Add(T a, T b)
{
    return a + b;
}

//// concept HasGetId
// value.GetId()
// must exist
// and result must be exactly int
template<typename T>
concept HasGetId =
    requires(const T& value)
    {
        // value.GetId();
        {
            value.GetId()
        } -> std::same_as<int>;
    };

struct Player
{
    int GetId() const
    {
        return 42;
    }
};

template<HasGetId T>
void PrintId(const T& value)
{
    std::cout << "Id: " << value.GetId();
}
//////
template<typename T>
concept HasSize =
    requires(const T& value)
    {
        {
            value.size()
        } -> std::convertible_to<std::size_t>;
    };
////Combine concepts with &&
template<typename T>
concept Numeric =
    std::integral<T>
    || std::floating_point<T>;

template<Numeric T>
T Square(T value)
{
    return value * value;
}    
///// requires expression versus requires clause
// They can be combined directly
template<typename T>
requires requires(T value)
//^^^^^^^ ^^^^^^^^
// clause   expression
{
    value.size();
}
void Process2(const T& value) //function
{
}
// Equivalent idea: 
template<typename T>
requires HasSize<T>
void Process3(const T& value)
{
}

//You can even have multiple clauses
template<typename T>
requires std::copy_constructible<T>   // requires-clause
void Process(T value)
    requires                          // another requires-clause
        requires(T x)                 // requires-expression
        {
            requires sizeof(T) > 4;
            x.size();
        }
{
}

///
template<typename T>
requires requires(T x) { x.size(); }
void func(T x);
///
struct PlayerH
{
    int health;
};

// Concept with member variables
template<typename T>
concept HasHealth =
    requires(T value)
    {
        value.health;
    };

// Works for anything exposing health.
template<HasHealth T>
void Kill(T& value)
{
    value.health = 0;
}
// Concept with operator requirements
template<typename T>
concept Comparable =
    requires(const T& a, const T& b)
    {
        {
            a == b
        } -> std::convertible_to<bool>;

        {
            a < b
        } -> std::convertible_to<bool>;
    };

template<Comparable T>
bool IsSmaller(const T& a, const T& b)
{
    return a < b;
}
///
template<std::integral T>
void Print(T value)
{
}
// new form --> convenient for small generic functions
void PrintShot(std::integral auto value)
{
}
/// a and b do not necessarily have the same type.
auto AddIntegral( std::integral auto a, std::integral auto b )
{
    return a + b;
}

// If you want exactly the same type, use one template parameter:
template<std::integral T>
T AddSameType(T a, T b)
{
    return a + b;
}

///Concepts participate in overload resolution, which is much nicer than many older SFINAE tricks.
template<typename T>
void PrintOve(T value)
{
    std::cout << "generic";
}

template<std::integral T>
void PrintOve(T value)
{
    std::cout << "integral";
}

// A simple C++23/26 compatible file
int main() {
    std::vector vec{1, 2, 3};
    PrintContainer(vec);

    static_assert(std::ranges::range<BedContainer>);
    BedContainer& bc = *BedContainer::Create();
    static_assert(!std::destructible<BedContainer>); 
    PrintRange(bc);
    
    // PrintContainerGood(bc); // compile error: because 'container.size()' may throw an exception
    // PrintContainer(bc); // there is not destrutor inBedContainer, because 'BedContainer' does not satisfy 'destructible'
    BedContainer::Destroy(&bc);

    // Addable concept
    std::println("int sum={}", Add(10, 20));        // OK
    std::println("double sum={}", Add(1.55, 2.05));        // OK


    ///
    Player p;
    PrintId(p);

    // all satisfy the idea of having .size() convertible to size_t.
    static_assert( HasSize<std::string>);
    static_assert( HasSize<std::array<int, 10>>);
    static_assert( HasSize<std::vector<int>>);
    std::array<int, 10> arr10;
    static_assert(arr10.size() == 10);

    Square(10);
    Square(2.5);
    // Square(std::string{"123"}); //compiler error: no matching function for call to 'Square'
    //Constrained auto
    std::integral auto value = 10;
    // std::integral auto value = 10.5; //Invalid. compiler error


    auto sum33 = AddIntegral(int{10}, long{20});
    static_assert(std::is_same_v<decltype(sum33), long>);


    std::println("");
    //The constrained overload is more specific:
    PrintOve(10);
    std::println("");
    PrintOve(3.14);

    std::println("");
    return 0;
}

// | Syntax                           | What it is          | Purpose                                              |
// | -------------------------------- | ------------------- | ---------------------------------------------------- |
// | `requires Condition`             | requires-clause     | Constrains a declaration                             |
// | `requires(T x) { ... }`          | requires-expression | Tests validity/capabilities                          |
// | `requires requires(T x) { ... }` | clause + expression | Uses a requires-expression as the clause's condition |

