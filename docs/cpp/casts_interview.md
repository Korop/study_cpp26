# C++26 Casts — Interview Notes

Focus: `static_cast`, `dynamic_cast`, `void*`, `const_cast`, `reinterpret_cast`, `std::bit_cast`, C-style casts, interview traps.

## 1. Cast overview

| Cast | Purpose | Runtime type check |
|---|---|---:|
| `static_cast<T>(x)` | Explicit language-defined conversion | No |
| `dynamic_cast<T>(x)` | Checked class-hierarchy conversion | Yes, when needed |
| `const_cast<T>(x)` | Change `const` / `volatile` qualification | No |
| `reinterpret_cast<T>(x)` | Low-level reinterpretation | No |
| `std::bit_cast<T>(x)` | Copy object representation as another type | No |
| `(T)x` | C-style cast | Avoid |

```text
static_cast  -> "I know this conversion is correct."
dynamic_cast -> "Check the runtime object."
```

---

# 2. `static_cast`

## Numeric conversion

```cpp
double value = 10.8;
int result = static_cast<int>(value);

std::println("{}", result); // 10
```

## Upcast: `Derived -> Base`

```cpp
struct Animal
{
    virtual ~Animal() = default;
};

struct Dog : Animal {};

Dog dog;

Animal* a1 = &dog;                        // preferred
Animal* a2 = static_cast<Animal*>(&dog); // valid, unnecessary
```

Public derived-to-base conversion is normally implicit.

## Downcast: `Base -> Derived`

```cpp
Dog dog;
Animal* animal = &dog;

Dog* dogPtr = static_cast<Dog*>(animal);
```

Valid only when the program already guarantees that `animal` points to the `Animal` base subobject of a `Dog`.

No runtime type check is performed.

```cpp
Animal animal;
Animal* base = &animal;

// Dog* wrong = static_cast<Dog*>(base); // wrong dynamic type -> UB
```

### Interview answer

> `static_cast` can downcast, but it is safe only when the actual derived type is guaranteed by other program logic.

---

# 3. `dynamic_cast`

Used for conversions through class hierarchies.

Runtime-checked downcasts, cross-casts, and `dynamic_cast<void*>` require a **polymorphic** source object.

```cpp
struct Base
{
    virtual ~Base() = default;
};

static_assert(std::is_polymorphic_v<Base>);
```

## Safe downcast

```cpp
struct Dog : Base {};
struct Cat : Base {};

Dog dog;
Base* base = &dog;

Dog* result = dynamic_cast<Dog*>(base);

if (result)
{
    std::println("Dog");
}
```

## Failed cast

Pointer:

```cpp
Cat cat;
Base* base = &cat;

Dog* dog = dynamic_cast<Dog*>(base);

std::println("{}", dog == nullptr); // true
```

```text
dynamic_cast<T*>(...) failure -> nullptr
```

Reference:

```cpp
try
{
    [[maybe_unused]] Dog& dog = dynamic_cast<Dog&>(*base);
}
catch (const std::bad_cast&)
{
    std::println("Not a Dog");
}
```

```text
dynamic_cast<T&>(...) failure -> throws std::bad_cast
```

This pointer/reference difference is a common interview question.

---

# 4. Cross-casting

`dynamic_cast` can move sideways through a polymorphic multiple-inheritance hierarchy.

```cpp
struct Component
{
    virtual ~Component() = default;
};

struct Renderable : virtual Component {};
struct Updatable  : virtual Component {};
struct Player : Renderable, Updatable {};

Player player;

Renderable* renderable = &player;
Updatable* updatable = dynamic_cast<Updatable*>(renderable);

std::println("{}", updatable != nullptr); // true
```

```text
        Component
        /       \
Renderable   Updatable
        \       /
          Player
```

`Renderable* -> Updatable*` is a **cross-cast**.

---

# 5. What is `void*`?

`void*` is a generic **object pointer**.

It stores an address without specifying the pointee type.

```cpp
int value = 42;

int* typed = &value;
void* raw = typed; // implicit T* -> void*
```

```text
int*
 |
 | forget pointee type
 v
void*
```

The actual object remains an `int`.

## Cannot dereference it directly

```cpp
void* raw = &value;

// std::println("{}", *raw); // error
```

Convert it back when the real type is known:

```cpp
int* restored = static_cast<int*>(raw);

std::println("{}", *restored); // 42
```

## No standard pointer arithmetic

```cpp
// ++raw; // error
```

`int* + 1` advances by `sizeof(int)`. `void*` has no element type/size.

For raw byte access, `std::byte*` is often a better representation.

## `void*` != `nullptr`

```cpp
void* p1 = &value;   // some object, type unspecified
void* p2 = nullptr;  // points nowhere
```

---

# 6. `dynamic_cast<void*>`

Special `dynamic_cast` form:

```cpp
void* whole = dynamic_cast<void*>(polymorphicBasePointer);
```

It returns a pointer to the **complete most-derived object** containing the source subobject.

This matters especially with multiple inheritance.

```cpp
struct A
{
    virtual ~A() = default;
};

struct B
{
    virtual ~B() = default;
};

struct C : A, B
{
    int value = 42;
};

C object;
B* b = &object;

void* bSubobject = static_cast<void*>(b);
void* wholeObject = dynamic_cast<void*>(b);

std::println("B subobject : {}", bSubobject);
std::println("whole C     : {}", wholeObject);
```

Possible layout:

```text
C object
0x1000
+----------------+
| A subobject    | <- 0x1000
+----------------+
| B subobject    | <- 0x1008
+----------------+
| C::value       |
+----------------+
```

Conceptually:

```text
B* b                    -> 0x1008
static_cast<void*>(b)   -> 0x1008
dynamic_cast<void*>(b)  -> 0x1000
```

So:

```text
static_cast<void*>(p)
    -> keep p's object-pointer value, forget pointee type

dynamic_cast<void*>(p)
    -> find complete most-derived polymorphic object
```

---

# 7. `const_cast`

Changes cv-qualification.

Safe example because the original object is non-const:

```cpp
int value = 10;

const int* p = &value;
int* mutablePtr = const_cast<int*>(p);

*mutablePtr = 20;

std::println("{}", value); // 20
```

Danger:

```cpp
const int value = 10;

const int* p = &value;
int* mutablePtr = const_cast<int*>(p);

// *mutablePtr = 20; // UB: original object is const
```

> Removing `const` from a pointer/reference is possible. Modifying an object originally defined as `const` is undefined behavior.

---

# 8. `reinterpret_cast`

Used for low-level conversions.

Example: pointer -> integer representation.

```cpp
int value = 42;
int* p = &value;

std::uintptr_t address = reinterpret_cast<std::uintptr_t>(p);

std::println("{:#x}", address);
```

It does not make an incompatible object safe to access.

Avoid type-punning like:

```cpp
double value = 3.14;

// auto bits = *reinterpret_cast<std::uint64_t*>(&value); // unsafe approach
```

Use `std::bit_cast` when the goal is to inspect the object representation.

## `reinterpret_cast<void*>`?

For ordinary object pointers, it is unnecessary:

```cpp
int value = 42;
int* p = &value;

void* raw1 = p;                        // preferred
void* raw2 = static_cast<void*>(p);   // explicit
```

---

# 9. `std::bit_cast`

Since C++20.

```cpp
float value = 1.0f;

std::uint32_t bits = std::bit_cast<std::uint32_t>(value);

std::println("{:#x}", bits);
```

Compare:

```cpp
auto numeric = static_cast<std::uint32_t>(value); // numeric conversion
auto bits    = std::bit_cast<std::uint32_t>(value); // representation
```

```text
static_cast -> convert value
bit_cast    -> preserve/copy representation
```

`std::bit_cast` requires equal sizes and trivially copyable source/target types.

---

# 10. `std::move` and `static_cast`

`std::move` does not move anything itself.

Conceptually:

```cpp
static_cast<std::remove_reference_t<T>&&>(value)
```

Example:

```cpp
std::string source = "hello";
std::string target = std::move(source);
```

`std::move` produces an xvalue. The move constructor/assignment performs the actual move.

### Interview answer

> `std::move` is essentially a cast to an rvalue reference; it enables move semantics but does not itself move the object.

---

# 11. C-style casts

Avoid:

```cpp
Dog* dog = (Dog*)base;
```

Prefer named casts:

```cpp
Dog* known = static_cast<Dog*>(base);
Dog* checked = dynamic_cast<Dog*>(base);
```

Named casts expose intent and are easier to review/search.

---

# 12. Compact C++26 example

```cpp
#include <bit>
#include <cstdint>
#include <print>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>

struct Animal
{
    virtual ~Animal() = default;
};

struct Dog : Animal
{
    void Bark() const
    {
        std::println("Woof");
    }
};

struct Cat : Animal {};

struct A
{
    virtual ~A() = default;
};

struct B
{
    virtual ~B() = default;
};

struct C : A, B
{
    int value = 42;
};

int main()
{
    static_assert(std::is_polymorphic_v<Animal>);

    // static_cast: numeric conversion.
    double d = 10.8;
    int i = static_cast<int>(d);
    std::println("numeric: {}", i);

    // Upcast: implicit conversion is preferred.
    Dog dog;
    Animal* animal = &dog;

    // static_cast downcast: safe only because we know this is Dog.
    Dog* knownDog = static_cast<Dog*>(animal);
    knownDog->Bark();

    // dynamic_cast: runtime-checked downcast.
    if (auto* checkedDog = dynamic_cast<Dog*>(animal))
    {
        checkedDog->Bark();
    }

    // Failed pointer dynamic_cast -> nullptr.
    Cat cat;
    Animal* catAsAnimal = &cat;
    std::println("Dog? {}", dynamic_cast<Dog*>(catAsAnimal) != nullptr);

    // Failed reference dynamic_cast -> std::bad_cast.
    try
    {
        [[maybe_unused]] Dog& bad = dynamic_cast<Dog&>(*catAsAnimal);
    }
    catch (const std::bad_cast&)
    {
        std::println("reference cast failed");
    }

    // void*: forget and restore pointee type.
    int value = 42;
    void* raw = &value;
    int* restored = static_cast<int*>(raw);
    std::println("void*: {}", *restored);

    // dynamic_cast<void*>: find complete most-derived object.
    C object;
    B* b = &object;

    void* bSubobject = static_cast<void*>(b);
    void* complete = dynamic_cast<void*>(b);

    std::println("B subobject: {}", bSubobject);
    std::println("complete C : {}", complete);

    // const_cast: safe because original object is non-const.
    int mutableValue = 10;
    const int* constView = &mutableValue;
    int* mutableView = const_cast<int*>(constView);
    *mutableView = 20;
    std::println("const_cast: {}", mutableValue);

    // reinterpret_cast: pointer -> integer representation.
    std::uintptr_t address = reinterpret_cast<std::uintptr_t>(&mutableValue);
    std::println("address: {:#x}", address);

    // bit_cast: representation, not numeric conversion.
    float f = 1.0f;
    auto bits = std::bit_cast<std::uint32_t>(f);
    std::println("float bits: {:#x}", bits);

    // std::move enables move semantics.
    std::string source = "hello";
    std::string target = std::move(source);
    std::println("target: {}", target);
}
```

---

# 13. Interview questions

### Q1. `static_cast` vs `dynamic_cast`?

`static_cast` performs no runtime dynamic-type check.

`dynamic_cast` can check the runtime class hierarchy.

### Q2. Does `dynamic_cast` always require a polymorphic class?

No.

Simple identity/upcast cases do not require runtime RTTI checking.

Runtime-checked downcasts, cross-casts, and `dynamic_cast<void*>` require polymorphism.

### Q3. Failed `dynamic_cast<T*>`?

```cpp
nullptr
```

### Q4. Failed `dynamic_cast<T&>`?

```cpp
std::bad_cast
```

### Q5. Can `static_cast` downcast?

Yes, when the relationship permits it, but the actual derived type must already be guaranteed.

### Q6. What is `void*`?

A generic object pointer containing an address without a pointee type.

### Q7. Can `void*` be dereferenced?

No. Convert it to the correct typed pointer first.

### Q8. Why `dynamic_cast<void*>`?

To get the complete most-derived polymorphic object from a pointer to one of its subobjects.

### Q9. `static_cast<void*>` vs `dynamic_cast<void*>`?

```text
static_cast<void*>  -> forget pointee type
dynamic_cast<void*> -> find most-derived complete object
```

### Q10. Use `reinterpret_cast<void*>(p)` for `T* -> void*`?

Usually no.

```cpp
void* raw = p;
```

is sufficient.

### Q11. `static_cast` vs `std::bit_cast`?

```text
static_cast -> value conversion
bit_cast    -> representation conversion
```

### Q12. Does `std::move` move?

No. It produces an xvalue; a move constructor/assignment performs the move.

### Q13. Is checked `dynamic_cast` slower than `static_cast`?

It has additional runtime hierarchy/type checking. Use it when that check is actually required.

---

# 14. Interview answer in ~30 seconds

> C++ has four named casts. `static_cast` handles explicit language-defined conversions such as numeric and related-class conversions but does not runtime-check a downcast. `dynamic_cast` is used for checked polymorphic hierarchy conversions; a failed pointer cast returns `nullptr`, while a failed reference cast throws `std::bad_cast`. `const_cast` changes cv-qualification, and modifying an originally const object is undefined behavior. `reinterpret_cast` is for low-level conversions such as pointer-to-integer conversion. I avoid C-style casts because they hide intent. `std::bit_cast` is different: it works with object representation rather than normal value conversion.

---

# 15. Cheat Sheet

```text
static_cast
├─ numeric conversion
├─ explicit related-type conversion
├─ upcast (usually implicit)
├─ downcast if type is guaranteed
├─ T* <-> void*
└─ no runtime type check

dynamic_cast
├─ polymorphic runtime checking
├─ checked downcast
├─ cross-cast
├─ T* failure -> nullptr
├─ T& failure -> std::bad_cast
└─ void* -> complete most-derived object

const_cast
└─ change const/volatile qualification

reinterpret_cast
├─ low-level pointer conversions
├─ pointer <-> integer
└─ does not make incompatible access safe

std::bit_cast
└─ copy object representation
```

## Core points to remember

1. Prefer named casts over C-style casts.
2. `static_cast` does not verify the runtime dynamic type.
3. Use `dynamic_cast` when runtime hierarchy checking is genuinely required.
4. Pointer cast failure -> `nullptr`; reference cast failure -> `std::bad_cast`.
5. `void*` is a generic object pointer, not a null pointer.
6. `static_cast<void*>` forgets the pointee type.
7. `dynamic_cast<void*>` finds the complete most-derived polymorphic object.
8. `reinterpret_cast` is low-level and does not bypass object lifetime or aliasing rules.
9. `std::bit_cast` is the modern tool for representation-level conversion.
10. `std::move` enables move semantics; it does not itself move.
