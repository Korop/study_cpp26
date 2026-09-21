# C++23/26 `std::generator` and `co_yield` — Interview / Study Notes

> `std::generator<T>` is the standard C++23 synchronous lazy generator built on C++20 coroutines.  
> Think of it as the closest standard C++ analogue to C# `IEnumerable<T>` + `yield return`.

---

## 1. Version map

| Feature | Standard |
|---|---:|
| Coroutines: `co_await`, `co_yield`, `co_return` | C++20 |
| `std::generator` / `<generator>` | C++23 |
| Using `std::generator` in a C++26 project | Yes |

Important distinction:

```text
C++ language feature                    Standard library feature
--------------------                    ------------------------
co_yield / coroutine syntax             std::generator
compiler support                        standard library support
```

A compiler may understand `co_yield` while its standard library still does not provide `<generator>`.

---

## 2. C# `yield return` analogue

C#:

```csharp
IEnumerable<int> Numbers()
{
    for (int i = 0; i < 5; ++i)
    {
        yield return i;
    }
}
```

C++23:

```cpp
std::generator<int> Numbers()
{
    for (int i = 0; i < 5; ++i)
    {
        co_yield i;
    }
}
```

Mental mapping:

| C# | C++ |
|---|---|
| `IEnumerable<T>` | `std::generator<T>` |
| `yield return x` | `co_yield x` |
| `foreach` | range-based `for` |
| `MoveNext()` conceptually | iterator increment resumes coroutine |
| lazy sequence | lazy sequence |

---

## 3. `co_yield` is lazy

This:

```cpp
auto numbers = Numbers();
```

does **not** generate every number first.

Conceptually:

```text
consumer asks for value
        ↓
coroutine resumes
        ↓
co_yield value
        ↓
coroutine suspends
        ↓
consumer receives value
        ↓
consumer asks for next value
        ↓
coroutine resumes after previous co_yield
```

Example order:

```cpp
std::generator<int> Test()
{
    std::println("A");
    co_yield 10;

    std::println("B");
    co_yield 20;

    std::println("C");
}
```

Iteration prints:

```text
A
value=10
B
value=20
C
```

Not:

```text
A
B
C
value=10
value=20
```

---

## 4. A generator is not a container

This:

```cpp
std::generator<Player> players = LoadPlayers();
```

does **not** mean:

```text
players = [Player1, Player2, Player3, ...]
```

A generator mainly owns coroutine state needed to produce the next element.

For a large sequence:

```text
std::vector<T>       → usually stores all N values
std::generator<T>    → produces values on demand
```

So if the consumer stops early, later values are never generated.

This also makes infinite sequences possible:

```cpp
std::generator<int> Infinite()
{
    int value = 0;

    while (true)
    {
        co_yield value++;
    }
}
```

The consumer can safely stop after a few elements.

---

## 5. What happens at `co_yield`

Conceptually:

```cpp
co_yield value;
```

means roughly:

```text
1. expose value to the consumer
2. save coroutine execution state
3. suspend
4. return control to consumer
5. resume later when next element is requested
6. continue after co_yield
```

The actual language mechanism is based on the coroutine promise and `yield_value`.

---

## 6. Moving a `std::generator`

Consider:

```cpp
auto players = LoadPlayers();

auto filtered = FilterByScore(
    std::move(players),
    100
);
```

`std::move(players)` does **not** move all `Player` objects.

At this point most future players do not even exist yet.

It moves ownership of the generator/coroutine state.

Before:

```text
main::players
    │
    └── owns LoadPlayers coroutine
```

After:

```text
main::players
    │
    └── moved-from

filtered
    │
    └── FilterByScore coroutine
            │
            └── owns source generator
                    │
                    └── LoadPlayers coroutine
```

`std::generator` is move-only:

```cpp
generator(const generator&) = delete;
generator(generator&&);
```

The moved-from generator remains valid, but no longer owns the transferred coroutine.

---

## 7. Where are the `Player` objects during filtering?

Source:

```cpp
std::generator<Player> LoadPlayers()
{
    co_yield Player{1, "Ivan", 120};
    co_yield Player{2, "Anna", 80};
    co_yield Player{3, "John", 150};
}
```

Filter:

```cpp
for (auto&& player : players)
{
    if (player.score >= minScore)
    {
        co_yield ...;
    }
}
```

The sequence is processed one element at a time:

```text
Ivan
  ↓
filter passes
  ↓
consumer

Anna
  ↓
filter rejects
  ↓
not stored anywhere
  ↓
source resumes

John
  ↓
filter passes
  ↓
consumer
```

There is no hidden:

```text
filtered = [Ivan, John]
```

container.

Rejected values simply stop being relevant when the source generator advances.

---

## 8. Why `auto&& player` is important

For:

```cpp
std::generator<Player>
```

iteration gives an effective reference type of:

```cpp
Player&&
```

so:

```cpp
for (auto&& player : players)
```

gives:

```cpp
decltype(player) == Player&&
```

But this is the classic rvalue-reference rule:

```cpp
Player&& player = ...;
```

The variable type is:

```cpp
Player&&
```

while the expression:

```cpp
player
```

is an **lvalue**, because every named variable expression is an lvalue.

This distinction matters for `co_yield`.

---

## 9. `co_yield player` vs `co_yield std::move(player)`

For:

```cpp
std::generator<Player>
```

this:

```cpp
co_yield player;
```

passes an lvalue expression.

`std::generator` can handle it, but the lvalue `yield_value` path creates temporary stored `Player` state, which can require a copy.

Conceptually:

```text
source Player
     ↓
auto&& player
     ↓
co_yield player
     ↓
extra stored Player
     ↓
consumer
```

For this concrete transient-value pipeline:

```cpp
co_yield std::move(player);
```

is preferable.

`std::move(player)` converts the named lvalue expression back to an xvalue.

The direct rvalue-reference `yield_value` path can expose the already-existing yielded object without that lvalue-copy path.

Conceptually:

```text
source Player
     ↑
     │
auto&& player
     │
co_yield std::move(player)
     │
     └──────────────→ consumer
```

Important:

> `std::move` itself does not move an object. It is a cast to an xvalue.  
> An actual move happens only if some receiving operation move-constructs or move-assigns from it.

---

## 10. Why the reference is safe while yielded

Suppose the source is suspended here:

```cpp
co_yield Player{1, "Ivan", 120};
```

The current `Player` remains alive while that source coroutine is suspended.

Then the filter suspends here:

```cpp
co_yield std::move(player);
```

So during consumer access:

```text
LoadPlayers suspended
    │
    └── current Player alive
            ▲
            │
FilterByScore suspended
            ▲
            │
consumer uses Player
```

Only when the consumer requests the next value does the filter resume and eventually advance the source generator.

That lifetime relationship is what makes this pipeline work.

---

## 11. `std::move` vs `std::forward`

For the concrete case:

```cpp
std::generator<Player>
```

where you intentionally want the current transient `Player` treated as an rvalue:

```cpp
co_yield std::move(player);
```

is clearer.

This would also produce an rvalue here:

```cpp
co_yield std::forward<decltype(player)>(player);
```

because:

```cpp
decltype(player) == Player&&
```

But `std::forward` communicates a different intent:

```text
std::move(x)
    → force x to be treated as an rvalue

std::forward<T>(x)
    → preserve the original value category represented by T
```

Use `std::forward` mainly in generic forwarding code where the source may produce `T&`, `const T&`, or `T&&`.

---

## 12. Do not blindly `std::move` every ranged value

This is the important counterexample.

Suppose a generator exposes objects owned by an external `std::vector`:

```cpp
std::generator<Player&> PlayersFromVector(std::vector<Player>& players)
{
    for (auto& player : players)
    {
        co_yield player;
    }
}
```

Now each yielded object is owned by the vector:

```text
vector
  │
  ├── Player Ivan  ← generator yields reference
  ├── Player Anna
  └── Player John
```

The generator does **not** own those `Player`s.

For a reference-preserving filter:

```cpp
std::generator<Player&> FilterVectorPlayers(
    std::generator<Player&> players,
    int minScore)
{
    for (auto&& player : players)
    {
        if (player.score >= minScore)
        {
            co_yield std::forward<decltype(player)>(player);
        }
    }
}
```

Here:

```cpp
decltype(player) == Player&
```

and:

```cpp
std::forward<decltype(player)>(player)
```

preserves `Player&`.

Do **not** adopt this rule:

```cpp
for (auto&& item : range)
{
    co_yield std::move(item); // NOT a universal optimization
}
```

If the item refers to persistent external storage, turning it into an rvalue can expose that external object as movable-from state in a value-oriented pipeline. A downstream consumer may then move resources out of the original object.

The correct choice depends on ownership and semantics, not on a desire to sprinkle `std::move` until the profiler becomes afraid.

---

## 13. Concrete vs generic rule

### Concrete transient-value generator

```cpp
std::generator<Player>
```

Use:

```cpp
co_yield std::move(player);
```

when intentionally passing the current transient yielded value onward as an rvalue.

### Reference-preserving / generic pipeline

Use:

```cpp
co_yield std::forward<decltype(player)>(player);
```

when you must preserve whether the source gave you:

```text
T&
const T&
T&&
```

Rule:

| Situation | Prefer |
|---|---|
| Explicitly want rvalue semantics | `std::move(x)` |
| Generic perfect-forwarding semantics | `std::forward<T>(x)` |
| `auto&&` and preserve source category | `std::forward<decltype(x)>(x)` |
| External persistent object should remain an lvalue | do not force `std::move` |

---

## 14. Memory and execution model

For a generator pipeline:

```text
LoadPlayers()
    ↓
FilterByScore()
    ↓
consumer
```

values flow on demand.

If the consumer needs only `K` values:

```text
work ≈ enough source iterations to produce those K values
```

The entire source sequence is not materialized first.

Typical additional storage is coroutine state plus the currently active yielded values, rather than an `O(N)` container containing the whole sequence.

Coroutine frames may require dynamic storage, though implementations can optimize allocation in some cases.

---

## 15. `std::generator` is synchronous

`std::generator` is a **synchronous** coroutine-based range.

It does not mean:

```text
background thread
parallel execution
asynchronous I/O
Task<T>
future<T>
```

It is closer to:

```text
lazy pull-based iteration
```

The caller requests the next item; the coroutine resumes until it yields one.

---

## 16. `std::generator` is an input range

`std::generator` models:

```text
std::ranges::view
std::ranges::input_range
```

Think **single-pass consumption**, not a reusable random-access container.

It is fundamentally different from:

```text
std::vector
std::array
std::span
```

which represent already-existing collections/storage.

---

## 17. Early termination

Generators are especially useful when the consumer may stop early.

Example:

```cpp
int found = 0;

for (auto&& player : FilterByScore(LoadPlayers(), 100))
{
    std::println("{}", player.name);

    if (++found == 2)
    {
        break;
    }
}
```

Once the consumer breaks, later values do not need to be generated.

This is one of the main advantages over first constructing a complete result vector.

---

## 18. Complete study example

This example covers:

- lazy `std::generator`
- C# `yield return` analogue
- moving generator ownership
- `auto&&`
- one `static_assert` block
- `co_yield std::move(...)`
- reference-preserving generator over `std::vector`
- `std::forward`
- why `std::move` is not universal
- `std::println`

```cpp
#include <generator>
#include <print>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

struct Player
{
    int id;
    std::string name;
    int score;
};

// std::generator<Player> is lazy.
// No vector of Players is created here.
std::generator<Player> LoadPlayers()
{
    std::println("LoadPlayers: start");

    co_yield Player{1, "Ivan", 120};
    co_yield Player{2, "Anna", 80};
    co_yield Player{3, "John", 150};

    std::println("LoadPlayers: end");
}

// Concrete value pipeline.
// The parameter owns the generator moved into this coroutine.
std::generator<Player> FilterByScore(
    std::generator<Player> players,
    int minScore)
{
    for (auto&& player : players)
    {
        static_assert(
            std::is_same_v<decltype(player), Player&&> &&
            std::is_lvalue_reference_v<decltype((player))>
        );

        if (player.score >= minScore)
        {
            // `player` has type Player&&, but the named expression is an lvalue.
            //
            // co_yield player;
            //     -> lvalue yield path; may create an extra Player copy.
            //
            // std::forward<decltype(player)>(player) also gives Player&& here,
            // but std::move expresses our concrete intent more directly.
            co_yield std::move(player);
        }
    }
}

// These Players are owned by an external vector.
// The generator only exposes references to them.
std::generator<Player&> PlayersFromVector(std::vector<Player>& players)
{
    for (auto& player : players)
    {
        co_yield player;
    }
}

// Preserve the reference category from the source generator.
std::generator<Player&> FilterVectorPlayers(
    std::generator<Player&> players,
    int minScore)
{
    for (auto&& player : players)
    {
        if (player.score >= minScore)
        {
            // Here decltype(player) is Player&.
            // Preserve it instead of forcing rvalue semantics.
            co_yield std::forward<decltype(player)>(player);
        }
    }
}

int main()
{
    std::println("--- transient generator<Player> ---");

    auto players = LoadPlayers();

    // Moves generator/coroutine ownership.
    // It does NOT move all future Player objects.
    auto filtered = FilterByScore(std::move(players), 100);

    for (auto&& player : filtered)
    {
        std::println("{}: {}", player.name, player.score);
    }

    std::println("--- external vector via generator<Player&> ---");

    std::vector<Player> storedPlayers{
        {10, "Bob", 200},
        {11, "Kate", 50},
        {12, "Mike", 180}
    };

    auto referenced = PlayersFromVector(storedPlayers);
    auto referencedFiltered =
        FilterVectorPlayers(std::move(referenced), 100);

    for (auto&& player : referencedFiltered)
    {
        std::println("{}: {}", player.name, player.score);
    }

    // Original vector objects are still the same objects.
    for (const auto& player : storedPlayers)
    {
        std::println("stored: {}: {}", player.name, player.score);
    }
}
```

Expected logical output:

```text
--- transient generator<Player> ---
LoadPlayers: start
Ivan: 120
John: 150
LoadPlayers: end
--- external vector via generator<Player&> ---
Bob: 200
Mike: 180
stored: Bob: 200
stored: Kate: 50
stored: Mike: 180
```

---

## 19. Toolchain support

A useful interview distinction:

```text
compiler frontend != standard library
```

`co_yield` is a language feature.

`std::generator` is a standard-library feature.

As of **2026-09-21**:

```text
GCC 16 + libstdc++              → std::generator available
Clang + current LLVM libc++     → P2502R2 std::generator still not implemented
Apple Clang + Apple libc++      → <generator> unavailable in the setup discussed
Clang + sufficiently new libstdc++ → possible
```

So this failure:

```text
fatal error: 'generator' file not found
```

can happen even with:

```text
-std=c++26
```

because `-std=c++26` selects the language mode; it does not add a missing library header.

Clang can use either libc++ or libstdc++, but mixing a Homebrew GCC library into an Apple-Clang macOS toolchain adds include/linker configuration complexity.

For this study project, using GCC 16 + libstdc++ for `std::generator` is the clean option.

---

## 20. Common interview traps

### Trap 1: "`std::move` moves the object"

Not exactly.

```cpp
std::move(x)
```

is essentially a cast producing an xvalue.

A later operation may move from it.

---

### Trap 2: "`Player&& player` means expression `player` is an rvalue"

False.

```cpp
Player&& player = ...;
```

Type:

```text
Player&&
```

Expression:

```cpp
player
```

is an lvalue because it is named.

---

### Trap 3: "`std::move(players)` moves all generated players"

False.

It moves generator ownership/coroutine state.

The future sequence is still lazy.

---

### Trap 4: "`std::generator` stores all results"

False.

It produces them on demand.

---

### Trap 5: "`co_yield std::move(x)` is always faster"

False.

It depends on ownership, yielded/reference types, and downstream semantics.

---

### Trap 6: "`std::generator` is async"

False.

It is a synchronous lazy range.

---

### Trap 7: "`auto&&` always means rvalue reference"

False.

Reference collapsing applies.

For example:

```text
source yields Player&& → auto&& becomes Player&&
source yields Player&  → auto&& becomes Player&
```

---

## 21. `std::move` vs `std::forward` mental model

Remember:

```text
std::move(x)
    ↓
"I no longer need to preserve x's original value category.
 Treat it as an rvalue."

std::forward<T>(x)
    ↓
"Preserve the value category represented by T."
```

For our concrete value generator:

```cpp
co_yield std::move(player);
```

For a generic/reference-preserving pipeline:

```cpp
co_yield std::forward<decltype(player)>(player);
```

---

## 22. Generator vs vector

| Property | `std::generator<T>` | `std::vector<T>` |
|---|---|---|
| Values created lazily | Yes | No |
| Stores full sequence | No | Yes |
| Can represent infinite sequence | Yes | No |
| Random access | No | Yes |
| Multi-pass container | No | Yes |
| External storage required | No | Yes, vector owns elements |
| Can stop generation early | Yes | Values usually already constructed |
| Main use | streaming/lazy transformation | owned materialized collection |

Use a generator when lazy pull-based production is useful.

Use a vector when you need ownership of a complete materialized collection, random access, repeated traversal, sorting, indexing, etc.

---

# Interview Questions and Answers

## 1. What is `std::generator`?

A C++23 synchronous coroutine-based lazy range that produces elements one at a time.

---

## 2. Which header defines it?

```cpp
#include <generator>
```

---

## 3. Which C++ version introduced `std::generator`?

C++23.

---

## 4. Which version introduced `co_yield`?

Coroutines, including `co_yield`, were standardized in C++20.

---

## 5. What is the C# analogue?

Roughly:

```text
C# IEnumerable<T> + yield return
        ↕
C++ std::generator<T> + co_yield
```

---

## 6. Is `std::generator` eager or lazy?

Lazy.

Execution resumes only when the consumer requests the next element.

---

## 7. Does creating a generator execute the complete function?

No.

The coroutine is suspended and resumed incrementally.

---

## 8. What does `co_yield` do?

It provides a value to the consumer and suspends the coroutine.

---

## 9. Where does execution resume?

Immediately after the previous `co_yield`.

---

## 10. Does a generator contain all generated values?

No.

It owns coroutine state, not a materialized sequence of all future values.

---

## 11. Why are generators useful for large sequences?

They avoid materializing all elements when only some may be consumed.

---

## 12. Can a generator represent an infinite sequence?

Yes, provided the consumer eventually stops iteration when appropriate.

---

## 13. What happens when the consumer breaks early?

The remaining sequence is not generated.

---

## 14. Is `std::generator` asynchronous?

No.

It is synchronous and pull-based.

---

## 15. Is it a range?

Yes. `std::generator` models `std::ranges::view` and `std::ranges::input_range`.

---

## 16. Is it multi-pass?

Treat it as a single-pass input range, not a reusable container.

---

## 17. Can a `std::generator` be copied?

No.

Its copy constructor is deleted.

---

## 18. Can it be moved?

Yes.

Moving transfers ownership of the underlying coroutine state.

---

## 19. What does `std::move(players)` move in our filter example?

The generator ownership/coroutine handle/state, not a collection of `Player` values.

---

## 20. Are all `Player`s already created when the generator is moved?

No.

Future players are generated lazily.

---

## 21. What happens to the old generator after the move?

It is valid but moved-from and no longer owns the transferred coroutine.

---

## 22. Where does a rejected filtered value go?

It is simply not yielded by the filter. When the source advances, that current yielded value is no longer needed.

---

## 23. What is `decltype(player)` for `auto&& player` iterating `std::generator<Player>`?

For this generator:

```cpp
Player&&
```

---

## 24. Is the expression `player` itself an rvalue?

No.

A named variable expression is an lvalue, even when the variable type is `Player&&`.

---

## 25. Why can `co_yield player` copy?

Because `player` is an lvalue expression, so `std::generator<Player>` can use its lvalue-yield path that stores a `Player` object.

---

## 26. Why use `co_yield std::move(player)` in this concrete pipeline?

It restores rvalue/xvalue semantics and lets the generator use the direct rvalue-reference yield path rather than the lvalue-copy path.

---

## 27. Does `std::move(player)` itself move the `Player`?

No.

It casts the expression to an xvalue.

---

## 28. When does an actual move happen?

When another operation constructs or assigns an object from that rvalue and selects a move operation.

---

## 29. Could `std::forward<decltype(player)>(player)` work in the same `std::generator<Player>` case?

Yes.

Because `decltype(player)` is `Player&&`, forwarding produces an rvalue.

---

## 30. Why prefer `std::move` there?

The code is concrete, and the intention is explicitly to treat the transient current `Player` as an rvalue.

---

## 31. When is `std::forward` preferable?

In generic/reference-preserving code where the incoming value may be `T&`, `const T&`, or `T&&`.

---

## 32. What does `std::forward<decltype(x)>(x)` do with `x` being `T&`?

It preserves an lvalue.

---

## 33. What does it do when `decltype(x)` is `T&&`?

It produces an rvalue/xvalue expression.

---

## 34. Why not blindly call `std::move` for items from a vector-backed range?

Those items may be persistent objects owned by the vector. Forcing rvalue semantics can allow downstream code to move resources out of the original elements.

---

## 35. How should a generator expose vector-owned elements when ownership must stay in the vector?

Use reference semantics, for example:

```cpp
std::generator<Player&>
```

and preserve those references.

---

## 36. Is `auto&&` always an rvalue reference?

No.

Reference collapsing can make it an lvalue reference.

---

## 37. What is the key ownership difference between `std::generator<Player>` and `std::generator<Player&>`?

`std::generator<Player>` is value-oriented and commonly produces transient values.

`std::generator<Player&>` exposes references to objects whose lifetime is managed elsewhere.

---

## 38. Why can Clang understand `co_yield` but fail on `#include <generator>`?

Because coroutine syntax is compiler-language support, while `<generator>` is provided by the C++ standard library.

---

## 39. Does `-std=c++26` guarantee every C++23/26 standard-library facility exists?

No.

The selected standard mode and the actual library implementation/support are separate concerns.

---

## 40. Give a strong 30-second interview answer

> `std::generator<T>` is a C++23 synchronous lazy input range implemented with C++20 coroutines. `co_yield` exposes one element and suspends the coroutine; iteration resumes it to obtain the next element. The sequence is not materialized like a vector, so generators are useful for streaming, large, filtered, or even infinite sequences and for early termination. `std::generator` is move-only because it owns coroutine state. In pipelines, `std::move(generator)` transfers generator ownership, while `std::move(element)` changes the element expression to an rvalue. I use `std::forward` instead when generic code must preserve the source reference category.

---

# Compact cheat sheet

```text
C++20:
    co_yield / coroutines

C++23:
    std::generator<T>

std::generator<T>:
    lazy
    synchronous
    view
    input_range
    move-only

co_yield x:
    expose one value
    suspend coroutine
    resume later

std::move(generator):
    transfer coroutine ownership

std::move(player):
    force rvalue/xvalue semantics
    does not itself perform a move

std::forward<decltype(player)>(player):
    preserve source reference category

generator<Player>:
    value-oriented transient pipeline
    co_yield std::move(player) can avoid lvalue-copy path

generator<Player&>:
    reference-oriented
    object usually owned elsewhere
    preserve reference semantics

Never:
    blindly std::move every item from every range
```

---

# References

- `std::generator`: https://en.cppreference.com/w/cpp/coroutine/generator
- `std::generator::promise_type::yield_value`: https://en.cppreference.com/w/cpp/coroutine/generator/promise_type/yield_value
- C++ coroutines / `co_yield`: https://en.cppreference.com/w/cpp/language/coroutines
- `std::move`: https://en.cppreference.com/w/cpp/utility/move
- libc++ C++23 implementation status: https://libcxx.llvm.org/Status/Cxx23.html
- Clang toolchain / libc++ vs libstdc++: https://clang.llvm.org/docs/Toolchain.html
- GCC 16 libstdc++ `<generator>`: https://gcc.gnu.org/onlinedocs/gcc-16.1.0/libstdc++/api/a00119_source.html
