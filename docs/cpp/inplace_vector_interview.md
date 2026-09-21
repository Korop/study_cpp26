# C++26 Interview Study: `std::inplace_vector`

## 1. Mental model

```cpp
std::inplace_vector<T, N>
```

A contiguous sequence container with:

- runtime `size()`
- compile-time fixed `capacity() == N`
- element storage inside the container object
- no dynamic allocation by the container for its element storage
- vector-like API

Think:

```text
std::array<T, N>
    fixed size
    fixed capacity
    inline storage

std::inplace_vector<T, N>
    dynamic size: 0..N
    fixed capacity: N
    inline storage

std::vector<T>
    dynamic size
    dynamic capacity
    external dynamically allocated storage
```

Header:

```cpp
#include <inplace_vector>
```

---

## 2. Basic example

```cpp
std::inplace_vector<int, 4> values;

values.push_back(10);
values.push_back(20);
values.emplace_back(30);

std::println("size={}, capacity={}", values.size(), values.capacity());
```

Result:

```text
size=3, capacity=4
```

`capacity()` is always `N`.

---

## 3. Storage and lifetime

`std::inplace_vector<T, N>` contains storage for up to `N` elements, but only `size()` objects are alive.

```cpp
std::inplace_vector<int, 8> values;
```

Conceptually:

```text
size = 0
capacity = 8

[ raw ][ raw ][ raw ][ raw ][ raw ][ raw ][ raw ][ raw ]
```

After:

```cpp
values.push_back(10);
values.push_back(20);
```

```text
[ 10 ][ 20 ][ raw ][ raw ][ raw ][ raw ][ raw ][ raw ]
  alive alive
```

Unlike:

```cpp
std::array<int, 8>
```

where all 8 `int` objects exist immediately.

---

## 4. In-place does not mean stack

The storage is inside the `inplace_vector` object.

Local object:

```cpp
void Foo()
{
    std::inplace_vector<int, 100> values;
}
```

Usually the whole object is in the stack frame.

But:

```cpp
auto* values = new std::inplace_vector<int, 100>;
```

puts the whole object on the heap.

Correct statement:

> Elements are stored inside the `inplace_vector` object.

Not:

> `inplace_vector` always stores elements on the stack.

---

## 5. Main operations

```cpp
values.size();
values.capacity();
values.empty();

values[0];
values.at(0);
values.front();
values.back();
values.data();

values.begin();
values.end();

values.push_back(value);
values.emplace_back(args...);
values.pop_back();

values.insert(pos, value);
values.erase(pos);
values.resize(n);
values.clear();
```

Storage is contiguous, so:

```cpp
T* ptr = values.data();
```

behaves like a pointer to an array of the active elements.

---

## 6. `push_back`, `try_push_back`, `unchecked_push_back`

### `push_back`

```cpp
auto& value = values.push_back(42);
```

- O(1)
- returns `T&`
- throws `std::bad_alloc` if full

```cpp
std::inplace_vector<int, 2> values{1, 2};

values.push_back(3); // throws std::bad_alloc
```

No allocation actually has to fail. The exception represents capacity exhaustion.

---

### `try_push_back`

Current C++26 interface:

```cpp
std::optional<T&>
```

Example:

```cpp
auto result = values.try_push_back(42);

if (result)
{
    std::println("Inserted {}", *result);
}
else
{
    std::println("Container is full");
}
```

- O(1)
- returns `std::nullopt` if full
- does not throw merely because capacity is exhausted

Older articles/implementations may show `T*`. The final C++26 interface uses `std::optional<T&>`.

---

### `unchecked_push_back`

```cpp
values.unchecked_push_back(42);
```

Precondition:

```cpp
values.size() < values.capacity()
```

Use only when the algorithm already guarantees available capacity.

```cpp
if (values.size() < values.capacity())
{
    values.unchecked_push_back(42);
}
```

Violating the precondition is a programming error.

---

## 7. `emplace_back` variants

Same idea exists for in-place construction:

```cpp
values.emplace_back(args...);
values.try_emplace_back(args...);
values.unchecked_emplace_back(args...);
```

`emplace_back()` and successful `try_emplace_back()` construct the element directly in the internal storage.

---

## 8. Complexity

| Operation | Complexity |
|---|---:|
| `size()` | O(1) |
| `capacity()` | O(1) |
| `empty()` | O(1) |
| `operator[]`, `at()` | O(1) |
| `front()`, `back()` | O(1) |
| `push_back()` | **O(1) worst case** |
| `emplace_back()` | **O(1) worst case** |
| `try_push_back()` | O(1) |
| `pop_back()` | O(1) |
| insert/erase at end | O(1) per element |
| insert/erase in middle | O(n) |
| `clear()` | O(size) for non-trivial `T` |
| copy construction | O(size) |
| move construction | **O(size)** |

Important difference:

```text
std::vector::push_back()
    amortized O(1)

std::inplace_vector::push_back()
    worst-case O(1)
```

`inplace_vector` never reallocates its storage.

---

## 9. Why middle insertion is O(n)

```text
Before:
[A][B][C][D][ ]

Insert X at index 1:

[A][X][B][C][D]
```

`B`, `C`, and `D` must move.

Therefore insertion/erase near the beginning or middle is linear in the number of affected elements.

---

## 10. Space complexity

For:

```cpp
std::inplace_vector<T, N>
```

the object contains storage sufficient for `N` elements.

Approximate storage:

```text
N * sizeof(T)
+ size metadata
+ alignment/padding
```

So:

```text
space = O(N)
```

even when:

```cpp
values.size() == 0
```

This is the major memory tradeoff.

---

## 11. Large `N` problem

Avoid blindly doing:

```cpp
void Process()
{
    std::inplace_vector<double, 1'000'000> values;
}
```

The object needs roughly:

```text
1'000'000 * 8 bytes ~= 8 MB
```

for element storage alone.

If local, this may overflow the thread stack.

Use `std::vector` when the capacity is large or naturally unbounded.

---

## 12. `T` may still allocate

This:

```cpp
std::inplace_vector<std::string, 32> names;
```

means the container does not dynamically allocate storage for the 32 `std::string` objects.

But each `std::string` may allocate its own character buffer.

Therefore:

> `inplace_vector` itself does not allocate storage for its elements.

It does **not** mean:

> Using `inplace_vector` guarantees that no heap allocation occurs anywhere.

---

## 13. Move is O(size)

This is a major interview point.

A `std::vector` can often move by stealing its external buffer:

```text
vector A                  heap storage
[ptr][size][capacity] ---> [ elements ]

vector B after move
[ptr][size][capacity] ---> [ same elements ]
```

This can be O(1).

An `inplace_vector` owns its storage physically:

```text
A object
[A][B][C][D]

B object
[ ][ ][ ][ ]
```

Moving `A` into `B` requires moving the elements:

```text
A -> B
A
B
C
D
```

Therefore:

```text
move construction = O(size)
```

Do not assume "move is always cheap".

---

## 14. Iterator and reference stability

Appending does not reallocate.

```cpp
std::inplace_vector<int, 8> values{10};

int* ptr = &values[0];

values.push_back(20);
values.push_back(30);

std::println("{}", *ptr); // 10
```

Existing element references/pointers remain valid after successful end insertion.

The old `end()` iterator changes.

Middle insertion/erase may invalidate iterators/references at and after the affected position because elements move.

Moving or swapping an `inplace_vector` invalidates iterators.

---

## 15. `reserve()` and `shrink_to_fit()`

Capacity is fixed at compile time.

```cpp
std::inplace_vector<int, 8> values;

values.reserve(4); // does nothing
values.reserve(8); // does nothing
values.reserve(9); // throws std::bad_alloc
```

`reserve()` exists mainly for vector-like interface compatibility.

```cpp
values.shrink_to_fit();
```

also cannot reduce the embedded storage.

---

## 16. Comparison

| Property | `std::array<T,N>` | `std::inplace_vector<T,N>` | `std::vector<T>` |
|---|---:|---:|---:|
| Size | fixed N | 0..N | runtime |
| Capacity | fixed N | fixed N | runtime |
| Storage inside object | yes | yes | normally no |
| Container heap allocation | no | no | normally yes |
| Contiguous | yes | yes | yes |
| `push_back()` | no | O(1) | amortized O(1) |
| Can exceed N | no | no | yes |
| Constructs all N initially | yes | no | no |
| Move container | O(N) normally | O(size) | often O(1) |
| Empty object storage | O(N) | O(N) | small |

---

## 17. `inplace_vector` vs `small_vector`

`std::inplace_vector` is **not** a small-buffer-optimized vector.

Typical non-standard `small_vector<T, N>`:

```text
size <= N
    use internal storage

size > N
    allocate external storage
```

`std::inplace_vector<T, N>`:

```text
size <= N
    use internal storage

size > N
    impossible
```

It never spills into dynamically allocated storage.

---

## 18. Good use cases

Use `std::inplace_vector` when the domain has a real small upper bound.

Examples:

```cpp
struct Packet
{
    std::inplace_vector<Event, 32> events;
};
```

```cpp
struct Entity
{
    std::inplace_vector<Effect, 16> activeEffects;
};
```

```cpp
struct SensorManager
{
    std::inplace_vector<Sensor, 8> sensors;
};
```

Good for:

- embedded systems
- games
- networking packets
- low-latency code
- bounded protocol fields
- small collections inside frequently used objects
- environments where dynamic allocation is undesirable

Benefits:

- predictable capacity
- predictable memory footprint
- no container reallocation
- contiguous/cache-friendly storage
- worst-case O(1) insertion at the end

---

## 19. When not to use it

Prefer `std::vector` when:

- maximum size is unknown
- capacity may be large
- arbitrary growth is required
- large containers are moved frequently
- reserving `N` elements inside every object wastes too much memory

Suspicious code:

```cpp
std::inplace_vector<User, 10000> users;
```

Ask:

> Why exactly 10,000?

If the answer is "probably enough", the type likely encodes an arbitrary operational guess rather than a real domain invariant.

---

## 20. Compact C++26 example

```cpp
#include <cstddef>
#include <inplace_vector>
#include <optional>
#include <print>
#include <string>
#include <type_traits>
#include <utility>

struct Effect
{
    int id;
    float duration;
};

int main()
{
    using Effects = std::inplace_vector<Effect, 3>;

    static_assert(
        std::is_same_v<Effects::value_type, Effect> &&
        Effects::capacity() == 3 &&
        Effects::max_size() == 3
    );

    Effects effects;

    effects.emplace_back(1, 2.5F);
    effects.push_back({2, 5.0F});

    if (auto inserted = effects.try_emplace_back(3, 1.0F))
    {
        std::println(
            "Inserted effect id={}, size={}, capacity={}",
            inserted->id,
            effects.size(),
            effects.capacity()
        );
    }

    if (!effects.try_push_back({4, 10.0F}))
    {
        std::println("Full: max {} effects", effects.capacity());
    }

    for (const auto& effect : effects)
    {
        std::println(
            "Effect id={}, duration={}",
            effect.id,
            effect.duration
        );
    }

    Effects moved = std::move(effects); // O(size), elements must move.

    std::println(
        "Moved size={}, fixed capacity={}",
        moved.size(),
        moved.capacity()
    );
}
```

---

# Interview Questions

## 1. What is `std::inplace_vector`?

A C++26 contiguous sequence container with runtime-variable size and compile-time fixed capacity. Its element storage is embedded inside the container object.

---

## 2. What is its main declaration form?

```cpp
std::inplace_vector<T, N>
```

`T` is the element type and `N` is the maximum capacity.

---

## 3. Can its size change at runtime?

Yes.

```text
0 <= size() <= N
```

---

## 4. Can its capacity change at runtime?

No.

```cpp
capacity() == N
```

for the lifetime of the type.

---

## 5. Does `inplace_vector` dynamically allocate element storage?

No. Its element storage is embedded inside the container object.

The element type itself may still perform allocations.

---

## 6. Does `inplace_vector` always use stack memory?

No.

Its storage lives wherever the container object lives.

---

## 7. Are all `N` elements constructed when the container is created?

No.

Only the first `size()` elements are alive.

This differs from `std::array<T, N>`.

---

## 8. Are elements contiguous?

Yes.

`data()` returns a pointer to the first active element.

---

## 9. What is the complexity of random access?

O(1).

---

## 10. What is the complexity of `push_back()`?

Worst-case O(1).

There is no reallocation.

---

## 11. How does that differ from `std::vector::push_back()`?

`std::vector::push_back()` is amortized O(1) because a particular insertion may require O(n) reallocation and element movement.

---

## 12. What happens when `push_back()` exceeds capacity?

It throws `std::bad_alloc`.

---

## 13. Why `std::bad_alloc` if no allocation occurs?

Capacity exhaustion is represented through the vector-like failure interface. It does not imply that an allocation was actually attempted.

---

## 14. What does `try_push_back()` return in current C++26?

```cpp
std::optional<T&>
```

Success gives a reference to the inserted element; full capacity gives `std::nullopt`.

---

## 15. What is `unchecked_push_back()` for?

For code that already guarantees:

```cpp
size() < capacity()
```

It avoids the capacity check. Violating its precondition is a programming error.

---

## 16. What is the complexity of middle insertion?

O(n), because elements after the insertion point must move.

---

## 17. What is the space complexity?

O(N), regardless of the current number of active elements.

---

## 18. What is the main memory disadvantage?

Every `inplace_vector<T, N>` object carries storage for up to `N` elements even when empty.

---

## 19. Why can a large local `inplace_vector` be dangerous?

Its embedded storage may make the stack frame huge and cause stack overflow.

---

## 20. Is moving an `inplace_vector` O(1)?

No.

Normally it is O(size) because the destination has separate embedded storage and elements must be moved.

---

## 21. Why can moving `std::vector` be cheaper?

A vector can often transfer ownership of its external allocated buffer rather than moving every element.

---

## 22. Does appending invalidate existing element pointers?

Normally no.

There is no reallocation. The previous `end()` iterator is invalidated.

---

## 23. Can middle insertion invalidate references?

Yes.

Elements at or after the insertion position may be moved.

---

## 24. What happens to iterators when an `inplace_vector` is moved?

They are invalidated because elements move into the destination object's storage.

---

## 25. What does `reserve()` do?

For:

```cpp
std::inplace_vector<T, N>
```

`reserve(x)`:

- does nothing if `x <= N`
- throws `std::bad_alloc` if `x > N`

It cannot grow the internal storage.

---

## 26. Does `shrink_to_fit()` reduce object size?

No.

The embedded capacity is part of the object's type and layout.

---

## 27. `std::array` vs `std::inplace_vector`?

`std::array<T, N>` always has exactly `N` live elements.

`std::inplace_vector<T, N>` has between 0 and `N` live elements.

Both have fixed-capacity inline storage.

---

## 28. `std::vector` vs `std::inplace_vector`?

`std::vector` has dynamically managed capacity and can grow.

`std::inplace_vector` has fixed compile-time capacity and stores elements inside itself.

---

## 29. Is `inplace_vector` equivalent to `small_vector`?

No.

A typical `small_vector` may spill to heap storage after exceeding its inline capacity.

`inplace_vector` never exceeds `N`.

---

## 30. Can `N` be zero?

Yes.

```cpp
std::inplace_vector<int, 0>
```

is valid and always empty.

---

## 31. Why is `inplace_vector` useful in embedded systems?

It gives bounded memory usage, no container allocation, contiguous storage, and predictable end-insertion complexity.

---

## 32. Why is it useful in games?

Many game-domain collections have small natural limits, such as active effects, nearby objects, actions, or fixed protocol data.

It avoids many small heap allocations while retaining vector-like behavior.

---

## 33. What is a good criterion for choosing `N`?

`N` should usually come from a real domain constraint, protocol limit, or algorithmic invariant.

Not from "this number is probably large enough".

---

## 34. Can `std::inplace_vector<std::string, N>` still allocate?

Yes.

The vector does not allocate storage for the `std::string` objects, but individual strings may allocate their character buffers.

---

## 35. Should a large `inplace_vector` be passed by value?

Usually not unless copying/moving is intentional.

Prefer a reference or `std::span` for read-only/non-owning processing.

---

## 36. Why can `std::span` be useful with `inplace_vector`?

Because `inplace_vector` is contiguous.

A function accepting:

```cpp
std::span<const T>
```

can work with `inplace_vector`, `vector`, `array`, and C arrays.

---

## 37. Is `push_back()` exception-free?

No.

It may throw:

- `std::bad_alloc` when full
- exceptions from constructing/copying/moving `T`

Use `try_push_back()` when capacity exhaustion should be handled without an exception.

---

## 38. Does `emplace_back()` avoid all moves/copies?

It constructs the new element directly in the destination storage.

That does not mean other operations such as middle insertion or container moves never move elements.

---

## 39. What is the biggest performance benefit?

Predictability:

```text
no reallocation
fixed maximum memory
O(1) worst-case append
contiguous storage
```

provided the fixed-capacity tradeoff matches the domain.

---

## 40. Give a strong 30-second interview answer.

`std::inplace_vector<T, N>` is a C++26 contiguous sequence container with runtime-variable size but compile-time fixed capacity. Unlike `std::vector`, it stores its element buffer inside the container object, so it performs no dynamic allocation or reallocation for that storage. End insertion is worst-case O(1), while middle insertion and erase are O(n). The main tradeoffs are O(N) object size even when empty, inability to grow beyond `N`, and O(size) moves because the internal buffer cannot be stolen like a vector's heap allocation.

---

# Cheat Sheet

```text
std::inplace_vector<T, N>

size:
    runtime, 0..N

capacity:
    compile-time N

storage:
    contiguous
    inside object
    no container heap allocation

push_back:
    O(1) worst-case
    throws std::bad_alloc when full

try_push_back:
    O(1)
    std::optional<T&>
    std::nullopt when full

unchecked_push_back:
    O(1)
    requires size() < capacity()

middle insert / erase:
    O(n)

space:
    O(N), even when empty

move:
    O(size)

best use:
    small, naturally bounded collections

avoid:
    huge N
    unknown/unbounded growth
    frequent movement of large containers
```

## Core points to remember

1. Dynamic **size**, fixed **capacity**.
2. Storage is **inside the object**, not necessarily "on the stack".
3. Only `size()` elements are alive.
4. No container reallocation.
5. `push_back()` is worst-case O(1).
6. Capacity overflow from normal insertion throws `std::bad_alloc`.
7. `try_push_back()` / `try_emplace_back()` return `std::optional<T&>` in current C++26.
8. Middle insert/erase is O(n).
9. Empty object still reserves O(N) storage.
10. Move is O(size), not O(1).
11. `T` itself may still allocate.
12. Choose `N` from a real bound, not wishful thinking.

---

## References

- https://en.cppreference.com/cpp/container/inplace_vector
- https://en.cppreference.com/cpp/container/inplace_vector/push_back
- https://en.cppreference.com/cpp/container/inplace_vector/try_push_back
- https://en.cppreference.com/cpp/container/inplace_vector/reserve
