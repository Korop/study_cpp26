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
    std::println("");
    return 0;
}


