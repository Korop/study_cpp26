// using_task.cpp
// C++23 / C++26
//
// Interview study:
// Main forms of `using` in modern C++.

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

// ============================================================
// 1. TYPE ALIAS
//    using NewName = ExistingType;
// ============================================================

using UserId = unsigned long long;
using StringVector = std::vector<std::string>;


// ============================================================
// 2. ALIAS TEMPLATE
//    A generic type alias.
// ============================================================

template<typename T>
using Vector = std::vector<T>;

template<typename T>
using ConstRef = const T&;


// ============================================================
// Test namespace for namespace-related using examples
// ============================================================

namespace app
{
    inline int version = 26;

    void print_version()
    {
        std::cout << "app::version = " << version << '\n';
    }

    void hello()
    {
        std::cout << "Hello from app\n";
    }
}


// ============================================================
// 3. BASE CLASS MEMBER USING-DECLARATION
//    using Base::foo;
//
// Important:
// foo() is already inherited.
// `using` makes the Base overloads visible in Derived's
// overload set.
// ============================================================

class Base
{
public:
    void foo(int value)
    {
        std::cout << "Base::foo(int): " << value << '\n';
    }

    void foo(double value)
    {
        std::cout << "Base::foo(double): " << value << '\n';
    }
};


class Derived : public Base
{
public:
    // Without this declaration, Derived::foo(string_view)
    // would hide Base::foo(int) and Base::foo(double).
    using Base::foo;

    void foo(std::string_view value)
    {
        std::cout << "Derived::foo(string_view): "
                  << value << '\n';
    }
};


// ============================================================
// 4. INHERITING CONSTRUCTORS
//    using Base::Base;
// ============================================================

class Person
{
public:
    explicit Person(std::string name)
        : name_{std::move(name)}
    {
        std::cout << "Person(string)\n";
    }

    Person(std::string name, int age)
        : name_{std::move(name)},
          age_{age}
    {
        std::cout << "Person(string, int)\n";
    }

    void print() const
    {
        std::cout
            << "name = " << name_
            << ", age = " << age_
            << '\n';
    }

private:
    std::string name_;
    int age_{};
};


class Developer : public Person
{
public:
    // Inherit Person constructors.
    using Person::Person;

    void code() const
    {
        std::cout << "Developer is coding\n";
    }
};


// ============================================================
// 5. using enum
//    C++20+
//
// Introduces enum enumerators into the current scope.
// ============================================================

enum class Status
{
    Ready,
    Running,
    Finished
};

void enum_example(Status status)
{
    // Instead of:
    //
    // case Status::Ready:
    //
    // we can introduce the enumerators.

    using enum Status;

    switch (status)
    {
        case Ready:
            std::cout << "Ready\n";
            break;

        case Running:
            std::cout << "Running\n";
            break;

        case Finished:
            std::cout << "Finished\n";
            break;
    }
}


// ============================================================
// 6. using with dependent base class
//
// Useful in templates.
// ============================================================

template<typename T>
struct ContainerBase
{
    using value_type = T;

    void process(const T& value)
    {
        std::cout << "ContainerBase::process(): "
                  << value << '\n';
    }
};


template<typename T>
struct Container : ContainerBase<T>
{
    // Bring type from dependent base class into this scope.
    using typename ContainerBase<T>::value_type;

    // Bring function from dependent base class into this scope.
    using ContainerBase<T>::process;

    void test(value_type value)
    {
        process(value);
    }
};


// ============================================================
// MAIN
// ============================================================

int main()
{
    std::cout << "\n=== 1. Type alias ===\n";

    UserId id = 123456;
    StringVector names{"Ivan", "Anna"};

    std::cout << "UserId: " << id << '\n';
    std::cout << "names[0]: " << names[0] << '\n';


    // --------------------------------------------------------
    // 2. Alias template
    // --------------------------------------------------------

    std::cout << "\n=== 2. Alias template ===\n";

    Vector<int> numbers{1, 2, 3};

    ConstRef<std::vector<int>> numbersRef = numbers;

    std::cout << "numbersRef[0]: "
              << numbersRef[0] << '\n';


    // --------------------------------------------------------
    // 3. using namespace member
    // --------------------------------------------------------

    std::cout << "\n=== 3. using std::cout ===\n";

    {
        // Using-declaration.
        //
        // Introduces ONE specific name into this scope.
        using std::cout;

        cout << "No std:: prefix required here\n";
    }


    // --------------------------------------------------------
    // 4. using namespace X
    // --------------------------------------------------------

    std::cout << "\n=== 4. using namespace app ===\n";

    {
        // Using-directive.
        //
        // Makes names from namespace app available for
        // unqualified lookup in this scope.
        using namespace app;

        hello();
        print_version();

        std::cout << "version = " << version << '\n';
    }

    // Prefer avoiding:
    //
    // using namespace std;
    //
    // especially at global scope and in header files.


    // --------------------------------------------------------
    // 5. using Base::foo
    // --------------------------------------------------------

    std::cout << "\n=== 5. using Base::foo ===\n";

    Derived derived;

    derived.foo(10);
    // Base::foo(int)

    derived.foo(3.14);
    // Base::foo(double)

    derived.foo("hello");
    // Derived::foo(string_view)


    // --------------------------------------------------------
    // 6. using Base::Base
    // --------------------------------------------------------

    std::cout << "\n=== 6. using Person::Person ===\n";

    Developer developer1{"Ivan"};

    Developer developer2{"Anna", 35};

    developer1.print();
    developer2.print();

    developer1.code();


    // --------------------------------------------------------
    // 7. using enum
    // --------------------------------------------------------

    std::cout << "\n=== 7. using enum ===\n";

    enum_example(Status::Running);


    // --------------------------------------------------------
    // 8. dependent-base using
    // --------------------------------------------------------

    std::cout << "\n=== 8. Template dependent-base using ===\n";

    Container<int> container;

    container.test(42);


    // --------------------------------------------------------
    // Interview summary
    // --------------------------------------------------------

    std::cout << "\n=== SUMMARY ===\n";

    std::cout <<
        R"(
1. using UserId = unsigned long long;
   -> type alias

2. template<typename T>
   using Vector = std::vector<T>;
   -> alias template

3. using std::cout;
   -> using-declaration
   -> introduce one namespace member

4. using namespace app;
   -> using-directive
   -> names from namespace participate in unqualified lookup

5. using Base::foo;
   -> using-declaration
   -> makes inherited Base::foo overloads visible

6. using Base::Base;
   -> inherit Base constructors

7. using enum Status;
   -> introduce enum enumerators into current scope

8. using typename Base<T>::value_type;
   -> introduce a dependent type from a template base class
)";

    return 0;
}