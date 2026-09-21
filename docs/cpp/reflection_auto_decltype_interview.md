# C++26 Reflection, `auto`, and `decltype(auto)` — Interview / Study Notes

## 1. What is C++26 reflection?

C++26 introduces **static reflection**: compile-time inspection of C++ entities such as types, variables, members, functions, parameters, enums, templates, and namespaces.

Core header:

```cpp
#include <meta>
```

Core mental model:

```text
C++ entity
   │
   │ ^^
   ▼
std::meta::info
   │
   │ std::meta::...
   ▼
metadata
   │
   │ [: ... :]
   ▼
C++ entity / type / member again
```

Reflection is mainly a **compile-time metaprogramming** feature, not a C#-style runtime reflection system.

---

## 2. Reflection operator `^^`

`^^` creates a reflection value of type `std::meta::info`.

```cpp
struct Player
{
    int score;
};

constexpr std::meta::info typeInfo = ^^Player;
constexpr std::meta::info intInfo = ^^int;
constexpr std::meta::info memberInfo = ^^Player::score;

static_assert(std::meta::is_type(typeInfo));
static_assert(std::meta::is_type(intInfo));
static_assert(std::meta::is_nonstatic_data_member(memberInfo));
```

Important:

```cpp
^^int      // reflection of type int
^^int&     // reflection of type int&
^^Player   // reflection of type Player
^^x        // reflection of variable x
```

`std::meta::info` is one common reflection type for many different C++ constructs.

---

## 3. Reflection queries

`std::meta` provides compile-time query functions.

Typical examples:

```cpp
std::meta::is_type(r);
std::meta::is_function(r);
std::meta::is_nonstatic_data_member(r);

std::meta::identifier_of(r);
std::meta::type_of(r);
std::meta::parent_of(r);

std::meta::members_of(r, ctx);
std::meta::nonstatic_data_members_of(r, ctx);
std::meta::parameters_of(r);
std::meta::enumerators_of(r);
```

Mental model:

```text
^^x                  -> reflection of variable x
type_of(^^x)         -> reflection of x's declared type

^^Player::score      -> reflection of member score
type_of(^^Player::score) -> reflection of member type
```

---

## 4. `type_of()` and `is_same_type()`

`std::meta::type_of()` expects reflection of a **typed entity** and returns reflection of its type.

```cpp
int x = 10;
int& z = x;

static_assert(std::meta::is_same_type(std::meta::type_of(^^z), ^^int&));
```

### Common mistake

Wrong:

```cpp
// std::meta::is_same_type<std::meta::type_of(^^int&), std::meta::type_of(^^z)>
```

Why?

1. `is_same_type` is a function, not a template.
2. `^^int&` already represents the type `int&`.
3. `type_of(^^z)` is useful because `^^z` represents the variable `z`.

Correct:

```cpp
static_assert(std::meta::is_same_type(^^int&, std::meta::type_of(^^z)));
```

Another clean form:

```cpp
static_assert(std::meta::is_same_type(^^decltype(z), ^^int&));
```

Traditional type-trait equivalent:

```cpp
static_assert(std::is_same_v<decltype(z), int&>);
```

---

## 5. `decltype(x)` vs `decltype((x))`

This rule is older than reflection but becomes useful when checking reflected types.

```cpp
int x = 10;

decltype(x) y = 20;   // int
decltype((x)) z = x;  // int&
```

Why?

For an **unparenthesized id-expression**:

```cpp
decltype(x)
```

`decltype` returns the declared type of `x`.

For a general expression:

```cpp
decltype((x))
```

the result depends on value category:

| Expression category | `decltype(expr)` |
|---|---|
| lvalue | `T&` |
| xvalue | `T&&` |
| prvalue | `T` |

Since `(x)` is an lvalue:

```cpp
decltype((x)) // int&
```

Example:

```cpp
int x = 10;
decltype((x)) z = x;

z += 1;
std::println("x={}, z={}", x, z); // x=11, z=11

static_assert(std::is_same_v<decltype(x), int>);
static_assert(std::is_same_v<decltype((x)), int&>);
```

---

## 6. Splicing `[: ... :]`

Reflection converts a C++ construct into `std::meta::info`.

A **splice** converts reflection back into something usable by C++ syntax:

```cpp
[: reflection :]
```

Conceptually:

```text
member
  │
  │ ^^
  ▼
std::meta::info
  │
  │ [: ... :]
  ▼
member
```

Example idea:

```cpp
Player player{"Ivan", 100};
constexpr auto member = ^^Player::score;

std::println("{}", player.[:member:]);
```

Conceptually this becomes:

```cpp
std::println("{}", player.score);
```

---

## 7. Reflection + `template for`

`template for` is a C++26 **expansion statement**. It expands code at compile time and works naturally with reflection.

```cpp
struct Player
{
    std::string name;
    int score;
};

void Print(const Player& player)
{
    constexpr auto ctx = std::meta::access_context::unprivileged();

    template for (constexpr auto member :
        std::meta::nonstatic_data_members_of(^^Player, ctx))
    {
        std::println("{} = {}",
            std::meta::identifier_of(member),
            player.[:member:]);
    }
}
```

Conceptually generated code:

```cpp
std::println("name = {}", player.name);
std::println("score = {}", player.score);
```

This is useful for:

- serialization;
- logging;
- enum conversion;
- RPC/API bindings;
- ORM mapping;
- generic testing/debug tooling.

---

## 8. Reflection vs RTTI vs type traits

| Feature | Main time | Structural inspection | Typical use |
|---|---|---:|---|
| RTTI: `typeid`, `dynamic_cast` | runtime | No | runtime polymorphism |
| Type traits | compile time | Limited/predefined | answer type questions |
| C++26 reflection | compile time | Yes | inspect structure and generate code |

Mental distinction:

```text
RTTI:
"What is the dynamic type of this object?"

Type traits:
"Does T satisfy a predefined type property?"

Reflection:
"What does this program entity contain, and can I generate code from it?"
```

---

# `auto` and `decltype(auto)`

## 9. Why does `auto` remove a returned reference?

```cpp
int value = 10;

int& Get()
{
    return value;
}

auto x = Get(); // int, not int&
```

`Get()` returns `int&`, but plain `auto` uses rules based on **template argument deduction**.

Think approximately:

```cpp
template<typename T>
void F(T value);

F(Get());
```

For a by-value `T`, the deduced type is:

```cpp
T == int
```

So:

```cpp
auto x = Get();
```

creates a new independent `int` object.

```cpp
x = 20;
std::println("value={}, x={}", value, x); // value=10, x=20
```

### Rule

```cpp
auto x = expression;
```

normally removes:

- references;
- top-level `const` / `volatile`.

---

## 10. Why does `decltype(auto)` preserve `int&`?

```cpp
decltype(auto) x = Get();
```

`decltype(auto)` does **not** use normal `auto` deduction.

It applies:

```cpp
decltype(Get())
```

`Get()` is an lvalue expression because its return type is `int&`.

Therefore:

```cpp
decltype(Get()) // int&
```

and:

```cpp
decltype(auto) x = Get(); // int&
```

Now `x` aliases `value`:

```cpp
x = 20;
std::println("value={}", value); // 20
```

### Important wording

Do not memorize:

> `decltype(auto)` always preserves the exact type.

Better:

> `decltype(auto)` deduces the type using **`decltype(initializer)` rules**.

Those rules depend on the exact expression and its value category.

---

## 11. `auto`, `auto&`, `auto&&`, `decltype(auto)`

For:

```cpp
int& Get();
```

the declarations behave as follows:

```cpp
auto a = Get();           // int
auto& b = Get();          // int&
auto&& c = Get();         // int&
decltype(auto) d = Get(); // int&
```

| Declaration | Type | Reason |
|---|---|---|
| `auto a = Get()` | `int` | value-style deduction |
| `auto& b = Get()` | `int&` | explicit lvalue reference |
| `auto&& c = Get()` | `int&` | forwarding-reference deduction + collapsing |
| `decltype(auto) d = Get()` | `int&` | `decltype(Get())` |

For `auto&& c = Get()`:

```text
initializer is lvalue
auto deduces as int&
int& && -> int&
```

Reference collapsing:

```text
T&  &  -> T&
T&  && -> T&
T&& &  -> T&
T&& && -> T&&
```

---

## 12. Is `decltype(auto&)` or `decltype(auto&&)` valid?

No.

These are invalid:

```cpp
// decltype(auto&) x = Get();   // error
// decltype(auto&&) x = Get();  // error
// decltype(auto)& x = Get();   // error
// decltype(auto)&& x = Get();  // error
```

`decltype(auto)` is a special placeholder and must be used as the **sole constituent of the declared type**.

Use these instead:

```cpp
auto& x = Get();
auto&& y = Get();
decltype(auto) z = Get();
```

You can apply references to an ordinary `decltype(expr)` result:

```cpp
decltype(Get())& a = Get();   // int& &  -> int&
decltype(Get())&& b = Get();  // int& && -> int&
```

Usually this is unnecessary, but it demonstrates reference collapsing.

---

# 13. Compact study task

```cpp
#include <meta>
#include <print>
#include <type_traits>

int value = 10;

int& Get()
{
    return value;
}

int main()
{
    int x = 10;

    decltype(x) y = 20;       // int
    decltype((x)) z = x;      // int&
    z += 1;

    auto a = Get();           // int
    auto& b = Get();          // int&
    auto&& c = Get();         // int&
    decltype(auto) d = Get(); // int&

    a = 100; // copy
    b = 20;  // changes value
    c = 30;  // changes value
    d = 40;  // changes value

    static_assert(std::is_same_v<decltype(x), int>);
    static_assert(std::is_same_v<decltype((x)), int&>);
    static_assert(std::is_same_v<decltype(a), int>);
    static_assert(std::is_same_v<decltype(b), int&>);
    static_assert(std::is_same_v<decltype(c), int&>);
    static_assert(std::is_same_v<decltype(d), int&>);
    static_assert(std::meta::is_same_type(^^decltype(z), ^^int&));
    static_assert(std::meta::is_same_type(std::meta::type_of(^^z), ^^int&));

    std::println("x={}, y={}, z={}", x, y, z);
    std::println("a(copy)={}, value={}", a, value);
}
```

Expected important result:

```text
a(copy)=100, value=40
```

`a` is independent; `b`, `c`, and `d` refer to `value`.

---

# 14. The compact mental map

```text
^^T
    -> reflection of type T

^^x
    -> reflection of variable x

type_of(^^x)
    -> reflection of x's declared type

[:r:]
    -> splice reflected construct back into C++


auto x = expr
    -> template-like value deduction

auto& x = expr
    -> lvalue reference deduction

auto&& x = expr
    -> forwarding-reference rules + reference collapsing

decltype(auto) x = expr
    -> exactly decltype(expr) rules


decltype(x)
    -> declared type of unparenthesized name x

decltype((x))
    -> expression rule; x is lvalue -> T&
```

---

# 15. Core points to remember

1. `^^entity` produces `std::meta::info`.
2. `^^int&` already represents the type `int&`.
3. `std::meta::type_of(^^x)` gets the type of reflected variable/entity `x`.
4. `std::meta::is_same_type(a, b)` is a **function**, not a template.
5. `[:r:]` splices a reflection back into C++ syntax.
6. `template for` performs compile-time expansion and pairs naturally with reflection.
7. `auto x = expr` uses template-like deduction and normally strips references/top-level cv.
8. `decltype(auto)` uses `decltype(expr)` rules.
9. `decltype(x)` and `decltype((x))` can differ.
10. `decltype(auto)&`, `decltype(auto)&&`, `decltype(auto&)`, and `decltype(auto&&)` are invalid.

---

# 16. Cheat Sheet

```cpp
// Reflection
constexpr auto r1 = ^^int;                  // reflection of int
constexpr auto r2 = ^^x;                    // reflection of variable x
constexpr auto r3 = std::meta::type_of(r2); // reflection of x's type

static_assert(std::meta::is_type(r1));
static_assert(std::meta::is_same_type(r3, ^^int));

// decltype
decltype(x) a = 1;    // declared type of x
decltype((x)) b = x;  // T& when x is lvalue

// auto family
auto a1 = Get();           // value
auto& a2 = Get();          // lvalue reference
auto&& a3 = Get();         // forwarding-reference deduction
decltype(auto) a4 = Get(); // decltype(Get())
```

---

# 17. Interview questions

### Q1. What does `^^T` return?

A value of type `std::meta::info` representing the reflected C++ construct.

### Q2. What is the difference between `^^x` and `std::meta::type_of(^^x)`?

`^^x` represents the variable `x`; `type_of(^^x)` represents its type.

### Q3. Why is this wrong?

```cpp
std::meta::is_same_type<^^int, ^^int>
```

Because `is_same_type` is a `consteval` function, not a function template.

Correct:

```cpp
static_assert(std::meta::is_same_type(^^int, ^^int));
```

### Q4. Why should I not write `type_of(^^int&)`?

`^^int&` already represents a type. `type_of()` is for a reflected typed entity such as a variable, member, or function.

### Q5. Why is `decltype(x)` `int`, but `decltype((x))` `int&`?

An unparenthesized id-expression uses the entity's declared type. `(x)` is a general lvalue expression, so the expression form of `decltype` returns `T&`.

### Q6. Why does `auto x = Get()` copy when `Get()` returns `int&`?

Plain `auto` uses template-like deduction for a value declaration, which removes the reference.

### Q7. How do I preserve the returned reference?

```cpp
auto& x = Get();
```

or:

```cpp
decltype(auto) x = Get();
```

when `decltype(Get())` is a reference type.

### Q8. Why is `auto&& x = Get()` an `int&`?

`auto&&` is a forwarding reference here. An lvalue initializer makes `auto` deduce as `int&`, then:

```text
int& && -> int&
```

### Q9. Can I write `decltype(auto)&`?

No. `decltype(auto)` must be used alone as the placeholder type.

### Q10. Reflection vs RTTI?

Reflection is primarily compile-time structural introspection/code generation. RTTI (`typeid`, `dynamic_cast`) is runtime type information for polymorphic/runtime behavior.

---

# 18. Interview answer in ~30 seconds

> C++26 reflection uses the `^^` operator to produce a `std::meta::info` representing a type, variable, member, function, enum, or other supported C++ entity. `std::meta` query functions inspect that metadata at compile time, while splice syntax `[:r:]` converts a reflection back into a C++ construct. `template for` can expand operations over reflected members, which makes reflection useful for serialization, logging, RPC, and similar generated code.
>
> For deduction, plain `auto` follows template-like deduction, so `auto x = Get()` drops a returned `int&` and creates an `int`. `decltype(auto)` instead applies `decltype(initializer)` rules, so if `Get()` returns `int&`, `decltype(auto) x = Get()` is `int&`. `decltype(auto)` must appear alone; `decltype(auto)&` and `decltype(auto&&)` are invalid.

---

## References

- cppreference: Reflection operator — <https://en.cppreference.com/cpp/language/operator_reflection>
- cppreference: Reflection library — <https://en.cppreference.com/cpp/meta/reflection>
- cppreference: `std::meta::type_of` — <https://en.cppreference.com/cpp/meta/type_of>
- cppreference: `std::meta::is_same_type` — <https://en.cppreference.com/cpp/meta/is_same_type>
- cppreference: Splice specifiers — <https://en.cppreference.com/cpp/language/splice_specifiers>
- cppreference: `template for` expansion — <https://en.cppreference.com/cpp/language/template_for>
- cppreference: `auto` / `decltype(auto)` — <https://en.cppreference.com/cpp/language/auto>
- cppreference: `decltype` — <https://en.cppreference.com/cpp/language/decltype>
- GCC 16 reflection: compile with `-std=c++26 -freflection`
