# C++26 `std::variant` and `std::visit` — Interview / Study Notes

## 1. What is `std::variant`?

**Question:** What problem does `std::variant` solve?

`std::variant<Ts...>` is a **type-safe union**. It stores exactly one active value from a fixed set of alternative types.

```cpp
#include <print>
#include <string>
#include <variant>

using Value = std::variant<int, double, std::string>;

Value value = 42;

static_assert(std::variant_size_v<Value> == 3);

std::println("index={}", value.index()); // 0

value = std::string{"hello"};
std::println("index={}", value.index()); // 2
```

The set of alternatives is known at compile time; the active alternative is selected at runtime.

### Accessing the active value

```cpp
Value value = 42;

if (std::holds_alternative<int>(value))
{
    std::println("{}", std::get<int>(value));
}

if (const auto* p = std::get_if<int>(&value))
{
    std::println("{}", *p);
}
```

- `std::get<T>`: returns the value or throws `std::bad_variant_access`.
- `std::get_if<T>`: returns a pointer or `nullptr`.
- `std::holds_alternative<T>`: checks the active type.
- `index()`: returns the zero-based alternative index.

---

## 2. What is `std::visit`?

**Question:** Why use `std::visit` instead of manually checking every alternative?

`std::visit` calls a visitor with the currently active alternative.

```cpp
Value value = 42;

std::visit(
    [](const auto& v)
    {
        std::println("{}", v);
    },
    value
);
```

Conceptually:

```text
variant contains int         -> visitor(int)
variant contains double      -> visitor(double)
variant contains std::string -> visitor(std::string)
```

The visitor must be valid for every possible alternative that may be passed to it.

---

## 3. Type-specific logic with `if constexpr`

**Question:** How can a generic visitor behave differently for each alternative?

Use `decltype`, `std::remove_cvref_t`, and `if constexpr`.

```cpp
Value value = 42;

std::visit(
    [](const auto& v)
    {
        using T = std::remove_cvref_t<decltype(v)>;

        if constexpr (std::is_same_v<T, int>)
        {
            std::println("int: {}", v);
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            std::println("double: {}", v);
        }
        else if constexpr (std::is_same_v<T, std::string>)
        {
            std::println("string: {}", v);
        }
    },
    value
);
```

`if constexpr` is important because non-selected branches are discarded during template instantiation.

---

## 4. What is `operator()`?

**Question:** What does `operator()` mean, and why do lambdas have it?

`operator()` is the **function-call operator**. It makes an object callable like a function.

```cpp
struct Printer
{
    void operator()(int value) const
    {
        std::println("{}", value);
    }
};

Printer printer;

printer(42);            // normal syntax
printer.operator()(42); // equivalent explicit syntax
```

A lambda is an object of a compiler-generated unnamed **closure type**. That closure type contains `operator()`.

```cpp
auto printer = [](int value)
{
    std::println("{}", value);
};
```

Conceptually resembles:

```cpp
struct CompilerGeneratedLambdaType
{
    void operator()(int value) const
    {
        std::println("{}", value);
    }
};
```

Every lambda expression has its own unique closure type.

```cpp
auto a = [](int) {};
auto b = [](int) {};

static_assert(!std::is_same_v<decltype(a), decltype(b)>);
```

---

## 5. The `overloaded` visitor pattern

**Question:** What does this helper mean?

```cpp
template<class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
```

It:

1. inherits from every callable type in `Ts...`;
2. brings every inherited `operator()` into one overload set.

Example:

```cpp
auto visitor = overloaded{
    [](int v) { std::println("int: {}", v); },
    [](double v) { std::println("double: {}", v); },
    [](const std::string& v) { std::println("string: {}", v); }
};

Value value = 42;
std::visit(visitor, value);
```

If the lambda types were named `L1`, `L2`, and `L3`, the helper would roughly expand to:

```cpp
struct Visitor : L1, L2, L3
{
    using L1::operator();
    using L2::operator();
    using L3::operator();
};
```

---

## 6. `using` here is not a type alias

**Question:** Is `using A::operator();` the same `using` syntax as `using Id = int;`?

No. C++ reuses the keyword for different grammar constructs.

### Type alias

```cpp
using UserId = int;
```

Pattern:

```cpp
using NewName = ExistingType;
```

### Using-declaration

```cpp
struct Derived : Base
{
    using Base::Process;
};
```

Pattern:

```cpp
using Scope::name;
```

It introduces an existing name from another scope into the current scope.

For the visitor:

```cpp
using Ts::operator()...;
```

is a **pack expansion of using-declarations**.

If:

```text
Ts... = A, B, C
```

it expands conceptually to:

```cpp
using A::operator();
using B::operator();
using C::operator();
```

No new `operator()` functions are created.

---

## 7. Why is `using Base::operator()` needed with multiple inheritance?

Consider:

```cpp
struct A
{
    void operator()(int v) const
    {
        std::println("A: {}", v);
    }
};

struct E
{
    void operator()(const std::string& v) const
    {
        std::println("E: {}", v);
    }
};

struct C : A, E
{
};
```

`struct` inheritance is public by default:

```cpp
struct C : A, E {};
// same as:
struct C2 : public A, public E {};
```

For `class`, omitted base access is private.

Even though `A::operator()(int)` and `E::operator()(std::string)` have different signatures, unqualified lookup of `operator()` through `C` finds the same name in different base classes.

Use:

```cpp
struct C : A, E
{
    using A::operator();
    using E::operator();
};
```

Now normal overload resolution can select the correct function:

```cpp
C c;

c(42);                  // A::operator()(int)
c(std::string{"text"}); // E::operator()(std::string)
```

### Equivalent behavior without `using`

You could manually forward:

```cpp
struct C : A, E
{
    void operator()(int v) const
    {
        A::operator()(v);
    }

    void operator()(const std::string& v) const
    {
        E::operator()(v);
    }
};
```

But this does not scale to generic variadic lambda visitors, which is why the `using` form is ideal.

---

## 8. What type does an `overloaded` lambda visitor have?

**Question:** Can we avoid `auto` and directly write the visitor type?

Each lambda has a concrete but unnamed closure type. You normally cannot spell that type directly.

```cpp
auto intVisitor = [](int v)
{
    std::println("int: {}", v);
};

auto doubleVisitor = [](double v)
{
    std::println("double: {}", v);
};

overloaded<
    decltype(intVisitor),
    decltype(doubleVisitor)
> visitor{
    intVisitor,
    doubleVisitor
};
```

The actual type is:

```text
overloaded<type-of-intVisitor, type-of-doubleVisitor>
```

`auto` is normally cleaner:

```cpp
auto visitor = overloaded{
    [](int v) { std::println("int: {}", v); },
    [](double v) { std::println("double: {}", v); }
};
```

If fully named types are required, use named function objects:

```cpp
struct IntVisitor
{
    void operator()(int v) const
    {
        std::println("int: {}", v);
    }
};

struct DoubleVisitor
{
    void operator()(double v) const
    {
        std::println("double: {}", v);
    }
};

overloaded<IntVisitor, DoubleVisitor> visitor{};
```

---

## 9. C++26 member `variant::visit()`

C++17 introduced free `std::visit`.

```cpp
std::visit(visitor, value);
```

C++26 adds member visitation:

```cpp
value.visit(visitor);
```

Feature-test macro:

```cpp
#if __cpp_lib_variant >= 202306L
    value.visit(visitor);
#else
    std::visit(visitor, value);
#endif
```

Member `visit()` is mainly a syntax improvement for a single variant.

Free `std::visit` remains important because it can visit several variants simultaneously:

```cpp
std::variant<int, std::string> a = 10;
std::variant<int, std::string> b = std::string{"x"};

std::visit(
    [](const auto& x, const auto& y)
    {
        std::println("two active alternatives");
    },
    a,
    b
);
```

For two variants with two alternatives each, the visitor must support the relevant Cartesian product of combinations.

---

## 10. Returning values from `std::visit`

`std::visit` may return the visitor result.

```cpp
std::variant<int, double> value = 10;

double result = std::visit(
    [](auto v)
    {
        return static_cast<double>(v) * 2.0;
    },
    value
);

std::println("{}", result);
```

When the return type is deduced, all possible visitor invocations must satisfy the required consistent return-type rules.

An explicit return type can be requested:

```cpp
double result = std::visit<double>(
    [](auto v)
    {
        return v;
    },
    value
);
```

---

## 11. What types can `std::variant` store?

**Question:** Can `std::variant` contain `const`, references, pointers, arrays, or `void`?

Alternatives must be suitable object types satisfying the required destruction rules.

Valid examples:

```cpp
std::variant<int, double>
std::variant<const int, double>
std::variant<int*, double>
std::variant<const int*, double>
std::variant<std::string, int>
```

Not valid as direct alternatives:

```cpp
std::variant<int&, double>        // reference
std::variant<const int&, double>  // reference
std::variant<int&&, double>       // reference
std::variant<int[5], double>      // array
std::variant<void, double>        // void
```

A cv-qualified object type such as `const int` is allowed, but making an alternative `const` may disable some assignment operations. Usually prefer storing the ordinary value and controlling mutability at the access point.

---

## 12. How to store reference semantics in `std::variant`

This is invalid:

```cpp
int external = 10;
// std::variant<const int&, double> value = external; // invalid
```

Use `std::reference_wrapper`:

```cpp
#include <functional>

int external = 10;

std::variant<std::reference_wrapper<const int>, double> value = std::cref(external);

std::visit(
    [](const auto& v)
    {
        using T = std::remove_cvref_t<decltype(v)>;

        if constexpr (std::is_same_v<T, std::reference_wrapper<const int>>)
        {
            std::println("{}", v.get());
        }
    },
    value
);
```

Or use a pointer when nullable semantics are appropriate:

```cpp
int external = 10;
std::variant<const int*, double> value = &external;
```

---

## 13. Why does `remove_cvref_t` preserve `const int*`?

Consider:

```cpp
int external = 10;
std::variant<const int*, double> value = &external;

std::visit(
    [](const auto& v)
    {
        using T = std::remove_cvref_t<decltype(v)>;

        if constexpr (std::is_same_v<T, const int*>)
        {
            static_assert(std::is_same_v<decltype(v), const int* const&>);
            static_assert(std::is_same_v<T, const int*>);

            std::println("{}", *v);
        }
    },
    value
);
```

For the pointer alternative:

```text
decltype(v)
= const int* const&
```

There are two different cv levels:

```text
const int* const&
^^^^^      ^^^^^
pointee    pointer/reference layer
```

`std::remove_cvref_t` removes the reference and **top-level** cv qualifiers:

```text
const int* const&
       ↓
const int* const
       ↓
const int*
```

It does not recursively remove `const` from the pointee.

---

## 14. `std::remove_cvref_t` vs `std::decay_t`

**Question:** Which should be the default in modern C++26 generic code?

Prefer `std::remove_cvref_t` when the intention is:

> Ignore only top-level `const` / `volatile` / reference qualifiers.

Use `std::decay_t` when you intentionally want the additional by-value-like transformations:

- array → pointer;
- function → function pointer.

### Ordinary types: same result

```cpp
static_assert(std::is_same_v<std::remove_cvref_t<const int&>, int>);
static_assert(std::is_same_v<std::decay_t<const int&>, int>);
static_assert(std::is_same_v<std::remove_cvref_t<const int* const&>, const int*>);
static_assert(std::is_same_v<std::decay_t<const int* const&>, const int*>);
```

### Arrays: different result

```cpp
using Array = int[5];

static_assert(std::is_same_v<std::remove_cvref_t<Array>, int[5]>);
static_assert(std::is_same_v<std::decay_t<Array>, int*>);
```

### Functions: different result

```cpp
using Function = void(int);

static_assert(std::is_same_v<std::remove_cvref_t<Function>, void(int)>);
static_assert(std::is_same_v<std::decay_t<Function>, void(*)(int)>);
```

### Practical rule

| Intent | Prefer |
|---|---|
| Compare a deduced type ignoring cv/ref | `std::remove_cvref_t` |
| `std::visit` type inspection | `std::remove_cvref_t` |
| Concepts / type traits where array/function identity matters | `std::remove_cvref_t` |
| Intentionally apply array-to-pointer and function-to-pointer transformation | `std::decay_t` |
| Model normal by-value parameter decay | `std::decay_t` |

---

## 15. Function parameters written as arrays

**Question:** What does this declaration mean?

```cpp
void Foo(int values[5]);
```

In a function parameter list, an array parameter is adjusted to a pointer parameter.

These declarations have the same function parameter type:

```cpp
void Foo(int* values);
void Foo(int values[]);
void Foo(int values[5]);
```

The `5` is **not enforced** by the function type.

```cpp
void Foo(int values[5])
{
    values[0] = 42;
}

int a[5]{};
int b[2]{};

Foo(a); // OK
Foo(b); // also compiles; parameter is really int*
```

For new C++ code, do not use `int values[5]` to imply a required size.

If the compile-time extent matters, use an array reference or `std::span`:

```cpp
void Foo(int (&values)[5])
{
    values[0] = 42;
}
```

or:

```cpp
#include <span>

void Foo(std::span<int, 5> values)
{
    values[0] = 42;
}
```

---

## 16. Function type containing an array-looking parameter

**Question:** What happens with `decay_t` and `remove_cvref_t` here?

```cpp
using TFuncArray = void(int values[5]);
```

The array parameter adjustment happens **when the function type is formed**:

```cpp
static_assert(std::is_same_v<TFuncArray, void(int*)>);
```

Then:

```cpp
static_assert(std::is_same_v<std::remove_cvref_t<TFuncArray>, void(int*)>);
static_assert(std::is_same_v<std::decay_t<TFuncArray>, void(*)(int*)>);
```

Transformation:

```text
written:
    void(int values[5])

function parameter adjustment:
    void(int*)

remove_cvref_t:
    void(int*)

decay_t:
    void(*)(int*)
```

Important: the `[5]` is already gone **before** either type trait processes `TFuncArray`.

---

## 17. `std::variant` vs virtual polymorphism

`std::variant` works well for a **closed set** of types:

```cpp
struct Circle {};
struct Rectangle {};
struct Triangle {};

using Shape = std::variant<Circle, Rectangle, Triangle>;
```

Virtual polymorphism works well for an **open hierarchy**:

```cpp
struct Shape
{
    virtual ~Shape() = default;
    virtual void Draw() const = 0;
};
```

Compact comparison:

```text
std::variant
- closed set of alternatives
- alternatives known at compile time
- value semantics are natural
- runtime selection + compile-time-known types
- std::visit for dispatch

virtual interface
- open set of derived implementations
- runtime polymorphism through base pointer/reference
- adding a new derived type usually does not change a central variant list
```

---

## 18. Complete compact study example

```cpp
#include <functional>
#include <print>
#include <string>
#include <type_traits>
#include <variant>

template<class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};

int main()
{
    using Value = std::variant<int, double, std::string>;

    Value value = 42;

    auto visitor = overloaded{
        [](int v) { std::println("int: {}", v); },
        [](double v) { std::println("double: {}", v); },
        [](const std::string& v) { std::println("string: {}", v); }
    };

    std::visit(visitor, value);

#if defined(__cpp_lib_variant) && __cpp_lib_variant >= 202306L
    value.visit(visitor); // C++26
#endif

    std::visit(
        [](const auto& v)
        {
            using T = std::remove_cvref_t<decltype(v)>;

            if constexpr (std::is_same_v<T, int>)
            {
                static_assert(std::is_same_v<decltype(v), const int&>);
                std::println("generic visitor int: {}", v);
            }
        },
        value
    );

    int external = 10;
    std::variant<const int*, double> pointerVariant = &external;

    std::visit(
        [](const auto& v)
        {
            using T = std::remove_cvref_t<decltype(v)>;

            if constexpr (std::is_same_v<T, const int*>)
            {
                static_assert(std::is_same_v<decltype(v), const int* const&>);
                static_assert(std::is_same_v<T, const int*>);
                std::println("pointer variant: {}", *v);
            }
        },
        pointerVariant
    );

    std::variant<std::reference_wrapper<const int>, double> refVariant = std::cref(external);

    std::visit(
        [](const auto& v)
        {
            using T = std::remove_cvref_t<decltype(v)>;

            if constexpr (std::is_same_v<T, std::reference_wrapper<const int>>)
            {
                std::println("reference variant: {}", v.get());
            }
        },
        refVariant
    );

    using Array = int[5];
    using Function = void(int);
    using TFuncArray = void(int values[5]);

    static_assert(std::is_same_v<std::remove_cvref_t<Array>, int[5]>);
    static_assert(std::is_same_v<std::decay_t<Array>, int*>);

    static_assert(std::is_same_v<std::remove_cvref_t<Function>, void(int)>);
    static_assert(std::is_same_v<std::decay_t<Function>, void(*)(int)>);

    static_assert(std::is_same_v<TFuncArray, void(int*)>);
    static_assert(std::is_same_v<std::remove_cvref_t<TFuncArray>, void(int*)>);
    static_assert(std::is_same_v<std::decay_t<TFuncArray>, void(*)(int*)>);
}
```

---

## Interview answer in ~30 seconds

`std::variant<Ts...>` is a type-safe union that stores one active value from a fixed set of types. `std::visit` performs type-safe runtime dispatch to a callable based on the active alternative.

A common visitor helper is:

```cpp
template<class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
```

Each lambda is an unnamed closure type with `operator()`. `overloaded` inherits from all lambda types and uses variadic using-declarations to expose their call operators as one overload set.

For generic visitor type inspection, prefer:

```cpp
using T = std::remove_cvref_t<decltype(value)>;
```

Use `std::decay_t` only when array-to-pointer and function-to-pointer decay is also intended.

C++26 adds member:

```cpp
variant.visit(visitor);
```

while free `std::visit` remains necessary for multi-variant visitation.

---

## Compact mental map

```text
std::variant<Ts...>
    |
    +-- one active alternative
    |
    +-- std::get / get_if / holds_alternative
    |
    +-- std::visit(visitor, variant)
            |
            +-- generic lambda + if constexpr
            |
            +-- overloaded{ lambdas... }
                    |
                    +-- each lambda = unique closure type
                    +-- each closure has operator()
                    +-- inherit from all closures
                    +-- using Ts::operator()...
                    +-- one overload set

type inspection:
    decltype(value)
        |
        +-- remove_cvref_t -> strips top-level cv/ref only
        |
        +-- decay_t -> also array->pointer, function->pointer
```

---

## Core points to remember

- `std::variant` is a closed, type-safe union.
- It stores one active alternative at a time.
- Direct reference alternatives (`T&`, `const T&`, `T&&`) are not allowed.
- Use `std::reference_wrapper<T>` or a pointer for reference semantics.
- A lambda is an object of a unique unnamed closure type.
- Lambda call syntax works through `operator()`.
- `using Base::operator();` is a using-declaration, not a type alias.
- `using Ts::operator()...;` expands one using-declaration per base.
- `struct` uses public inheritance by default; `class` uses private inheritance by default.
- `std::remove_cvref_t` removes only top-level cv/ref.
- `std::decay_t` additionally performs array/function decay.
- `void Foo(int values[5])` really has an `int*` parameter.
- C++26 adds unary member `variant::visit()`.

---

## References

- [`std::variant`](https://en.cppreference.com/w/cpp/utility/variant.html)
- [`std::visit`](https://en.cppreference.com/w/cpp/utility/variant/visit2.html)
- [C++26 `std::variant::visit`](https://en.cppreference.com/w/cpp/utility/variant/visit.html)
- [Using-declaration](https://en.cppreference.com/w/cpp/language/using_declaration.html)
- [Lambda expressions / closure type](https://en.cppreference.com/w/cpp/language/lambda.html)
- [`std::remove_cvref`](https://en.cppreference.com/w/cpp/types/remove_cvref.html)
- [`std::decay`](https://en.cppreference.com/w/cpp/types/decay.html)
- [Function declaration / parameter adjustment](https://en.cppreference.com/w/cpp/language/function.html)
