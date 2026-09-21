# C++26 `auto` Type Deduction — Study Notes

`auto` is a compile-time placeholder type. Its deduction mostly follows function-template argument deduction rules.

Mental model:

```cpp
auto x = expr;        // like T value
auto& x = expr;       // like T&
const auto& x = expr; // like const T&
auto&& x = expr;      // like forwarding T&&
```

`decltype(auto)` is different: it follows `decltype` rules.

---

## 1. Plain `auto`

Plain `auto` behaves like deduction for a by-value template parameter:

```cpp
template<typename T>
void Foo(T value);
```

It removes references and top-level `const` / `volatile`:

```cpp
const int value = 42;
const int& ref = value;

auto a = value; // int
auto b = ref;   // int
```

Low-level `const` is preserved:

```cpp
const int* p = &value;
auto q = p; // const int*
```

---

## 2. `auto&` and `const auto&`

```cpp
int x = 10;
const int cx = 20;

auto& a = x;        // int&
auto& b = cx;       // const int&
const auto& c = x;  // const int&
```

`const auto&` is useful for read-only access without copying.

---

## 3. `auto&&` and forwarding references

For a normal initializer, `auto&&` is a forwarding reference:

```cpp
int x = 10;

auto&& a = x;   // int&
auto&& b = 42;  // int&&
```

The same rule applies to:

```cpp
template<typename T>
void Foo(T&& value);
```

For an lvalue argument of type `U`, forwarding-reference deduction has a special rule:

```text
T = U&
```

Then reference collapsing applies:

```text
T&  &  -> T&
T&  && -> T&
T&& &  -> T&
T&& && -> T&&
```

Example:

```cpp
int x = 10;

Foo(x);            // T = int&, parameter = int&
Foo(42);           // T = int,  parameter = int&&
Foo(std::move(x)); // T = int,  parameter = int&&
```

Important: `T` becomes `U&` for an lvalue because this is a special forwarding-reference deduction rule.

---

## 4. `const auto&&` is not a forwarding reference

```cpp
const auto&& value = 42; // const int&&
```

A forwarding reference requires a CV-unqualified deduced type.

```cpp
int x = 10;
// const auto&& value = x; // error
```

---

## 5. `auto&&` + `{}` special trap

A braced initializer list is not an ordinary expression with its own type.

```cpp
auto&& values = {1, 2, 3};
```

This does **not** use normal forwarding-reference deduction. `auto` has a special initializer-list rule and deduces:

```cpp
std::initializer_list<int>&&
```

So the result is an rvalue reference, but **not a forwarding reference**.

Normal template deduction cannot do the same:

```cpp
template<typename T>
void Foo(T&&);

// Foo({1, 2, 3}); // cannot deduce T
```

---

## 6. Brace initialization

```cpp
auto a = {1, 2, 3}; // std::initializer_list<int>
auto b{1};          // int

// auto c{1, 2};    // error
// auto d = {1, 2.0}; // error
```

`auto` does not calculate a common type for mixed initializer-list elements.

---

## 7. Arrays and functions

Plain `auto` performs decay:

```cpp
int values[5]{};

auto a = values;  // int*
auto& b = values; // int (&)[5]
```

Functions behave similarly:

```cpp
void Process(int);

auto f1 = Process;  // void (*)(int)
auto& f2 = Process; // void (&)(int)
```

---

## 8. `auto` return type

`auto` return deduction is template-like:

```cpp
int value = 10;

int& Get()
{
    return value;
}

auto Foo()
{
    return Get();
}
```

`Foo()` returns `int`, not `int&`.

All return statements must deduce the same type:

```cpp
auto Good(bool flag)
{
    if (flag)
    {
        return 1;
    }

    return 2;
}
```

This is invalid:

```cpp
// auto Bad(bool flag)
// {
//     if (flag)
//     {
//         return 1;
//     }
//
//     return 2.0;
// }
```

---

## 9. `decltype(auto)`

`decltype(auto)` follows `decltype` rules and can preserve references:

```cpp
decltype(auto) Foo()
{
    return Get();
}
```

Return type: `int&`.

Parentheses matter:

```cpp
int x = 10;

decltype(auto) a = x;   // int
decltype(auto) b = (x); // int&
```

`decltype(auto)` must be the complete placeholder type:

```cpp
// const decltype(auto)& x = ...; // invalid
```

---

## 10. Function parameters with `auto`

Since C++20:

```cpp
void Print(auto value)
{
}
```

is an abbreviated function template, conceptually:

```cpp
template<typename T>
void Print(T value)
{
}
```

Reference forms match normal template rules:

```cpp
void A(auto value);        // T
void B(auto& value);       // T&
void C(const auto& value); // const T&
void D(auto&& value);      // forwarding T&&
```

---

## 11. Constrained `auto`

```cpp
void Process(std::integral auto value)
{
    std::println("integral={}", value);
}
```

The type is deduced and then checked against the concept.

---

## 12. Generic lambdas

```cpp
auto twice = [](auto value)
{
    return value * 2;
};
```

The generated call operator behaves like a function template.

```cpp
auto forward = [](auto&& value)
{
    // forwarding-reference deduction
};
```

---

## 13. Structured bindings

```cpp
std::pair<int, std::string> player{1, "Ivan"};

auto [id1, name1] = player;        // copies
auto& [id2, name2] = player;       // references
const auto& [id3, name3] = player; // const references
```

---

## 14. Range-for

```cpp
for (auto value : values)        // copy
{
}

for (auto& value : values)       // mutable reference
{
}

for (const auto& value : values) // read-only reference
{
}

for (auto&& value : values)      // generic/reference-friendly
{
}
```

---

## 15. `auto` does not mean move

```cpp
std::string name{"Ivan"};

auto a = std::move(name); // std::string
```

`a` is a value. `std::move` changes the value category of the initializer, not the deduced type.

```cpp
auto&& b = std::move(name); // std::string&&
```

---

## 16. Pointer deduction

```cpp
const int value = 42;

auto p1 = &value;  // const int*
auto* p2 = &value; // const int*
```

`auto*` also documents that the initializer must be a pointer.

---

## 17. Main restrictions

No initializer means no deduction:

```cpp
// auto x; // invalid
```

Multiple declarations must deduce the same placeholder type:

```cpp
auto a = 1, b = 2;      // OK
// auto c = 1, d = 2.0; // error
```

Plain non-static data members cannot simply use `auto`:

```cpp
struct Player
{
    // auto score = 100; // invalid
    static inline auto maxScore = 100; // OK
};
```

---

## 18. Proxy-type trap

`auto` deduces the actual returned type, not the conceptual value type.

```cpp
std::vector<bool> flags{true, false};

auto value = flags[0]; // proxy type, not necessarily bool
```

If `bool` is required:

```cpp
bool value = flags[0];
```

---

# Compact C++26 Sample

```cpp
#include <concepts>
#include <initializer_list>
#include <print>
#include <string>
#include <type_traits>
#include <utility>

int globalValue{10};

int& GetValue()
{
    return globalValue;
}

template<typename T>
void Forward(T&& value)
{
    std::println(
        "T: lvalue-ref={}, rvalue-ref={}",
        std::is_lvalue_reference_v<T>,
        std::is_rvalue_reference_v<T>
    );
}

void Process(std::integral auto value)
{
    std::println("integral={}", value);
}

int main()
{
    const int cx{20};
    int x{10};
    int values[3]{1, 2, 3};

    auto a = cx;                 // int
    auto& b = cx;                // const int&
    auto&& c = x;                // int&
    auto&& d = 42;               // int&&
    auto e = values;             // int*
    auto& f = values;            // int (&)[3]
    auto list = {1, 2, 3};       // initializer_list<int>
    auto&& listRef = {1, 2, 3};  // initializer_list<int>&&, not forwarding
    auto g = GetValue();          // int
    decltype(auto) h = GetValue();// int&

    static_assert(std::is_same_v<decltype(a), int> && std::is_same_v<decltype(b), const int&> && std::is_same_v<decltype(c), int&> && std::is_same_v<decltype(d), int&&> && std::is_same_v<decltype(e), int*> && std::is_same_v<decltype(f), int (&)[3]> && std::is_same_v<decltype(list), std::initializer_list<int>> && std::is_same_v<decltype(listRef), std::initializer_list<int>&&> && std::is_same_v<decltype(g), int> && std::is_same_v<decltype(h), int&>);

    Forward(x);            // T = int&
    Forward(42);           // T = int
    Forward(std::move(x)); // T = int

    Process(42);

    std::println(
        "a={}, b={}, c={}, d={}, first={}, h={}",
        a,
        b,
        c,
        d,
        e[0],
        h
    );
}
```

---

# Review Questions

## 1. What rules does plain `auto` mostly follow?

Function-template argument deduction for a by-value parameter.

## 2. What does plain `auto` remove?

References and top-level CV qualifiers.

## 3. Does plain `auto` remove low-level `const`?

No.

```cpp
const int* p{};
auto q = p; // const int*
```

## 4. What does `auto&` preserve?

Reference semantics and CV qualification of the referred object.

## 5. When is `auto&&` a forwarding reference?

When it is deduced from a normal initializer and `auto` is CV-unqualified.

## 6. Why does `Foo(x)` deduce `T = int&` for `template<typename T> void Foo(T&&)`?

Because forwarding-reference deduction has a special rule: for an lvalue argument of type `U`, `T` is deduced as `U&`.

Then:

```text
T&& -> int& && -> int&
```

## 7. What does `T` become for an rvalue passed to `Foo(T&&)`?

Usually the non-reference type:

```cpp
Foo(42); // T = int
```

so the parameter becomes `int&&`.

## 8. Why is `const auto&&` not a forwarding reference?

A forwarding reference requires a CV-unqualified deduced type.

## 9. Why is `auto&& x = {1,2,3}` not a forwarding reference?

Because `{1,2,3}` has no ordinary expression type. `auto` uses its special initializer-list rule and produces `std::initializer_list<int>&&`.

## 10. Why does `Foo({1,2,3})` fail for `template<typename T> void Foo(T&&)`?

Normal template deduction cannot deduce `T` directly from a braced initializer list.

## 11. Difference between these?

```cpp
auto a = {1};
auto b{1};
```

Result:

```cpp
a // std::initializer_list<int>
b // int
```

## 12. What happens to an array with plain `auto`?

It decays to a pointer.

```cpp
int a[3]{};
auto x = a;  // int*
auto& y = a; // int (&)[3]
```

## 13. What happens to a function name with plain `auto`?

It decays to a function pointer. `auto&` preserves a function reference.

## 14. Difference between `auto` and `decltype(auto)`?

`auto` uses template-like deduction. `decltype(auto)` uses `decltype` rules and can preserve references exactly.

## 15. Why can parentheses change `decltype(auto)`?

```cpp
decltype(auto) a = x;   // decltype(x)
decltype(auto) b = (x); // decltype((x))
```

For lvalue `(x)`, the second form produces `T&`.

## 16. Does `auto x = std::move(value)` make `x` an rvalue reference?

No. `x` is a value. `std::move` only changes the value category of the initializer.

## 17. What is `void Foo(auto value)`?

An abbreviated function template.

## 18. What does constrained `auto` do?

It deduces the type and checks it against a concept.

```cpp
void Foo(std::integral auto value);
```

## 19. What is a common proxy-type trap?

Some APIs return proxy objects rather than the conceptual value type.

```cpp
std::vector<bool> values;
auto x = values[0];
```

`x` may not be `bool`.

## 20. Give a short interview definition of `auto`.

`auto` is a compile-time placeholder whose type is deduced mostly using function-template argument deduction rules. Plain `auto` behaves like by-value deduction, while `auto&`, `const auto&`, and `auto&&` follow corresponding reference deduction rules. `decltype(auto)` is different because it uses `decltype` rules.
