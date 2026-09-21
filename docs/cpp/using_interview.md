# C++ `using` — Study Notes

`using` has several distinct meanings in C++. The syntax looks similar, but the language feature depends on what follows `using`.

## Quick reference

| Syntax | Meaning |
|---|---|
| `using Id = int;` | Type alias |
| `using Ptr = T*;` | Alias template |
| `using std::println;` | Introduce one namespace member |
| `using namespace std::literals;` | Using-directive for a namespace |
| `using Base::Print;` | Introduce a base-class member into derived scope |
| `using Base::Base;` | Inherit constructors |

---

## 1. Type alias

Modern replacement for many uses of `typedef`.

```cpp
using PlayerId = unsigned long long;
using PlayerName = std::string;

PlayerId id{42};
PlayerName name{"Ivan"};
```

Equivalent old-style form:

```cpp
typedef unsigned long long PlayerId;
```

Prefer `using` in modern C++ because it reads left-to-right and supports alias templates cleanly.

### Alias template

```cpp
template<typename T>
using Ptr = T*;

Ptr<int> p;        // int*
Ptr<double> q;     // double*
```

Example check:

```cpp
static_assert(std::is_same_v<Ptr<int>, int*>);
```

---

## 2. `using namespace_member;`

A using-declaration can introduce one existing name from a namespace into the current scope.

```cpp
using std::println;
using std::string;

string name{"Ivan"};
println("name={}", name);
```

This is more selective than:

```cpp
using namespace std;
```

and normally causes fewer name-collision problems.

---

## 3. `using namespace ...;`

A using-directive makes names from a namespace available for unqualified lookup.

```cpp
using namespace std::string_literals;

auto text = "Hello"s;
```

Here `"Hello"s` works because the string literal operator from `std::string_literals` is available.

Avoid broad directives such as:

```cpp
using namespace std;
```

in headers and usually at global scope because they can introduce many names and create ambiguities.

Local function scope is less dangerous:

```cpp
void Foo()
{
    using namespace std::string_literals;

    auto text = "Hello"s;
}
```

---

## 4. `using Base::member;`

A derived-class member with the same name hides the complete base-class overload set during normal name lookup.

```cpp
class Base
{
public:
    void Print(int);
    void Print(double);
};

class Derived : public Base
{
public:
    void Print(std::string_view);
};
```

Now:

```cpp
Derived d;

// d.Print(10);
```

does not automatically select `Base::Print(int)`.

`Derived::Print` hides the base overloads by name.

Bring them back with:

```cpp
class Derived : public Base
{
public:
    using Base::Print;

    void Print(std::string_view);
};
```

Now the visible overload set contains:

```cpp
Base::Print(int)
Base::Print(double)
Derived::Print(std::string_view)
```

and overload resolution selects the best match.

Important:

> `using Base::Print;` does not copy or override the base method. It introduces the existing base member name into the derived scope.

---

## 5. `using Base::Base;`

This syntax inherits constructors from a base class.

```cpp
class Base
{
public:
    Base() = default;
    Base(int value);
    Base(std::string_view name);
};

class Derived : public Base
{
public:
    using Base::Base;
};
```

Now these are valid:

```cpp
Derived a{};
Derived b{42};
Derived c{"Ivan"};
```

The inherited constructors initialize the `Base` subobject of `Derived`.

This does **not** mean C++ literally copies the constructor source code into `Derived`.

It also does not mean:

```cpp
using Base::Base;
```

inherits every special member function in the ordinary sense. `Derived` still has its own copy/move construction rules.

---

## `using Base::Base` vs `using Base::Foo`

They look similar but solve different problems.

```cpp
using Base::Base;
```

means:

> Make base constructors available for constructing the derived class.

While:

```cpp
using Base::Foo;
```

means:

> Introduce the existing base member named `Foo` into the derived scope.

---

## Complete study sample

```cpp
#include <print>
#include <string>
#include <string_view>
#include <type_traits>

// ------------------------------------------------------------
// 1. Type aliases
// ------------------------------------------------------------

using PlayerId = unsigned long long;
using PlayerName = std::string;

template<typename T>
using Ptr = T*;


// ------------------------------------------------------------
// Base class used by the inheritance examples
// ------------------------------------------------------------

class Base
{
public:
    Base()
    {
        std::println("Base()");
    }

    Base(int value)
    {
        std::println("Base(int): {}", value);
    }

    Base(std::string_view name)
    {
        std::println("Base(string_view): {}", name);
    }

    void Print(int value) const
    {
        std::println("Base::Print(int): {}", value);
    }

    void Print(double value) const
    {
        std::println("Base::Print(double): {}", value);
    }
};


// ------------------------------------------------------------
// 2. using Base::Base
//    Inherit constructors
// ------------------------------------------------------------

class Derived : public Base
{
public:
    using Base::Base;
};


// ------------------------------------------------------------
// 3. using Base::Print
//    Reintroduce hidden base overloads
// ------------------------------------------------------------

class DerivedWithPrint : public Base
{
public:
    using Base::Base;
    using Base::Print;

    void Print(std::string_view text) const
    {
        std::println(
            "DerivedWithPrint::Print(string_view): {}",
            text
        );
    }
};


// ------------------------------------------------------------
// 4. Introduce individual namespace members
// ------------------------------------------------------------

void NamespaceMemberExample()
{
    using std::println;
    using std::string;

    string name{"Ivan"};

    println("namespace member -> {}", name);
}


// ------------------------------------------------------------
// 5. using namespace
// ------------------------------------------------------------

void NamespaceDirectiveExample()
{
    using namespace std::string_literals;

    auto text = "Hello"s;

    std::println(
        "namespace directive -> {}",
        text
    );
}


int main()
{
    PlayerId id{42};
    PlayerName name{"Ivan"};

    int value{100};
    Ptr<int> ptr{&value};

    static_assert(
        std::is_same_v<PlayerId, unsigned long long> &&
        std::is_same_v<PlayerName, std::string> &&
        std::is_same_v<Ptr<int>, int*>
    );

    std::println(
        "id={}, name={}, *ptr={}",
        id,
        name,
        *ptr
    );

    // Inherited constructors.
    Derived d1{};
    Derived d2{42};
    Derived d3{"Ivan"};

    // Base overloads + derived overload form one visible overload set.
    DerivedWithPrint object{};

    object.Print(10);       // Base::Print(int)
    object.Print(3.14);     // Base::Print(double)
    object.Print("Hello");  // DerivedWithPrint::Print(string_view)

    NamespaceMemberExample();
    NamespaceDirectiveExample();
}
```

---

## Interview notes

Remember these five forms:

```cpp
using X = T;             // type alias

using std::println;      // introduce one namespace member

using namespace ns;      // using-directive

using Base::Foo;         // introduce base member / overloads

using Base::Base;        // inherit constructors
```

### Important traps

`using Base::Foo;` is especially useful when a derived declaration hides base overloads:

```cpp
class Derived : public Base
{
public:
    using Base::Foo;

    void Foo(std::string_view);
};
```

Without the `using` declaration, base overloads named `Foo` are hidden during normal lookup.

`using namespace ns;` and `using ns::name;` are different:

```cpp
using namespace std;  // broad namespace directive
using std::string;    // one selected name
```

For normal production code, prefer the narrower form when practical.

---

## Short interview answer

`using` is context-dependent in C++. It can define a type alias, define an alias template, introduce one namespace member, create a namespace using-directive, expose a hidden base-class member in derived scope, or inherit base constructors. `using Base::Foo` affects name lookup and overload visibility, while `using Base::Base` is the special syntax for inherited constructors.

---

# 40 Interview Questions and Answers

## 1. What are the main meanings of `using` in C++?

`using` can define a type alias, define an alias template, introduce a namespace member, create a namespace using-directive, expose a base-class member in derived scope, inherit base constructors, or introduce enum enumerators with `using enum`.

## 2. What is a type alias?

A type alias gives another name to an existing type.

```cpp
using PlayerId = unsigned long long;
```

`PlayerId` and `unsigned long long` are the same type.

## 3. Is `using X = T;` different from `typedef T X;`?

For simple aliases they are equivalent in meaning.

```cpp
using PlayerId = unsigned long long;
typedef unsigned long long PlayerIdOld;
```

Modern C++ generally prefers `using` because it reads more naturally and supports alias templates.

## 4. Why is `using` better than `typedef` for templates?

Because alias templates are directly supported:

```cpp
template<typename T>
using Ptr = T*;
```

There is no equally direct `typedef` syntax for an alias template.

## 5. What is an alias template?

It is a template that produces a type alias.

```cpp
template<typename T>
using Ptr = T*;
```

Then:

```cpp
Ptr<int>       // int*
Ptr<double>    // double*
```

## 6. Does a type alias create a new type?

No.

```cpp
using PlayerId = unsigned long long;
```

`PlayerId` is only another name for `unsigned long long`.

## 7. What does `using std::string;` mean?

It introduces the existing name `std::string` into the current scope.

```cpp
using std::string;

string name{"Ivan"};
```

## 8. Does `using std::string;` import the whole `std` namespace?

No. It introduces only the selected name.

## 9. What is the difference between `using std::string;` and `using namespace std;`?

```cpp
using std::string;
```

introduces one name.

```cpp
using namespace std;
```

makes names from the whole namespace available for unqualified lookup.

## 10. What is a using-declaration?

A using-declaration introduces an existing name into another scope.

Examples:

```cpp
using std::string;
using Base::Print;
```

## 11. What is a using-directive?

This form:

```cpp
using namespace std;
```

is a using-directive.

It affects unqualified name lookup for names in that namespace.

## 12. Why is `using namespace std;` discouraged in headers?

Because every translation unit including that header can have extra names participating in lookup, increasing the risk of ambiguities and name collisions.

## 13. Is `using namespace` always bad?

No. A narrow using-directive inside a small local scope can be reasonable.

```cpp
void Foo()
{
    using namespace std::string_literals;

    auto text = "Hello"s;
}
```

## 14. What does `using namespace std::string_literals;` provide?

It makes standard string literal operators available, such as:

```cpp
"Hello"s
```

which produces a `std::string`.

## 15. What does `using Base::Print;` do?

It introduces `Base::Print` into the derived-class scope.

This is commonly used to make hidden base overloads visible again.

## 16. Why can a derived method hide base methods with different parameter types?

C++ name lookup first works by name.

If the derived class declares a member named `Print`, base members with the same name are hidden from ordinary lookup before overload resolution considers parameter types.

## 17. Give an example of base overload hiding.

```cpp
class Base
{
public:
    void Print(int);
    void Print(double);
};

class Derived : public Base
{
public:
    void Print(std::string_view);
};
```

Here `Derived::Print` hides the `Base::Print` overload set during normal lookup.

## 18. How do you restore hidden base overloads?

Use:

```cpp
using Base::Print;
```

Example:

```cpp
class Derived : public Base
{
public:
    using Base::Print;

    void Print(std::string_view);
};
```

## 19. Does `using Base::Print;` override `Base::Print`?

No.

It does not create a virtual override and does not copy the function. It only affects visibility and name lookup.

## 20. Does `using Base::Print;` copy the base function into `Derived`?

No.

The function still belongs to `Base`. The using-declaration makes the name visible in derived scope.

## 21. Can `using Base::Foo;` expose several overloads at once?

Yes.

```cpp
void Foo(int);
void Foo(double);
void Foo(std::string_view);
```

Then:

```cpp
using Base::Foo;
```

makes that overload set visible.

## 22. What happens after base and derived overloads are visible together?

Normal overload resolution selects the best match.

```cpp
object.Print(10);       // Base::Print(int)
object.Print(3.14);     // Base::Print(double)
object.Print("Hello");  // derived overload
```

## 23. What does `using Base::Base;` mean?

It enables inherited constructors from `Base`.

```cpp
class Derived : public Base
{
public:
    using Base::Base;
};
```

## 24. What problem does `using Base::Base;` solve?

It avoids manually forwarding every base constructor.

Instead of writing many wrappers such as:

```cpp
Derived(int value)
    : Base(value)
{
}
```

the derived class can inherit the base constructor set.

## 25. If `Base` has `Base(int)`, can `Derived` be constructed from `int` after `using Base::Base;`?

Yes.

```cpp
Derived value{42};
```

can use the inherited constructor.

## 26. What gets initialized by an inherited constructor?

The inherited constructor initializes the `Base` subobject of the `Derived` object.

Derived members still follow their own initialization rules.

## 27. Does `using Base::Base;` literally copy constructor bodies into `Derived`?

No.

Inherited constructors are a language mechanism, not textual copying.

## 28. Does `using Base::Base;` inherit copy constructors?

Not in the sense of replacing the derived class's own special member rules.

`Derived` still has its own copy/move constructor behavior.

## 29. Does `using Base::Base;` inherit the destructor?

No.

Destructors are not inherited through this syntax.

## 30. What is the difference between `using Base::Base;` and `using Base::Foo;`?

```cpp
using Base::Base;
```

inherits constructors for constructing `Derived`.

```cpp
using Base::Foo;
```

introduces an existing base member into derived scope.

## 31. Is `using Base::Base;` just an ordinary member using-declaration?

It uses the same `using Base::...` syntax, but constructor inheritance has special language rules.

## 32. Can a derived class still define its own constructor after `using Base::Base;`?

Yes.

```cpp
class Derived : public Base
{
public:
    using Base::Base;

    Derived()
        : Base(100)
    {
    }
};
```

## 33. Can an inherited constructor run custom initialization code for derived members?

Not through the base constructor implementation itself.

If custom derived initialization is required, define an explicit `Derived` constructor.

## 34. Can access control affect inherited constructors?

Yes.

Normal access rules still matter. `using Base::Base;` does not magically bypass constructor accessibility.

## 35. Can `using Base::member;` change access level in the derived class?

A using-declaration can expose an accessible base member under the access section where the declaration appears.

```cpp
class Base
{
protected:
    void Foo();
};

class Derived : public Base
{
public:
    using Base::Foo;
};
```

`Foo` becomes publicly accessible through `Derived`.

## 36. What does `using enum` do?

Given:

```cpp
enum class Color
{
    Red,
    Green,
    Blue
};
```

you can write:

```cpp
using enum Color;
```

and use:

```cpp
Red
Green
Blue
```

without repeating `Color::`.

## 37. Since when is `using enum` available?

Since C++20.

It remains part of modern C++26 code.

## 38. When should you prefer `using std::name;` over `using namespace std;`?

Prefer the selective form when you need only a few names:

```cpp
using std::println;
using std::string;
```

It keeps the visible namespace surface smaller and reduces ambiguity.

## 39. What are the main `using` forms worth memorizing for an interview?

```cpp
using X = T;             // type alias
using std::println;      // one namespace member
using namespace ns;      // namespace directive
using Base::Foo;         // expose base member / overloads
using Base::Base;        // inherit constructors
using enum EnumType;     // introduce enumerators
```

## 40. Give a compact interview explanation of `using`.

`using` is context-dependent in C++. It can define a type alias or alias template, introduce one namespace member, create a namespace using-directive, expose hidden base-class members in derived scope, inherit base constructors, or introduce enum enumerators with `using enum`. The key distinction is that `using Base::Foo` affects name lookup and overload visibility, while `using Base::Base` activates inherited-constructor rules.

