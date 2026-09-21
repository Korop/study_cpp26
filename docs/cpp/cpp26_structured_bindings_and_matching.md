# C++26 Structured Bindings and Modern C++ Matching Techniques

## Question

**C++26 structured bindings and other types of matching available in modern C++, for `switch`, tuples, templates, etc.**

---

## 1. Structured bindings are not general pattern matching

Modern C++ has several mechanisms that look related to pattern matching, but they solve different problems.

> **C++26 does not have general Rust/C#-style pattern matching.**

A general `match` expression has been proposed, including matching tuples, variants, optionals, values, and other structures, but it is **not part of C++26**.

The main modern C++ toolbox is:

| Problem | Mechanism | Since |
|---|---|---:|
| Split struct into variables | Structured binding | C++17 |
| Split `pair` / `tuple` / `array` | Structured binding | C++17 |
| Split map entry | Structured binding | C++17 |
| Match `variant` by contained type | `std::visit` | C++17 |
| Check one `variant` alternative | `std::get_if`, `std::holds_alternative` | C++17 |
| Match integral / enum value | `switch` | Classic C++ |
| Initialize value inside `if` / `switch` | Init-statement | C++17 |
| Compile-time type selection | `if constexpr` | C++17 |
| Template/type constraints | Concepts / `requires` | C++20 |
| Structured binding in condition | `if (auto [..] = ...)` | C++26 |
| Structured binding packs | `[first, ...rest]` | C++26 |
| Pack indexing | `args...[I]`, `Ts...[I]` | C++26 |
| General `match` expression | Not in C++26 | Proposed |

---

# 2. Basic structured binding

```cpp
struct Point
{
    int x;
    int y;
};

Point p{10, 20};

auto [x, y] = p;
```

Now:

```cpp
static_assert(std::is_same_v<decltype(x), int>);
```

Conceptually:

```cpp
auto hidden = p;

// x refers to hidden.x
// y refers to hidden.y
```

`auto [x, y]` creates a copy of the source object.

```cpp
Point p{10, 20};

auto [x, y] = p;

x = 100;

std::println("{}", p.x); // 10
```

---

# 3. Structured binding by reference

To modify the original object:

```cpp
Point p{10, 20};

auto& [x, y] = p;

x = 100;

std::println("{}", p.x); // 100
```

Useful forms:

```cpp
auto [x, y] = p;         // copy
auto& [x, y] = p;        // reference
const auto& [x, y] = p;  // const reference
auto&& [x, y] = expr;    // forwarding-style binding
```

Mental model:

```text
auto [x, y]        -> copy object
auto& [x, y]       -> bind original object
const auto& [x, y] -> read-only binding
```

---

# 4. Arrays

Structured bindings work with C arrays.

```cpp
int values[3]{10, 20, 30};

auto [a, b, c] = values;

std::println("{} {} {}", a, b, c);
```

To modify the array:

```cpp
int values[3]{10, 20, 30};

auto& [a, b, c] = values;

b = 99;

std::println("{}", values[1]); // 99
```

Structured bindings have three main decomposition categories:

1. arrays,
2. tuple-like types,
3. class data members.

---

# 5. `std::pair`

```cpp
std::pair<int, std::string> p{
    42,
    "hello"
};

auto [number, text] = p;

std::println("{} {}", number, text);
```

Instead of:

```cpp
auto number = p.first;
auto text = p.second;
```

---

# 6. `std::tuple`

```cpp
std::tuple<int, std::string, double> data{
    42,
    "hello",
    3.14
};

auto [id, name, value] = data;

std::println("{} {} {}", id, name, value);
```

Equivalent explicit access:

```cpp
auto id = std::get<0>(data);
auto name = std::get<1>(data);
auto value = std::get<2>(data);
```

Structured bindings are usually much easier to read.

---

# 7. Returning multiple values

```cpp
std::tuple<int, int, bool> Calculate()
{
    return {10, 20, true};
}

int main()
{
    auto [x, y, success] = Calculate();

    if (success)
        std::println("{}", x + y);
}
```

When values have domain meaning, a named struct is often clearer:

```cpp
struct CalculationResult
{
    int x;
    int y;
    bool success;
};

CalculationResult Calculate()
{
    return {
        .x = 10,
        .y = 20,
        .success = true
    };
}

auto [x, y, success] = Calculate();
```

A named struct is usually preferable in public APIs because:

```cpp
result.success
```

is clearer than:

```cpp
std::get<2>(result)
```

---

# 8. `std::map`

A very common structured-binding use case:

```cpp
std::map<std::string, int> scores{
    {"Alice", 100},
    {"Bob", 80}
};

for (const auto& [name, score] : scores)
{
    std::println("{}: {}", name, score);
}
```

Each map element is approximately:

```cpp
std::pair<const Key, Value>
```

Therefore:

```cpp
for (auto& [key, value] : scores)
{
    value += 10;

    // key = "Other"; // error: key is const
}
```

---

# 9. Functions returning `std::pair`

A typical example:

```cpp
std::set<int> numbers;

auto [iterator, inserted] = numbers.insert(42);

if (inserted)
    std::println("Inserted");
else
    std::println("Already existed");
```

`std::set::insert()` returns approximately:

```cpp
std::pair<iterator, bool>
```

---

# 10. `if` + structured binding before C++26

Since C++17, structured bindings can be used in an init-statement:

```cpp
if (auto [it, inserted] = numbers.insert(42); inserted)
{
    std::println("Inserted");
}
```

Conceptually:

```cpp
auto [it, inserted] = numbers.insert(42);

if (inserted)
{
}
```

The difference is that `it` and `inserted` exist only inside the `if` / `else` scope.

The general form is:

```cpp
if (initialization; condition)
{
}
```

Example:

```cpp
if (auto ptr = FindObject(); ptr != nullptr)
{
    ptr->Run();
}
```

---

# 11. C++26: structured binding directly as a condition

C++26 allows a structured binding declaration directly in a condition when the underlying object can be converted to `bool`.

```cpp
struct Result
{
    int value;
    std::string error;

    explicit operator bool() const noexcept
    {
        return error.empty();
    }
};
```

Usage:

```cpp
if (auto [value, error] = DoWork())
{
    std::println("{}", value);
}
else
{
    std::println("{}", error);
}
```

The condition uses the underlying hidden `Result` object.

Conceptually:

```cpp
auto hidden = DoWork();

auto [value, error] = hidden;

if (static_cast<bool>(hidden))
{
}
```

---

# 12. C++26 structured-binding packs

Before C++26:

```cpp
struct Point
{
    int x;
    int y;
    int z;
};

Point p{1, 2, 3};

auto [x, y, z] = p;
```

The number of binding names had to match the number of elements.

C++26 introduces structured-binding packs:

```cpp
template<class T>
void Foo(T value)
{
    auto [first, ...rest] = value;
}
```

For:

```cpp
struct Data
{
    int a;
    int b;
    int c;
    int d;
};
```

this means approximately:

```text
first -> a

rest:
    b
    c
    d
```

The pack can also appear first:

```cpp
auto [...rest, last] = value;
```

or consume all elements:

```cpp
auto [...elements] = value;
```

---

# 13. C++26 pack indexing

C++26 also introduces pack indexing:

```cpp
args...[I]
```

Example:

```cpp
template<std::size_t I, typename... Ts>
decltype(auto) GetArgument(Ts&&... args)
{
    return args...[I];
}
```

Usage:

```cpp
auto x = GetArgument<0>(10, 20.5, "hello");
auto y = GetArgument<1>(10, 20.5, "hello");

std::println("{}", x); // 10
std::println("{}", y); // 20.5
```

Pack indexing can be combined with structured-binding packs:

```cpp
template<std::size_t I, typename T>
decltype(auto) GetElement(T&& object)
{
    auto&& [...elements] = object;

    return elements...[I];
}
```

Example:

```cpp
struct Point
{
    int x;
    int y;
    int z;
};

Point p{10, 20, 30};

std::println("{}", GetElement<0>(p)); // 10
std::println("{}", GetElement<1>(p)); // 20
std::println("{}", GetElement<2>(p)); // 30
```

---

# 14. Tuple -> function arguments with `std::apply`

Structured bindings decompose a tuple into variables.

`std::apply` decomposes a tuple into function arguments.

```cpp
void Print(int id, const std::string& name)
{
    std::println("{} {}", id, name);
}

auto data = std::tuple{
    42,
    std::string{"Alice"}
};

std::apply(Print, data);
```

Without `std::apply`:

```cpp
Print(
    std::get<0>(data),
    std::get<1>(data)
);
```

Mental model:

```text
tuple
  |
  v
elements
  |
  v
function arguments
```

---

# 15. `switch` is value matching, not pattern matching

```cpp
enum class State
{
    Idle,
    Running,
    Error
};

State state = State::Running;

switch (state)
{
    case State::Idle:
        break;

    case State::Running:
        break;

    case State::Error:
        break;
}
```

`switch` performs value-based dispatch.

It does not directly work with arbitrary tuples:

```cpp
std::tuple<int, int> value;

// switch (value) // invalid
```

It also cannot directly switch on `std::variant`.

---

# 16. Modern `switch` with initialization

Since C++17:

```cpp
switch (auto state = GetState(); state)
{
    case State::Idle:
        break;

    case State::Running:
        break;

    case State::Error:
        break;
}
```

Conceptually:

```cpp
{
    auto state = GetState();

    switch (state)
    {
        // ...
    }
}
```

The initialized variable has limited scope.

---

# 17. Matching `std::variant`

`std::variant` stores exactly one value from a fixed set of possible types.

```cpp
using Value = std::variant<
    int,
    double,
    std::string
>;

Value value = std::string{"hello"};
```

At runtime:

```text
variant
└── currently contains std::string
```

This is C++'s standard discriminated-union type.

---

# 18. `std::get_if`

A simple way to check a `variant` alternative:

```cpp
if (auto* number = std::get_if<int>(&value))
{
    std::println("int: {}", *number);
}
else if (auto* text = std::get_if<std::string>(&value))
{
    std::println("string: {}", *text);
}
```

`std::get_if<T>` returns:

```text
T*       if T is active
nullptr  otherwise
```

It does not throw.

---

# 19. `std::holds_alternative`

Another option:

```cpp
if (std::holds_alternative<int>(value))
{
    int x = std::get<int>(value);

    std::println("{}", x);
}
```

Often `std::get_if` is more convenient because it checks and retrieves the value at the same time.

---

# 20. `std::visit`: the main `variant` matching mechanism

```cpp
using Value = std::variant<
    int,
    double,
    std::string
>;
```

A generic visitor:

```cpp
Value value = std::string{"hello"};

std::visit(
    [](const auto& x)
    {
        std::println("{}", x);
    },
    value
);
```

For different behavior per type, a common helper is:

```cpp
template<class... Ts>
struct Overload : Ts...
{
    using Ts::operator()...;
};
```

Then:

```cpp
std::visit(
    Overload{
        [](int x)
        {
            std::println("int: {}", x);
        },

        [](double x)
        {
            std::println("double: {}", x);
        },

        [](const std::string& x)
        {
            std::println("string: {}", x);
        }
    },
    value
);
```

Conceptually:

```text
match value
{
    int x       -> ...
    double x    -> ...
    string text -> ...
}
```

This is the current standard C++ approximation of runtime type-pattern matching.

---

# 21. Avoid `switch (variant.index())`

Technically:

```cpp
std::variant<int, double, std::string> value;

switch (value.index())
{
    case 0:
        // int
        break;

    case 1:
        // double
        break;

    case 2:
        // std::string
        break;
}
```

But the indexes depend on the alternative order.

```cpp
std::variant<
    int,          // 0
    double,       // 1
    std::string   // 2
>
```

If changed to:

```cpp
std::variant<
    bool,         // 0
    int,          // 1
    double,       // 2
    std::string   // 3
>
```

all indexes move.

Prefer:

```cpp
std::visit
```

for type-based behavior.

---

# 22. Compile-time matching with `if constexpr`

Different types can select different branches during template instantiation.

```cpp
template<typename T>
void Describe(T value)
{
    if constexpr (std::is_integral_v<T>)
    {
        std::println("integer: {}", value);
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        std::println("floating point: {}", value);
    }
    else
    {
        std::println("other");
    }
}
```

Usage:

```cpp
Describe(42);
Describe(3.14);
```

This is compile-time selection, not runtime type matching.

Mental model:

```text
std::visit    -> runtime alternative type
if constexpr  -> compile-time type/property
switch        -> runtime value
structured binding -> decomposition
```

---

# 23. Concepts as compile-time matching

C++20 concepts can constrain templates based on type properties.

```cpp
template<typename T>
concept Number =
    std::integral<T> ||
    std::floating_point<T>;
```

Usage:

```cpp
template<Number T>
void Calculate(T value)
{
}
```

Or:

```cpp
template<typename T>
requires std::integral<T>
void Process(T value)
{
}
```

Concept-based overloads:

```cpp
void Process(std::integral auto value)
{
    std::println("integer: {}", value);
}

void Process(std::floating_point auto value)
{
    std::println("floating point: {}", value);
}
```

Usage:

```cpp
Process(42);
Process(3.14);
```

The compiler selects the valid overload.

---

# 24. Template specialization as compile-time matching

Primary template:

```cpp
template<typename T>
struct Info
{
    static constexpr std::string_view name = "unknown";
};
```

Full specialization:

```cpp
template<>
struct Info<int>
{
    static constexpr std::string_view name = "integer";
};
```

Usage:

```cpp
static_assert(Info<int>::name == "integer");
static_assert(Info<double>::name == "unknown");
```

This is another form of compile-time type matching.

---

# 25. Partial specialization matches type shape

Example:

```cpp
template<typename T>
struct Traits;
```

Match any pointer type:

```cpp
template<typename T>
struct Traits<T*>
{
    using element_type = T;
};
```

The compiler can match:

```text
int*
double*
MyClass*
```

against:

```text
T*
```

and infer `T`.

For example:

```cpp
static_assert(std::is_same_v<
    Traits<int*>::element_type,
    int
>);
```

---

# 26. Matching template structure

Partial specialization can also match a template pattern.

```cpp
template<typename T>
struct Traits;
```

Match `std::vector<T>`:

```cpp
template<typename T>
struct Traits<std::vector<T>>
{
    using element_type = T;
};
```

Usage:

```cpp
using T = Traits<std::vector<int>>::element_type;

static_assert(std::is_same_v<T, int>);
```

Conceptually:

```text
std::vector<int>
       |
       v
std::vector<T>
       |
       v
T = int
```

This is genuine compile-time structural matching.

---

# 27. Compact mental map

Keep these mechanisms separate:

```text
1. STRUCTURED BINDING
   object -> components

   auto [x, y] = point;


2. SWITCH
   runtime value -> branch

   switch (state) { ... }


3. VARIANT + VISIT
   runtime alternative type -> handler

   std::visit(visitor, value);


4. IF CONSTEXPR / CONCEPTS
   compile-time type/property -> code

   if constexpr (...)


5. TEMPLATE SPECIALIZATION
   compile-time type shape -> implementation

   Traits<T*>
```

They may look like different forms of "matching", but they solve different problems.

---

# 28. General pattern matching

A general language-level pattern-matching facility has been proposed for C++, covering concepts such as:

- value matching,
- tuple-like matching,
- variant matching,
- optional-like matching,
- destructuring.

Possible proposal syntax resembles:

```cpp
value match {
    0 => ...,
    1 => ...,
    _ => ...
};
```

But this is **not standard C++26**.

For an interview:

> **C++26 does not yet provide general language-level pattern matching. Runtime variant matching is normally implemented with `std::visit`, while structured bindings provide decomposition rather than matching.**

---

# Interview answer in ~30 seconds

Structured bindings were introduced in C++17 and let us decompose arrays, tuple-like objects, and structs:

```cpp
auto [x, y] = point;
```

They can bind by value or reference. C++26 extends them with structured-binding packs and allows structured bindings in conditions.

For other forms of matching:

- `switch` handles runtime integral or enum values.
- `std::visit` handles runtime alternatives of `std::variant`.
- `if constexpr` and concepts perform compile-time selection.
- Template specialization performs compile-time matching against type structure.
- C++26 still has no general Rust-style `match` expression.

---

# Core points to remember

```cpp
// decomposition
auto [x, y] = object;

// reference decomposition
auto& [x, y] = object;

// map
for (const auto& [key, value] : map)
{
}

// C++17 init-statement
if (auto [it, inserted] = set.insert(x); inserted)
{
}

// C++26 structured-binding pack
auto [first, ...rest] = object;

// C++26 pack indexing
args...[I];

// tuple -> function arguments
std::apply(function, tuple);

// runtime value matching
switch (state)
{
}

// runtime variant/type matching
std::visit(visitor, variant);

// compile-time selection
if constexpr (std::integral<T>)
{
}

// compile-time constraint
template<std::integral T>
void Foo(T value);

// compile-time type-shape matching
template<typename T>
struct Trait<T*>
{
};
```

---

# Cheat Sheet

| Need | Use |
|---|---|
| Decompose struct / tuple / pair / array | Structured binding |
| Modify decomposed original object | `auto& [a, b]` |
| Read without copying | `const auto& [a, b]` |
| Convert tuple into function arguments | `std::apply` |
| Match enum / integer runtime value | `switch` |
| Match `std::variant` runtime type | `std::visit` |
| Safely test one variant type | `std::get_if<T>` |
| Test active variant type | `std::holds_alternative<T>` |
| Select code by compile-time property | `if constexpr` |
| Restrict templates by type properties | Concepts / `requires` |
| Match a compile-time type shape | Partial specialization |
| Decompose an unknown number of members | C++26 structured-binding pack |
| Index into parameter/binding packs | C++26 pack indexing |
| General Rust-style pattern matching | Not in C++26 |
