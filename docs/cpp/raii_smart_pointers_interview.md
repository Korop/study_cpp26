# C++26 Interview — RAII, Smart Pointers, Ownership, and Lifetime

## 1. What is RAII?

**RAII = Resource Acquisition Is Initialization.**

The idea:

> Bind a resource lifetime to the lifetime of a C++ object.

A resource can be:

- heap memory
- file
- socket
- mutex lock
- OS handle
- database connection
- transaction
- GPU resource

When the owning object is destroyed, its destructor releases the resource automatically.

```cpp
#include <memory>
#include <print>
#include <vector>

struct Player
{
    ~Player()
    {
        std::println("~Player()");
    }
};

int main()
{
    {
        std::vector<int> values(1000);
        auto player = std::make_unique<Player>();
    } // vector frees its memory; unique_ptr deletes Player
}
```

### Main goal

**Deterministic and exception-safe resource cleanup.**

```cpp
std::lock_guard lock(mutex);
DoWork(); // even if this throws, lock is released during stack unwinding
```

---

## 2. Main RAII limitations

RAII manages lifetime automatically **only if ownership is designed correctly**.

Important limitations:

1. Raw owning pointers can still leak:
   ```cpp
   auto* p = new Player; // must be deleted manually
   ```

2. `std::shared_ptr` ownership cycles can leak.

3. Complex async lifetime still requires an explicit ownership model.

4. Destructors should not throw.

5. RAII does not decide who should own a resource; it only enforces the lifetime model you define.

---

## 3. Rule of Zero

Prefer composing classes from RAII-aware members.

```cpp
#include <memory>
#include <string>
#include <vector>

struct Weapon {};

class Player
{
    std::string name;
    std::vector<int> data;
    std::unique_ptr<Weapon> weapon;
};
```

No explicit destructor is required.

The compiler provides a destructor equivalent in intent to:

```cpp
~Player() = default;
```

Its members are destroyed automatically in **reverse declaration order**:

```text
weapon
data
name
```

Each member then performs its own cleanup.

---

## 4. Are constructor and destructor created automatically?

If no constructor is declared, the compiler can provide a default constructor:

```cpp
class Player
{
public:
    Player() = default;
    ~Player() = default;
};
```

Conceptually:

```cpp
Player p; // default construction
```

works if all members can be default-constructed.

If you declare another constructor:

```cpp
class Player
{
public:
    explicit Player(std::string name)
        : name_(std::move(name))
    {}

private:
    std::string name_;
};
```

then:

```cpp
Player a{"Alice"}; // OK
// Player b;       // error: no default constructor
```

Add it explicitly if needed:

```cpp
Player() = default;
```

### Special member functions

Modern C++ has six important special member functions:

```cpp
T();
~T();
T(const T&);
T& operator=(const T&);
T(T&&);
T& operator=(T&&);
```

Whether the compiler generates them depends on the class members and on which special members you declare yourself.

---

## 5. `unique_ptr` affects copyability

```cpp
class Player
{
    std::unique_ptr<Weapon> weapon;
};
```

`std::unique_ptr` is non-copyable:

```cpp
auto a = std::make_unique<Weapon>();
// auto b = a; // error

auto b = std::move(a); // ownership transferred
```

Therefore a class containing `unique_ptr` is normally non-copyable unless you implement custom copy behavior.

---

## 6. Does `Player p;` mean stack?

For a normal local variable:

```cpp
void Foo()
{
    Player p;
}
```

`p` has **automatic storage duration**.

It is normally implemented using the thread's stack.

Interview-precise terminology:

```text
local variable           -> automatic storage duration
new / make_unique        -> dynamic storage duration
global / static          -> static storage duration
thread_local             -> thread storage duration
```

Do not reduce this to:

> no `new` = stack

because this is not always true:

```cpp
static Player p;       // static storage
thread_local Player p; // thread storage
```

---

## 7. A stack object can own heap memory

```cpp
void Foo()
{
    std::vector<int> data(1000);
}
```

Conceptually:

```text
automatic storage
+------------------+
| vector           |
| pointer ---------+------+
| size             |      |
| capacity         |      |
+------------------+      |
                          v
dynamic storage
+-------------------------+
| 1000 ints               |
+-------------------------+
```

The `vector` object can be local while its elements use dynamic storage.

The same idea applies to `unique_ptr`.

---

# Smart Pointer Model

## 8. Main pointer types

| Type | Owns object? | Control block? | Copyable? | Main use |
|---|---:|---:|---:|---|
| `T*` | not expressed | No | Yes | non-owning access / low-level interop |
| `std::unique_ptr<T>` | Yes, exclusive | No | No | default dynamic ownership |
| `std::shared_ptr<T>` | Yes, shared | Yes | Yes | shared lifetime |
| `std::weak_ptr<T>` | No | Uses same block as `shared_ptr` | Yes | observe shared object |

---

## 9. `std::unique_ptr`

```cpp
auto player = std::make_unique<Player>();
```

Conceptually:

```text
automatic storage
+-----------------------+
| unique_ptr<Player>    |
| pointer --------------+----+
+-----------------------+    |
                             v
dynamic storage
+-----------------------+
| Player                |
+-----------------------+
```

`unique_ptr` has no reference counter and no shared control block.

It represents:

> exactly one owner

When the `unique_ptr` is destroyed:

```text
~unique_ptr()
    ->
delete Player
    ->
~Player()
```

### Main use

Prefer it when dynamic allocation is required and ownership is exclusive.

```cpp
std::unique_ptr<Player> player = std::make_unique<Player>();
```

---

## 10. `std::shared_ptr`

```cpp
auto a = std::make_shared<Player>();
auto b = a;
```

Now both own the same `Player`.

```text
a ----+
      v
 control block ----> Player
      ^
b ----+

strong count = 2
```

```cpp
std::println("{}", a.use_count()); // 2
```

A simplified mental model:

```cpp
template<class T>
struct SharedPtrLike
{
    T* object;
    ControlBlock* control;
};
```

The real implementation is more complex.

---

## 11. `make_shared` and allocation

This:

```cpp
auto p = std::make_shared<Player>();
```

usually performs one allocation containing both:

```text
+----------------------+
| control block        |
| strong count         |
| weak count           |
| Player               |
+----------------------+
```

Whereas:

```cpp
std::shared_ptr<Player> p(new Player);
```

typically needs separate allocations for:

- `Player`
- control block

Prefer `std::make_shared<T>()` in normal code.

---

## 12. `std::weak_ptr`

`weak_ptr` observes an object managed by `shared_ptr`, but does not own it.

```cpp
auto shared = std::make_shared<Player>();
std::weak_ptr<Player> weak = shared;
```

Conceptually:

```text
shared_ptr ----+
               v
          control block ----> Player
               ^
               |
weak_ptr ------+
```

`weak_ptr` does **not** increase the strong ownership count.

```cpp
std::println("{}", shared.use_count()); // 1
```

---

# Control Block

## 13. How does `weak_ptr` know that the object was deleted?

Because `weak_ptr` does not simply remember the object's address.

It refers to the same **control block** used by `shared_ptr`.

Simplified control block:

```text
control block
+------------------+
| strong count     |
| weak count       |
| deletion info    |
+------------------+
```

Example:

```cpp
std::weak_ptr<Player> weak;

{
    auto shared = std::make_shared<Player>();
    weak = shared;

    // strong = 1
    // weak references exist
}
```

When `shared` is destroyed:

```text
strong: 1 -> 0
```

The `Player` object is destroyed.

But the control block remains while `weak` still exists.

Therefore:

```cpp
weak.expired()
```

can determine that the managed object no longer exists.

---

## 14. `weak_ptr::lock()`

```cpp
if (auto player = weak.lock())
{
    // object still exists
}
```

If the object exists, `lock()` returns a `shared_ptr`.

Its strong count is increased temporarily.

```text
strong: 1 -> 2
```

When the returned `shared_ptr` dies:

```text
strong: 2 -> 1
```

If the strong count has already reached zero:

```cpp
auto p = weak.lock();
```

returns an empty `shared_ptr`.

The object cannot be resurrected.

---

## 15. `weak_ptr` lifecycle example

```cpp
#include <memory>
#include <print>

struct A
{
    ~A()
    {
        std::println("~A()");
    }
};

int main()
{
    std::weak_ptr<A> weak;

    {
        auto shared = std::make_shared<A>();
        weak = shared;

        std::println("count = {}", shared.use_count());
        std::println("expired = {}", weak.expired());

        if (auto temp = weak.lock())
        {
            std::println("after lock count = {}", temp.use_count());
        }

        std::println("count again = {}", shared.use_count());
    }

    std::println("expired after scope = {}", weak.expired());

    auto ptr = weak.lock();
    std::println("lock succeeded = {}", static_cast<bool>(ptr));
}
```

Typical output:

```text
count = 1
expired = false
after lock count = 2
count again = 1
~A()
expired after scope = true
lock succeeded = false
```

---

# Cyclic Ownership

## 16. Why can `shared_ptr` leak?

Consider:

```cpp
#include <memory>
#include <print>

struct B;

struct A
{
    std::shared_ptr<B> b;

    ~A()
    {
        std::println("~A()");
    }
};

struct B
{
    std::shared_ptr<A> a;

    ~B()
    {
        std::println("~B()");
    }
};

int main()
{
    {
        auto a = std::make_shared<A>();
        auto b = std::make_shared<B>();

        std::println("before cycle: A={}, B={}", a.use_count(), b.use_count());

        a->b = b;
        b->a = a;

        std::println("after cycle:  A={}, B={}", a.use_count(), b.use_count());
    }

    std::println("scope finished");
}
```

Typical output:

```text
before cycle: A=1, B=1
after cycle:  A=2, B=2
scope finished
```

Notice that:

```text
~A()
~B()
```

are never printed.

---

## 17. Why does the cycle leak?

After:

```cpp
a->b = b;
b->a = a;
```

ownership is:

```text
      owns
A ------------> B
^               |
|               |
+---------------+
      owns
```

Before leaving scope:

```text
A strong count = 2
B strong count = 2
```

Local `a` and `b` are destroyed:

```text
A: 2 -> 1
B: 2 -> 1
```

But:

```text
A owns B
B owns A
```

so neither count reaches zero.

The destructors never run.

---

## 18. Fix the cycle with `weak_ptr`

One direction should usually be non-owning:

```cpp
struct B;

struct A
{
    std::shared_ptr<B> b;
};

struct B
{
    std::weak_ptr<A> a;
};
```

Now:

```text
      owns
A ------------> B
^
|
| observes
|
B
```

Example:

```cpp
#include <memory>
#include <print>

struct B;

struct A
{
    std::shared_ptr<B> b;

    ~A()
    {
        std::println("~A()");
    }
};

struct B
{
    std::weak_ptr<A> a;

    ~B()
    {
        std::println("~B()");
    }
};

int main()
{
    {
        auto a = std::make_shared<A>();
        auto b = std::make_shared<B>();

        a->b = b;
        b->a = a;

        std::println("A count = {}", a.use_count());
        std::println("B count = {}", b.use_count());
    }
}
```

Typical output:

```text
A count = 1
B count = 2
~A()
~B()
```

---

# Storage Location

## 19. Where are smart pointers and managed objects stored?

Example:

```cpp
void Foo()
{
    Player value;

    auto unique = std::make_unique<Player>();
    auto shared = std::make_shared<Player>();
    std::weak_ptr<Player> weak = shared;
}
```

Conceptually:

```text
automatic storage / normally stack
+----------------------------------+
| value  : Player                  |
| unique : unique_ptr<Player> -----+----> dynamic Player
| shared : shared_ptr<Player> -----+----> control block + Player
| weak   : weak_ptr<Player> -------+----> same control block
+----------------------------------+
```

Important:

> The smart pointer object itself is stored wherever that smart pointer variable/member is stored.

For example:

```cpp
struct Game
{
    std::unique_ptr<Player> player;
};

auto game = std::make_unique<Game>();
```

Here:

```text
local unique_ptr<Game>       -> automatic storage
Game                         -> dynamic storage
Game::player unique_ptr      -> inside dynamic Game object
Player                       -> dynamic storage
```

Therefore it is incorrect to say:

> "`unique_ptr` always lives on the stack."

---

# Raw Pointer Comparison

## 20. Why is a raw pointer different?

```cpp
auto* p = new Player;
delete p;
```

After `delete`, `p` may still contain the old address.

```cpp
if (p)
{
    // does NOT prove that Player still exists
}
```

This is a dangling pointer.

`weak_ptr` avoids this problem because it checks lifetime through the shared control block.

---

# Recommended Ownership Order

Use the simplest ownership model that works:

```text
1. value
   Player player;

2. unique_ptr
   auto player = std::make_unique<Player>();

3. shared_ptr
   auto player = std::make_shared<Player>();

4. weak_ptr
   observe an object already managed by shared_ptr
```

Do not use `shared_ptr` simply because it looks "safer".

Shared ownership has:

- reference-count overhead
- more complicated lifetime
- possible cycles
- harder ownership reasoning

---

# Compact Mental Map

```text
Does this code need dynamic allocation?
        |
   +----+----+
   |         |
  NO        YES
   |         |
 value    Is there exactly one owner?
             |
        +----+----+
        |         |
       YES        NO
        |         |
 unique_ptr   truly shared lifetime?
                  |
             +----+----+
             |         |
            YES        rethink design
             |
        shared_ptr
             |
   non-owning observer needed?
             |
            YES
             |
         weak_ptr
```

---

# Core Points to Remember

- RAII ties resource lifetime to object lifetime.
- Destructors perform deterministic cleanup.
- Prefer the **Rule of Zero**.
- A local object usually has automatic storage duration.
- A local object's internals may still allocate dynamically.
- `unique_ptr` has one owner and no shared control block.
- `shared_ptr` and `weak_ptr` use the same control block.
- `shared_ptr` increments the strong ownership count.
- `weak_ptr` does not keep the object alive.
- `weak_ptr::lock()` safely tries to obtain a `shared_ptr`.
- `shared_ptr` cycles can prevent counts from reaching zero.
- Break ownership cycles using `weak_ptr`.
- Prefer values first, then `unique_ptr`, and use `shared_ptr` only for genuinely shared lifetime.
- Do not use `use_count()` as normal application logic; it is mainly useful for diagnostics and learning.

---

# Interview Questions

## 1. What does RAII mean?

RAII binds a resource to an object's lifetime. Construction acquires or initializes the resource, and destruction releases it automatically.

## 2. What is the main benefit of RAII?

Deterministic, exception-safe resource cleanup.

## 3. Is RAII only about memory?

No. It also applies to locks, files, sockets, handles, transactions, and other resources.

## 4. What is the Rule of Zero?

Prefer classes composed of RAII-aware members so that you do not need to manually implement destructor, copy, or move operations.

## 5. Is an implicit destructor really generated?

Yes. If valid, the compiler provides a destructor that destroys all members automatically in reverse declaration order.

## 6. Is a default constructor always generated?

No. A default constructor can be implicitly generated when no user-declared constructor prevents it and all members support the required construction.

## 7. Does `Player p;` always mean heap or stack?

A normal local `Player p;` has automatic storage duration and is normally stack-backed. Storage duration is the precise language-level concept.

## 8. Does `std::vector` inside a stack object keep all elements on the stack?

No. The vector object may be local while its element buffer normally uses dynamic storage.

## 9. What is the main difference between `unique_ptr` and `shared_ptr`?

`unique_ptr` models exclusive ownership. `shared_ptr` models shared ownership using reference counting.

## 10. Does `unique_ptr` use a control block?

No.

## 11. Which smart pointers share a control block?

`shared_ptr` and related `weak_ptr` instances referring to the same shared ownership state.

## 12. Why does `weak_ptr` not cause an ownership cycle?

Because it does not increment the strong ownership count.

## 13. How does `weak_ptr` know the object was destroyed?

The control block remains alive while weak references exist. `weak_ptr` can observe that the strong count reached zero.

## 14. What does `weak_ptr::lock()` do?

It atomically tries to create a `shared_ptr`. If the object still exists it succeeds; otherwise it returns an empty `shared_ptr`.

## 15. Why can two `shared_ptr` members create a memory leak?

If objects own each other, each keeps the other's strong count above zero, so neither destructor runs.

## 16. When should `shared_ptr` be used?

When multiple independent owners genuinely need to share responsibility for an object's lifetime.

## 17. What should normally be preferred over `shared_ptr`?

A value object first, then `unique_ptr` when dynamic exclusive ownership is required.

---

# Interview Answer in ~30 Seconds

> RAII means tying resource ownership to object lifetime. Resources are released automatically by destructors, including during exception unwinding. Modern C++ normally prefers value types and RAII members, following the Rule of Zero. For dynamic ownership, `unique_ptr` is the default exclusive-owner type and does not use reference counting. `shared_ptr` supports shared ownership through a control block containing ownership state, while `weak_ptr` observes the same control block without keeping the object alive. A `shared_ptr` cycle can prevent strong counts from reaching zero, so one direction is usually represented by `weak_ptr`.

---

# Small Integrated Study Example

```cpp
#include <memory>
#include <print>
#include <type_traits>

struct Player
{
    ~Player()
    {
        std::println("~Player()");
    }
};

struct B;

struct A
{
    std::shared_ptr<B> b;

    ~A()
    {
        std::println("~A()");
    }
};

struct B
{
    std::weak_ptr<A> a;

    ~B()
    {
        std::println("~B()");
    }
};

int main()
{
    static_assert(!std::is_copy_constructible_v<std::unique_ptr<Player>>);

    {
        auto player = std::make_unique<Player>();
        std::println("unique_ptr owns Player");
    }

    std::weak_ptr<Player> weak;

    {
        auto shared = std::make_shared<Player>();
        weak = shared;

        std::println("shared count = {}", shared.use_count());

        if (auto locked = weak.lock())
        {
            std::println("after lock count = {}", locked.use_count());
        }
    }

    std::println("expired = {}", weak.expired());

    {
        auto a = std::make_shared<A>();
        auto b = std::make_shared<B>();

        a->b = b;
        b->a = a;

        std::println("A strong count = {}", a.use_count());
        std::println("B strong count = {}", b.use_count());
    } // no cycle: weak_ptr does not own A
}
```
