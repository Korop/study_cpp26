# C++26 Interview Study: `std::deque`

## Question: What is `std::deque`?

`std::deque<T>` is a **double-ended sequence container**.

Main properties:

- random access: **O(1)**
- `push_front()` / `pop_front()`: **O(1)**
- `push_back()` / `pop_back()`: **O(1)**
- middle insertion/erase: **O(n)**
- storage is **not contiguous**

```cpp
#include <deque>
#include <print>

int main()
{
    std::deque<int> values{20, 30};

    values.push_front(10);
    values.push_back(40);

    std::println("{} {} {}", values.front(), values[1], values.back());
}
```

Output:

```text
10 20 40
```

---

## Question: How is `std::deque` stored in memory?

Unlike `std::vector`, a deque does not use one contiguous dynamic array.

Typical implementation:

```text
             block map
          +----+----+----+
          | *  | *  | *  |
          +-+--+-+--+-+--+
            |    |    |
            v    v    v
          [....][....][....]
           block block block
```

Each block contains multiple contiguous elements, but the complete deque is **segmented**.

The exact layout and block size are implementation-specific.

### Important consequence

This is valid:

```cpp
std::deque<int> values{10, 20, 30};

static_assert(std::random_access_iterator<decltype(values.begin())>);
```

But a deque is not a contiguous range:

```cpp
static_assert(!std::contiguous_iterator<decltype(values.begin())>);
```

---

## Question: How can random access still be O(1)?

The implementation can locate an element using roughly:

```text
logical index
    ↓
determine block
    ↓
determine offset inside block
```

Conceptually:

```cpp
block  = index / block_size;
offset = index % block_size;
```

Therefore:

```cpp
values[i];
```

is still **O(1)**.

Compared with `std::vector`, it usually needs extra indirection.

---

## Question: Why is `push_front()` O(1)?

A vector normally has to shift every existing element:

```text
vector:

[A][B][C][D]

insert X at front

[X][A][B][C][D]
   ↑  ↑  ↑  ↑
   elements move
```

A deque can use free space in its first block or allocate another block:

```text
deque:

[X] [A][B][C][D]
```

No movement of all existing elements is required.

---

## Complexity

| Operation | Complexity |
|---|---:|
| `operator[]`, `at()` | O(1) |
| `front()`, `back()` | O(1) |
| `push_front()` | O(1) |
| `push_back()` | O(1) |
| `pop_front()` | O(1) |
| `pop_back()` | O(1) |
| middle insert/erase | O(n) |
| iteration | O(n) |
| `size()` | O(1) |

---

## Question: `std::deque` vs `std::vector`

| Property | `std::vector` | `std::deque` |
|---|---|---|
| Random access | O(1) | O(1) |
| `push_back()` | amortized O(1) | O(1) |
| `push_front()` | O(n) | O(1) |
| `pop_front()` | O(n) | O(1) |
| Contiguous memory | yes | no |
| Cache locality | excellent | weaker |
| `data()` | yes | no |
| `reserve()` | yes | no |

### Rule of thumb

Prefer `std::vector` by default.

Use `std::deque` when efficient operations at **both ends** are important.

---

## Question: `std::deque` vs `std::list`

```text
deque:
[ A B C D ] [ E F G H ]

list:
[A] <-> [B] <-> [C] <-> [D]
```

| Property | `std::deque` | `std::list` |
|---|---|---|
| Random access | O(1) | O(n) |
| Front/back insertion | O(1) | O(1) |
| Middle insertion with known iterator | O(n) movement | O(1) |
| Memory locality | better | poor |
| Storage | segmented blocks | separate nodes |

`std::list` has no `operator[]`.

---

## Question: Does random-access iterator mean contiguous memory?

No.

`std::deque` provides random-access iterators:

```cpp
#include <algorithm>
#include <deque>

std::deque<int> values{5, 2, 4, 1, 3};

std::sort(values.begin(), values.end());
```

But deque iterators are not contiguous iterators.

```cpp
static_assert(std::random_access_iterator<std::deque<int>::iterator>);
static_assert(!std::contiguous_iterator<std::deque<int>::iterator>);
```

Therefore deque does not provide vector-style contiguous storage.

---

## Question: Where are the deque object and elements stored?

For a local variable:

```cpp
void Foo()
{
    std::deque<int> values;
}
```

Conceptually:

```text
stack
+----------------------+
| deque metadata       |
| block-map state      |
+----------------------+
          |
          v
dynamic storage
+---------+
| block 0 |
+---------+

+---------+
| block 1 |
+---------+
```

The deque object itself is a local object.

Its element storage is normally dynamically allocated through its allocator.

---

## Question: What happens to references and iterators after insertion?

End insertion has unusual but useful behavior.

```cpp
std::deque<int> values{10, 20, 30};

int& ref = values[1];

values.push_front(5);

std::println("{}", ref); // 20
```

For insertion at the front/back:

- references and pointers to existing elements remain valid
- iterators may be invalidated

Middle insertion is more destructive and can invalidate references and iterators.

Do not keep deque iterators across modifications unless the operation's invalidation rules guarantee safety.

---

## Question: Why does deque have no `reserve()`?

`std::vector` maintains one contiguous allocation:

```text
size <= capacity
```

A deque grows by adding segmented blocks, so it does not expose vector-style capacity.

These do not exist:

```cpp
// values.reserve(1000);
// values.capacity();
```

`std::deque` does provide:

```cpp
values.shrink_to_fit();
```

---

## Question: What are `emplace_front()` and `emplace_back()` for?

They construct an object directly in deque storage.

```cpp
#include <deque>
#include <string>

struct Player
{
    int id;
    std::string name;
};

int main()
{
    std::deque<Player> players;

    players.emplace_back(42, "Ivan");
    players.emplace_front(1, "Alice");
}
```

Use them when direct in-place construction is useful.

---

## Question: What modern range operations are available?

Modern sequence containers provide range-oriented APIs such as:

```cpp
std::deque<int> values{3, 4};
std::vector<int> first{1, 2};
std::vector<int> last{5, 6};

values.prepend_range(first);
values.append_range(last);
```

Result:

```text
1 2 3 4 5 6
```

Related operations include:

```cpp
values.insert_range(pos, range);
values.assign_range(range);
```

---

## Question: What is relevant about `std::deque` in C++26?

C++26 makes deque operations usable in constant evaluation where the normal constexpr allocation rules are satisfied.

The important interview-level point is:

> `std::deque` becomes constexpr-capable in C++26.

This does **not** make it equivalent to `std::array`: deque still uses dynamic storage internally.

---

## Question: Why does `std::queue` use `std::deque` by default?

A queue needs efficient:

```cpp
push_back();
pop_front();
```

`std::deque` provides both in O(1).

Conceptually:

```cpp
std::queue<int>
```

uses:

```cpp
std::queue<int, std::deque<int>>
```

by default.

`std::stack` also uses `std::deque` as its default underlying container.

---

## Question: What are typical `std::deque` use cases?

Good use cases:

- BFS queues
- sliding-window algorithms
- monotonic queues
- schedulers
- work queues
- algorithms requiring efficient operations on both ends

Example monotonic deque:

```cpp
#include <deque>
#include <vector>

void Process(const std::vector<int>& values)
{
    std::deque<int> q;

    for (int value : values)
    {
        while (!q.empty() && q.back() < value)
        {
            q.pop_back();
        }

        q.push_back(value);
    }
}
```

---

# Interview answer in ~30 seconds

`std::deque` is a double-ended sequence container with O(1) indexed access and O(1) insertion/removal at both ends. Unlike `std::vector`, it uses segmented rather than contiguous storage, which makes front operations efficient but gives weaker cache locality and no `data()`, `reserve()`, or contiguous-range guarantee. Middle insertion is still O(n). I would normally prefer `vector`, and choose `deque` when efficient front and back operations are required.

---

# The compact mental map

```text
vector
    one contiguous array
    O(1) random access
    fast back
    expensive front
    best locality

deque
    segmented blocks
    O(1) random access
    fast front
    fast back
    weaker locality

list
    linked nodes
    O(n) random access
    O(1) insertion with known iterator
    poor locality
```

---

# Core points to remember

1. `deque` means **double-ended queue**.
2. Storage is **segmented**, not contiguous.
3. Indexed access is **O(1)**.
4. Front/back insertion and removal are **O(1)**.
5. Middle insertion/erase is **O(n)**.
6. Random-access iterator does **not** imply contiguous storage.
7. No `data()`, `reserve()`, or `capacity()`.
8. `std::queue` and `std::stack` use `std::deque` by default.
9. Prefer `std::vector` unless efficient front operations are actually needed.
10. In C++26, `std::deque` becomes constexpr-capable.
