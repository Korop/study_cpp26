# C++26 Reflection — Interview / Study Notes

## 1. What is C++26 static reflection?

C++26 reflection lets a program inspect C++ entities at compile time.

```cpp
#include <meta>

constexpr std::meta::info r = ^^int;
```

`^^` is the **reflection operator**. Its result is a `std::meta::info`.

The important rule is:

> `^^X` means **reflect `X` itself**, not “get the type of `X`”.

Examples:

```cpp
int value = 42;

constexpr auto a = ^^int;    // reflection of a type
constexpr auto b = ^^value;  // reflection of a variable
```

`a` and `b` are both `std::meta::info`, but they represent different kinds of C++ entities.

---

## 2. What does `^^variable` represent?

Given:

```cpp
BoxD boxIvanString{"Ivan"};
```

this:

```cpp
constexpr auto info = ^^boxIvanString;
```

represents the **variable entity** `boxIvanString`.

It does **not** directly represent:

- its runtime value;
- its address;
- its type.

Mental model:

```text
boxIvanString
    ↓
C++ variable/object

^^boxIvanString
    ↓
std::meta::info representing that variable
```

To obtain the reflected type of the variable:

```cpp
std::meta::type_of(^^boxIvanString)
```

---

## 3. How do I reflect a type?

Reflect the type directly:

```cpp
constexpr auto intInfo = ^^int;
constexpr auto playerInfo = ^^Player;
constexpr auto boxInfo = ^^BoxD<std::string>;
```

All expressions produce `std::meta::info`, but each represents a type.

---

## 4. How do `^^variable` and `^^Type` differ?

```cpp
int value = 42;

constexpr auto variableInfo = ^^value;
constexpr auto typeInfo = ^^int;
```

Conceptually:

```text
^^value  → reflection of variable `value`
^^int    → reflection of type `int`
```

To compare the **type of the variable** with `int`:

```cpp
static_assert(std::meta::is_same_type(std::meta::type_of(^^value), ^^int));
```

This is the reflection equivalent of asking:

```cpp
static_assert(std::same_as<decltype(value), int>);
```

---

## 5. What does `std::meta::type_of()` do?

`type_of(info)` returns a reflection representing the type of the reflected entity.

```cpp
int value = 42;

constexpr auto variableInfo = ^^value;
constexpr auto valueTypeInfo = std::meta::type_of(variableInfo);

static_assert(std::meta::is_same_type(valueTypeInfo, ^^int));
```

Typical flow:

```text
^^variable
    ↓
reflection of variable
    ↓ type_of(...)
reflection of its type
```

It also works for data members:

```cpp
static_assert(std::meta::is_same_type(
    std::meta::type_of(^^BoxD<std::string>::Value),
    ^^std::string));
```

---

## 6. How should reflected types be compared?

Prefer:

```cpp
std::meta::is_same_type(a, b)
```

when the intent is **type equality**.

```cpp
using Int = int;

static_assert(std::meta::is_same_type(^^Int, ^^int));
```

Reflection identity and type equality are not exactly the same concept. A reflection can preserve that something was named through a type alias, while `is_same_type()` answers the semantic question: *do these reflections represent the same type?*

---

## 7. How do I print a reflected entity?

Use:

```cpp
std::meta::display_string_of(info)
```

Example:

```cpp
constexpr auto info = ^^Player;
std::println("{}", std::meta::display_string_of(info));
```

The returned display string is implementation-defined, so a library type may be printed using an internal implementation name.

---

## 8. How does reflection interact with CTAD and deduction guides?

Consider:

```cpp
template<typename T>
class BoxD
{
public:
    T Value{};

    BoxD(T&& value)
        : Value{std::move(value)}
    {
    }
};

BoxD(const char*) -> BoxD<std::string>;
```

Then:

```cpp
BoxD boxIvanString{"Ivan"};
```

uses the deduction guide:

```cpp
const char* → BoxD<std::string>
```

So:

```cpp
static_assert(std::meta::is_same_type(
    std::meta::type_of(^^boxIvanString),
    ^^BoxD<std::string>));
```

checks that the deduced object type really is `BoxD<std::string>`.

Reflection does not perform CTAD itself. CTAD happens first; reflection can then inspect the result.

---

## 9. How do I inspect the type of a reflected data member?

Given:

```cpp
template<typename T>
class BoxD
{
public:
    T Value{};
};
```

reflect the member:

```cpp
^^BoxD<std::string>::Value
```

That is reflection of the **data member**, not of `std::string`.

Use `type_of()` to obtain its type:

```cpp
static_assert(std::meta::is_same_type(
    std::meta::type_of(^^BoxD<std::string>::Value),
    ^^std::string));
```

Similarly:

```cpp
static_assert(std::meta::is_same_type(
    std::meta::type_of(^^BoxD<char*>::Value),
    ^^char*));
```

---

## 10. `decltype` vs reflection

`decltype` gives a **C++ type at compile time**:

```cpp
using T = decltype(boxIvanString);
```

Reflection gives a **compile-time value describing a C++ entity**:

```cpp
constexpr std::meta::info r = ^^boxIvanString;
```

Then reflection queries can inspect that entity:

```cpp
constexpr auto type = std::meta::type_of(r);
```

Mental distinction:

```text
decltype(expr) → a type

^^entity       → std::meta::info describing the entity
```

---

# Complete study example

```cpp
#include <meta>
#include <print>
#include <string>
#include <utility>

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

    BoxD(T&& value)
        : Value{std::move(value)}
    {
    }
};

BoxD(const char*) -> BoxD<std::string>;

int main()
{
    constexpr auto playerInfo = ^^Player;
    std::println("Player reflection: {}", std::meta::display_string_of(playerInfo));

    int value = 42;
    static_assert(std::meta::is_same_type(std::meta::type_of(^^value), ^^int));

    BoxD boxIvanString{"Ivan"};

    // ^^boxIvanString reflects the variable itself.
    constexpr auto variableInfo = ^^boxIvanString;

    // type_of(...) converts variable reflection → type reflection.
    constexpr auto variableType = std::meta::type_of(variableInfo);

    static_assert(std::meta::is_same_type(variableType, ^^BoxD<std::string>));
    static_assert(std::meta::is_same_type(std::meta::type_of(^^BoxD<std::string>::Value), ^^std::string));
    static_assert(std::meta::is_same_type(std::meta::type_of(^^BoxD<char*>::Value), ^^char*));

    std::println("Variable: {}", std::meta::display_string_of(variableInfo));
    std::println("Type: {}", std::meta::display_string_of(variableType));
    std::println("Value member type: {}",
                 std::meta::display_string_of(std::meta::type_of(^^BoxD<std::string>::Value)));
}
```

---

# Compiler / CMake note

For the macOS setup used in this study project, GCC 16 supports the C++26 reflection implementation with:

```bash
g++-16 -std=c++26 -freflection ...
```

Keep the experimental flag target-specific:

```cmake
if(
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
    AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 16
)
    add_executable(reflection_gcc_task reflection_gcc_task.cpp)
    target_compile_options(reflection_gcc_task PRIVATE -freflection)
endif()
```

Do **not** manually add Apple's libc++ directory to a GCC target:

```text
.../MacOSX.sdk/usr/include/c++/v1
```

GCC uses libstdc++; mixing libc++ and libstdc++ headers causes hard-to-explain standard-library errors.

For a C++26 study project, prefer:

```cmake
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

---

## `std::println` note

Normal formatted output is available through `<print>`:

```cpp
#include <print>

std::println("value = {}", 42);
std::println("");
```

C++26 also standardizes the zero-argument newline form:

```cpp
std::println();
```

Compiler and standard-library feature support can lag behind the language mode, so `-std=c++26` does not guarantee that every C++26 library addition is already implemented.

---

# Interview answer in ~30 seconds

C++26 reflection uses the unary `^^` operator and `std::meta::info`.

`^^X` reflects **the entity `X` itself**. Therefore `^^int` represents the type `int`, while `^^value` represents the variable `value`, not its type.

To get the type of a reflected variable or member, use:

```cpp
std::meta::type_of(^^value)
```

To compare reflected types semantically, use:

```cpp
std::meta::is_same_type(...)
```

For readable diagnostics, use:

```cpp
std::meta::display_string_of(...)
```

---

# Compact mental map

```text
^^Type
    → reflection of TYPE

^^variable
    → reflection of VARIABLE
    → type_of(...)
    → reflection of its TYPE

^^Class::member
    → reflection of MEMBER
    → type_of(...)
    → reflection of member TYPE

is_same_type(typeInfo1, typeInfo2)
    → semantic type comparison

display_string_of(info)
    → implementation-defined readable name
```

---

# Core points to remember

- `^^X` means **reflect X**, not “type of X”.
- The result of reflection is `std::meta::info`.
- `^^variable` and `^^Type` represent different kinds of entities.
- Use `std::meta::type_of()` to move from variable/member reflection to type reflection.
- Use `std::meta::is_same_type()` when the question is type equality.
- `^^Class::member` reflects the member itself; `type_of()` gives its type.
- CTAD/deduction guides determine the object type before reflection inspects it.
- `display_string_of()` is useful for diagnostics, but its exact spelling is implementation-defined.
- GCC 16 reflection requires `-std=c++26 -freflection`.
- Do not mix GCC's libstdc++ headers with Apple's libc++ headers.
