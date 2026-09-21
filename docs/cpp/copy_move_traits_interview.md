# C++26 Copy/Move Traits, `= default`, References, `add_const_t`, and Type Deduction

## 1. Why is `std::is_copy_constructible_v<Connection&>` `true`?

```cpp
class Connection
{
public:
    Connection() = default;
    Connection(Connection&) = delete;
    Connection(const Connection&) = delete;
    Connection(Connection&&) = default;
};

static_assert(!std::is_copy_constructible_v<Connection>);
static_assert( std::is_copy_constructible_v<Connection&>);
static_assert( std::is_move_constructible_v<Connection>);
```

### Question

Why does this compile?

```cpp
static_assert(std::is_copy_constructible_v<Connection&>);
```

even though:

```cpp
Connection(Connection&) = delete;
Connection(const Connection&) = delete;
```

### Explanation

`std::is_copy_constructible_v<T>` is approximately based on:

```cpp
std::is_constructible_v<T, std::add_lvalue_reference_t<std::add_const_t<T>>>
```

For:

```cpp
T = Connection
```

the source type becomes:

```cpp
const Connection&
```

so the trait tests whether this is valid:

```cpp
Connection copy{source};
```

It is not, because the copy constructor is deleted.

But for:

```cpp
T = Connection&
```

`std::add_const_t<Connection&>` is still:

```cpp
Connection&
```

Therefore the test is effectively:

```cpp
std::is_constructible_v<Connection&, Connection&>
```

which is valid:

```cpp
Connection c;
Connection& r1 = c;
Connection& r2 = r1; // no Connection object is copied
```

`r1` and `r2` simply refer to the same object.

### Core point

```cpp
Connection c1;
Connection c2{c1};   // ERROR: constructs Connection -> copy ctor needed

Connection& r1 = c1;
Connection& r2 = r1; // OK: binds another reference
```

`std::is_copy_constructible_v<Connection&>` tests reference binding, not whether `Connection` itself is copyable.

---

## 2. What does `= default` mean?

```cpp
class Astral
{
public:
    Astral(const Astral&) = default;
};
```

`= default` means:

> Ask the compiler to generate the normal implementation of this specific special member function.

Here it requests the normal copy constructor:

```cpp
Astral(const Astral&);
```

It does **not** request all other constructors.

### Important consequence

Declaring a constructor prevents generation of the implicit default constructor.

Therefore:

```cpp
class Astral
{
public:
    Astral(const Astral&) = default;
};

Astral a;   // ERROR: no Astral()
Astral a{}; // ERROR: no Astral()
```

To allow default construction:

```cpp
class Astral
{
public:
    Astral() = default;
    Astral(const Astral&) = default;
};
```

Now:

```cpp
Astral a{};
Astral b{a};
```

are both valid.

---

## 3. Why does `Astral aOfA();` compile without `Astral()`?

```cpp
Astral aOfA();
```

This does **not** create an object.

It declares a function:

```cpp
// function name: aOfA
// parameters: none
// return type: Astral
Astral aOfA();
```

Equivalent:

```cpp
Astral aOfA(void);
```

So the default constructor is not needed.

This is the classic **most vexing parse** family of C++ syntax problems.

Compare:

```cpp
Astral aOfA(); // function declaration
Astral a{};    // object construction
```

---

## 4. `std::add_const_t<T>` rules

`std::add_const_t<T>` adds **top-level `const`** when the type can be top-level cv-qualified.

```cpp
static_assert(std::is_same_v<std::add_const_t<int>, const int>);
static_assert(std::is_same_v<std::add_const_t<int*>, int* const>);
```

For a pointer:

```cpp
T = int*
```

adding top-level `const` gives:

```cpp
int* const
```

not:

```cpp
const int*
```

because the pointer itself becomes const.

### References are different

```cpp
static_assert(std::is_same_v<std::add_const_t<int&>, int&>);
static_assert(std::is_same_v<std::add_const_t<int&&>, int&&>);
```

A reference itself cannot be cv-qualified.

This hypothetical type is invalid:

```cpp
int& const // invalid
```

But this is valid:

```cpp
const int&
```

because it means:

> reference to const `int`

The `const` applies to `int`, not to the reference.

### Why references cannot be `const`

A reference cannot be reseated after initialization anyway:

```cpp
int a = 10;
int b = 20;

int& ref = a;
ref = b; // assigns b's value to a; ref still refers to a
```

A separate `const` qualification on the reference itself would add nothing.

### Mental model

```text
int
    -> add_const -> const int

int*
    -> add_const -> int* const

int&
    -> cannot const-qualify the reference layer
    -> remains int&
```

---

## 5. How to create `const int&` with type traits

If the original type is:

```cpp
using T = int;
```

then:

```cpp
using R = std::add_lvalue_reference_t<std::add_const_t<T>>;
```

produces:

```cpp
const int&
```

because the transformations are:

```text
int
-> const int
-> const int&
```

But if:

```cpp
using T = int&;
```

then:

```cpp
std::add_const_t<T>
```

cannot "reach through" the reference to make the referred object const.

---

## 6. `std::is_move_constructible_v<T>` does not guarantee a move constructor exists

Consider:

```cpp
class Astral
{
public:
    Astral() = default;
    Astral(const Astral&) = default;
    // no move constructor
};
```

This can still be true:

```cpp
static_assert(std::is_move_constructible_v<Astral>);
```

Why?

The trait effectively asks whether an `Astral` can be constructed from an rvalue:

```cpp
Astral{std::declval<Astral&&>()}
```

A `const Astral&` can bind to an rvalue, so the copy constructor can satisfy that expression:

```cpp
Astral(const Astral&);
```

Therefore:

> `is_move_constructible_v<T> == true` means construction from `T&&` is valid, not necessarily that a real move constructor is selected.

---

# Type Deduction Example

## 7. Why does `Describe("Jora")` print `Describe other type`?

```cpp
template<typename T>
void Describe(T&& value)
{
    using RawT = std::decay_t<T>;

    if constexpr (std::is_integral_v<RawT>)
    {
        std::println("Describe integral: {}", value);
    }
    else if constexpr (std::is_floating_point_v<RawT>)
    {
        std::println("Describe floating: {}", value);
    }
    else if constexpr (std::is_same_v<RawT, std::string>)
    {
        std::println("Describe string: {}", value);
    }
    else
    {
        std::println("Describe other type");
    }
}
```

Calls:

```cpp
int x{10};
const double d{3.14};
std::string name{"Ivan"};

Describe(x);
Describe(d);
Describe(name);
Describe(std::string{"Anna"});
Describe("Jora");
```

A string literal is not a `std::string`.

```cpp
"Jora"
```

has array type:

```cpp
const char[5]
```

The five elements are:

```text
'J' 'o' 'r' 'a' '\0'
```

Because the parameter is a forwarding reference:

```cpp
T&&
```

and the argument is an lvalue array, template deduction gives:

```cpp
T = const char(&)[5]
```

Then:

```cpp
using RawT = std::decay_t<T>;
```

performs array-to-pointer decay:

```cpp
const char(&)[5] -> const char*
```

So:

```cpp
static_assert(std::is_same_v<std::decay_t<decltype("Jora")>, const char*>);
```

and the test:

```cpp
std::is_same_v<RawT, std::string>
```

is false.

---

## 8. `std::decay_t` vs `std::remove_cvref_t`

For most scalar types they often produce the same result:

```cpp
const int& -> int
int&&      -> int
```

But arrays and functions reveal the important difference.

For a string literal:

```cpp
static_assert(std::is_same_v<decltype("Jora"), const char(&)[5]>);
static_assert(std::is_same_v<std::decay_t<decltype("Jora")>, const char*>);
static_assert(std::is_same_v<std::remove_cvref_t<decltype("Jora")>, const char[5]>);
```

### Compact comparison

| Original type | `std::decay_t<T>` | `std::remove_cvref_t<T>` |
|---|---|---|
| `const int&` | `int` | `int` |
| `int&&` | `int` | `int` |
| `const char(&)[5]` | `const char*` | `const char[5]` |
| function reference | function pointer | function type |

`std::decay_t` additionally performs:

```text
array    -> pointer
function -> function pointer
```

`std::remove_cvref_t` only removes top-level cv/ref qualifiers.

---

## 9. Can `"Jora"` implicitly convert to `std::string` during template deduction?

A conversion exists:

```cpp
std::string s = "Jora";
```

But function template argument deduction does not first convert the argument to `std::string`.

This:

```cpp
Describe("Jora");
```

deduces `T` from the actual argument type:

```cpp
const char(&)[5]
```

The conversion to `std::string` is not used to choose `T`.

To force a `std::string`:

```cpp
Describe(std::string{"Jora"});
```

or:

```cpp
using namespace std::string_literals;
Describe("Jora"s);
```

---

## 10. Can a deduction guide fix `Describe("Jora")`?

For a **class template**, yes:

```cpp
template<typename T>
class BoxD;

BoxD(const char*) -> BoxD<std::string>;
```

This is a CTAD deduction guide.

It tells the compiler:

```cpp
BoxD box{"Jora"};
```

to deduce:

```cpp
BoxD<std::string>
```

But function templates do not support deduction guides.

This does not exist:

```cpp
Describe(const char*) -> Describe<std::string>; // invalid syntax
```

For function templates, use:

- overloads,
- constraints,
- explicit conversion,
- or explicit template arguments.

Example overload:

```cpp
void Describe(const char* value)
{
    Describe(std::string{value});
}
```

Then:

```cpp
Describe("Jora");
```

can be redirected into the existing `std::string` branch.

---

# Complete Study Snippet

```cpp
#include <print>
#include <string>
#include <type_traits>
#include <utility>

class Connection
{
public:
    Connection() = default;
    Connection(Connection&) = delete;
    Connection(const Connection&) = delete;
    Connection(Connection&&) = default;
};

class Astral
{
public:
    Astral() = default;
    Astral(const Astral&) = default;
    // no move constructor
};

template<typename T>
void Describe(T&& value)
{
    using RawT = std::decay_t<T>;

    if constexpr (std::is_integral_v<RawT>)
    {
        std::println("Describe integral: {}", value);
    }
    else if constexpr (std::is_floating_point_v<RawT>)
    {
        std::println("Describe floating: {}", value);
    }
    else if constexpr (std::is_same_v<RawT, std::string>)
    {
        std::println("Describe string: {}", value);
    }
    else if constexpr (std::is_same_v<RawT, const char*>)
    {
        std::println("Describe C string: {}", value);
    }
    else
    {
        std::println("Describe other type");
    }
}

static_assert(!std::is_copy_constructible_v<Connection>);
static_assert( std::is_copy_constructible_v<Connection&>);
static_assert( std::is_move_constructible_v<Connection>);
static_assert( std::is_move_constructible_v<Astral>);

static_assert(std::is_same_v<std::add_const_t<int>, const int>);
static_assert(std::is_same_v<std::add_const_t<int*>, int* const>);
static_assert(std::is_same_v<std::add_const_t<int&>, int&>);
static_assert(std::is_same_v<std::add_const_t<int&&>, int&&>);

static_assert(std::is_same_v<decltype("Jora"), const char(&)[5]>);
static_assert(std::is_same_v<std::decay_t<decltype("Jora")>, const char*>);
static_assert(std::is_same_v<std::remove_cvref_t<decltype("Jora")>, const char[5]>);

int main()
{
    Connection connection;
    Connection& r1 = connection;
    Connection& r2 = r1;

    std::println("Same object: {}", &r1 == &r2);

    Astral a{};
    Astral b{a};

    int x{10};
    const double d{3.14};
    std::string name{"Ivan"};

    Describe(x);
    Describe(d);
    Describe(name);
    Describe(std::string{"Anna"});
    Describe("Jora");
}
```

---

# Interview Answer in ~30 Seconds

`std::is_copy_constructible_v<T>` tests whether `T` can be constructed from its const-lvalue-reference form. For `T = Connection&`, adding `const` does not produce `const Connection&`, because references themselves cannot be cv-qualified. The type stays `Connection&`, so the trait effectively tests whether a `Connection&` can bind to another `Connection&`, which is valid and does not invoke a `Connection` copy constructor.

`= default` generates only the specific special member function that is declared. Declaring a copy constructor does not also generate a default constructor.

For type normalization, `remove_cvref_t` removes cv/ref qualifiers but preserves arrays and functions, while `decay_t` additionally converts arrays to pointers and functions to function pointers.

---

# Compact Mental Map

```text
is_copy_constructible<Connection>
    -> needs Connection(const Connection&)
    -> deleted
    -> false

is_copy_constructible<Connection&>
    -> effectively Connection& from Connection&
    -> reference binding
    -> true

add_const<int>       -> const int
add_const<int*>      -> int* const
add_const<int&>      -> int&
add_const<int&&>     -> int&&

"Jora"
    -> const char[5]
    -> forwarding reference deduction: const char(&)[5]

decay_t
    -> remove cv/ref
    -> arrays -> pointers
    -> functions -> function pointers

remove_cvref_t
    -> remove cv/ref only
    -> preserves arrays/functions

CTAD deduction guides
    -> class templates only
    -> no equivalent deduction-guide syntax for function templates
```

---

# Core Points to Remember

1. `std::is_copy_constructible_v<T&>` is about reference binding, not copying a `T` object.
2. A reference itself cannot be `const`; `const T&` means reference to const `T`.
3. `std::add_const_t<T&>` remains `T&`.
4. `= default` applies only to the special member function where it appears.
5. Declaring a constructor suppresses the implicit default constructor.
6. `Type object();` is parsed as a function declaration, not object construction.
7. `is_move_constructible_v<T>` can be true even without a real move constructor.
8. String literals are arrays: `"Jora"` is `const char[5]`.
9. `decay_t` changes arrays/functions; `remove_cvref_t` preserves them.
10. Function template deduction does not use user-defined conversions to decide `T`.
11. Deduction guides customize CTAD for class templates, not function templates.
