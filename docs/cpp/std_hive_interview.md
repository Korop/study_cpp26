# C++26 `std::hive` — Interview / Study Notes

## 1. What is `std::hive`?

**Question:** What problem does `std::hive` solve?

`std::hive<T>` is a C++26 sequence container (`<hive>`) optimized for:

- frequent insertion and erasure;
- stable addresses of surviving elements;
- better locality and lower per-element allocation overhead than node-based containers such as `std::list`.

It stores elements in **multiple blocks**, not one contiguous array.

```text
Block 1: [A][B][C][D]
Block 2: [E][F][G][ ]
Block 3: [H][ ][ ][ ]
```

When an element is erased, other elements normally do not move:

```text
erase F:

Block 2: [E][X][G][ ]
               ^
          reusable hole
```

A later insertion may reuse erased storage. The exact insertion position is unspecified.

---

## 2. Why not just use `std::vector`?

**Question:** How does `std::hive` differ from `std::vector`?

`std::vector` stores elements contiguously:

```text
[A][B][C][D]
```

Growing a vector may allocate a new buffer and move its elements:

```text
old: [A][B][C][D]

new: [A][B][C][D][ ][ ][ ][ ]
```

Therefore pointers/references/iterators may be invalidated.

`std::hive` grows by adding blocks, so normal insertion does not move existing elements.

```cpp
std::hive<Player> players;

auto it = players.insert(Player{1, "Ivan"});
Player* player = &*it;

players.insert(Player{2, "Alex"}); // player remains valid
```

Use `std::vector` when contiguous storage and random access are more important.

Use `std::hive` when stable element addresses and cheap insertion/erasure are more important.

---

## 3. What happens after `erase()`?

**Question:** Does erasing one element move the following elements?

No.

```text
before: [A][B][C][D]
erase B
after:  [A][X][C][D]
```

Only the erased object is destroyed. Iterators, pointers, and references to other elements remain valid.

```cpp
auto a = hive.insert(A{});
auto b = hive.insert(B{});

A* ptr = &*a;

hive.erase(b); // ptr is still valid
```

But:

```cpp
hive.erase(a);
// ptr is now dangling
```

Single-element `erase()` returns an iterator to the next live element, so this pattern is natural:

```cpp
for (auto it = players.begin(); it != players.end();)
{
    if (it->dead)
    {
        it = players.erase(it);
    }
    else
    {
        ++it;
    }
}
```

---

## 4. How are erased holes skipped?

**Question:** If blocks contain erased slots, does iteration scan every hole?

The standard requires efficient iteration; it does not require a specific internal representation.

Implementations can maintain per-block metadata, commonly described as a **skipfield-style structure**, to jump over erased runs efficiently.

Conceptually:

```text
[A][X][X][X][B]
    \_______/
       skip
```

Iterator increment/decrement is constant-time, so a full traversal is linear in the number of live elements:

```text
iterate all elements -> O(N)
```

Do not treat the skipfield as mandated object layout; it is an implementation technique.

---

## 5. What iterator category does `std::hive` provide?

**Question:** Can `std::hive` use random-access operations?

No. `std::hive` provides **bidirectional iterators**.

```cpp
static_assert(std::ranges::bidirectional_range<std::hive<int>>);
static_assert(!std::ranges::random_access_range<std::hive<int>>);
static_assert(!std::ranges::contiguous_range<std::hive<int>>);
```

Therefore:

```cpp
auto it = hive.begin();
++it;   // OK
--it;   // OK
```

but:

```cpp
// hive[5];            // error: no operator[]
// hive.begin() + 5;   // error: not random-access
```

To reach the Nth element:

```cpp
auto it = std::next(hive.begin(), 5); // O(N)
```

If indexing is fundamental to the algorithm, prefer `std::vector` or another random-access container.

---

## 6. Is `std::hive` contiguous?

**Question:** Are all hive elements adjacent in memory?

No.

Each block contains multiple elements, but different blocks are separate allocations.

```text
Block 1: [A][B][C][D]

Block 2: [E][F][G][H]
```

So `std::hive` sacrifices contiguous storage and random access in exchange for stable element locations and cheap erase/insert.

---

## 7. Complexity

Let:

- `N` = number of live elements;
- `K` = number of inserted/erased elements;
- `C` = allocated capacity.

| Operation | Complexity |
|---|---:|
| `size()`, `empty()` | `O(1)` |
| `begin()`, iterator increment/decrement | `O(1)` |
| single `insert()` / `emplace()` | `O(1)` |
| insert `K` elements | `O(K)` |
| single `erase(iterator)` | `O(1)` |
| erase `K` elements | `O(K)` normally* |
| iterate all live elements | `O(N)` |
| search by value | `O(N)` |
| Nth element via iteration | `O(N)` |
| `sort()` | `O(N log N)` comparisons |
| random access | not supported |

\* Range erase can also incur block-management work when blocks become empty.

---

## 8. Space complexity

**Question:** What is the space complexity of `std::hive`?

A useful precise model is:

```text
O(C)
```

where `C = capacity()`.

Storage includes:

- element blocks;
- unused/reusable slots;
- bookkeeping metadata for blocks and erased regions.

`capacity()` can be larger than `size()`:

```cpp
std::hive<int> values;
values.reserve(1'000'000);

// size() == 0
// capacity() >= 1'000'000
```

Therefore saying only "`O(N)` where `N = size()`" is imprecise after large reservations or many erasures.

---

## 9. Pointer/reference stability

**Question:** Which operations preserve addresses?

Normal insertion:

```cpp
hive.emplace(...);
```

does not invalidate pointers/references/iterators to existing elements, although `end()` changes.

Erasing one element invalidates only handles to that erased element; other elements remain valid.

```text
insert(other) -> existing object addresses stable
erase(other)  -> existing object addresses stable
erase(target) -> handles to target invalid
```

### Important exceptions

Operations that reorganize/reallocate storage can invalidate handles, for example:

- `sort()` may invalidate references, pointers, and iterators;
- `shrink_to_fit()` may reallocate and invalidate them;
- `reshape()` may reallocate blocks.

So the interview-safe statement is:

> `std::hive` provides stable element addresses across normal insertions and erasures of other elements, not across every possible member function.

---

## 10. Why does `std::hive` have its own `sort()`?

**Question:** Why can't we use `std::sort(hive.begin(), hive.end())`?

`std::sort` requires random-access iterators, while hive iterators are bidirectional.

```cpp
// std::sort(hive.begin(), hive.end()); // error

hive.sort(); // OK
```

Complexity:

```text
O(N log N) comparisons
```

Unlike ordinary insertion/erasure, `hive.sort()` may reorganize storage and may invalidate pointers/references/iterators.

---

## 11. Why not just use `std::list`?

**Question:** Both `std::list` and `std::hive` provide stable element addresses. What is the difference?

A typical list stores each element as a separate node:

```text
[prev|A|next] -> [prev|B|next] -> [prev|C|next]
```

This usually means:

- one allocation per node;
- two link pointers per element;
- poor cache locality when nodes are scattered.

Hive groups many elements into blocks:

```text
[A][B][C][D]    [E][F][G][H]
```

This generally gives better locality and less per-element allocation overhead.

### Comparison

| Property | `std::vector` | `std::list` | `std::hive` |
|---|---|---|---|
| Contiguous | yes | no | no |
| Random access | `O(1)` | no | no |
| Single known-element erase | `O(N)` due to shifting | `O(1)` | `O(1)` |
| Stable surviving addresses | not generally | yes | yes |
| Per-element node allocation | no | typically yes | no |
| Locality | excellent | poor | generally better than list |
| Reuses erased slots | not directly | allocator-dependent | designed for it |

---

## 12. Is insertion order controllable?

**Question:** Can I insert an element exactly before a particular hive iterator?

No.

Hive insertion location is **unspecified**. Hint overloads exist for interface compatibility, but the hint does not establish a required position.

This freedom lets the container reuse erased slots efficiently.

Therefore `std::hive` is not a replacement for `std::list` when exact positional insertion is part of the algorithm.

---

## 13. Typical use cases

Good candidates:

- game entities;
- particles;
- simulation objects;
- connections/sessions;
- jobs/tasks;
- objects frequently created and destroyed;
- systems that keep raw pointers/references to live objects.

Example:

```text
Every frame:

spawn entities
erase dead entities
iterate live entities
other systems keep Entity*
```

This maps naturally to `std::hive`.

Less suitable when you need:

- `container[index]`;
- contiguous memory;
- SIMD-friendly dense data;
- direct interoperability with APIs expecting contiguous ranges.

In such cases `std::vector` is usually a better default.

---

## 14. Compact C++26 example

```cpp
#include <hive>
#include <print>
#include <ranges>
#include <string>

struct Player
{
    int id{};
    std::string name;
    bool dead{};
};

int main()
{
    static_assert(std::ranges::bidirectional_range<std::hive<Player>>);
    static_assert(!std::ranges::random_access_range<std::hive<Player>>);
    static_assert(!std::ranges::contiguous_range<std::hive<Player>>);

    std::hive<Player> players;

    auto ivanIt = players.insert(Player{1, "Ivan"});
    auto alexIt = players.insert(Player{2, "Alex"});
    Player* ivan = &*ivanIt;

    players.insert(Player{3, "Kate"}); // existing elements do not move
    players.erase(alexIt);             // Ivan remains valid

    std::println("{}: {}", ivan->id, ivan->name);

    players.insert(Player{4, "Bob", true});

    for (auto it = players.begin(); it != players.end();)
    {
        if (it->dead)
        {
            it = players.erase(it);
        }
        else
        {
            ++it;
        }
    }

    players.sort([](const Player& a, const Player& b)
    {
        return a.id < b.id;
    });

    for (const auto& player : players)
    {
        std::println("{}: {}", player.id, player.name);
    }
}
```

Note: do not use `ivan` after `sort()` unless you reacquire it; `sort()` may invalidate element pointers/references/iterators.

---

## 15. Core points to remember

1. `std::hive` is a C++26 **sequence container**.
2. Storage consists of multiple **element blocks**.
3. Erased slots can be reused.
4. Single insertion and erasure are `O(1)`.
5. Surviving elements normally keep their addresses across insert/erase.
6. Iterators are **bidirectional**, not random-access.
7. There is no `operator[]`.
8. The container is not contiguous.
9. Full traversal is `O(N)`.
10. Space tracks allocated capacity: approximately `O(C)`.
11. It usually offers better locality than `std::list`.
12. Insertion position is unspecified.
13. `hive.sort()` exists because `std::sort` requires random-access iterators.
14. Reorganizing operations such as `sort()` / possible `shrink_to_fit()` reallocation can invalidate handles.

---

## 16. The compact mental map

```text
std::vector
    |
    +-- contiguous
    +-- O(1) random access
    +-- excellent locality
    `-- growth / erase may move elements

std::list
    |
    +-- stable nodes
    +-- O(1) known-node erase
    `-- per-node allocation + poor locality

std::hive
    |
    +-- block-based
    +-- reusable erased slots
    +-- O(1) insert / erase
    +-- stable surviving objects
    +-- better locality than node lists
    `-- no random access / not contiguous
```

---

## 17. Interview answer in ~30 seconds

> `std::hive` is a C++26 sequence container intended for workloads with frequent insertion and erasure where surviving objects should keep stable addresses. It stores elements in multiple blocks and can reuse erased slots, so single-element insertion and erasure are constant time without shifting other elements. Its iterators are bidirectional, so there is no random access or `operator[]`, and the storage is not contiguous. Compared with `std::list`, hive generally improves locality and avoids one allocation and link pointers per element. Compared with `std::vector`, it trades contiguous storage and indexing for stable element locations and cheap erase/insert. Reorganizing operations such as `sort()` are exceptions and may invalidate handles.

---

## Cheat Sheet

```text
Header:             <hive>
Standard:           C++26
Category:           sequence container
Storage:            multiple blocks
Contiguous:         no
Iterator:           bidirectional
Random access:      no
operator[]:         no

insert one:         O(1)
erase one:          O(1)
iterate all:        O(N)
search:             O(N)
sort:               O(N log N) comparisons
space:              O(capacity)

Normal insert:
    existing pointers/references/iterators stay valid
    end() is invalidated

erase(x):
    handles to x invalid
    handles to other elements stay valid

Best mental model:
    vector locality
        +
    list-like stable objects / cheap erase
        -
    random access
```

## References

- C++26 `std::hive` library specification / cppreference.
- WG21 proposal family P0447, which introduced the hive design.
