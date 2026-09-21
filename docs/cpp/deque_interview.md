# C++26 Interview Study: `std::deque`

`std::deque<T>` means **double-ended queue**.

The shortest interview definition is:

> `std::deque` is a sequence container with **O(1) random access** and **O(1) insertion/removal at both front and back**, implemented using multiple separately allocated memory blocks rather than one contiguous array.

That last part is the reason `deque` exists. Humans apparently decided that neither `vector` nor `list` was sufficiently complicated, so we got something in between.

---

## 1. Basic usage

```cpp
#include <deque>
#include <iostream>

int main()
{
    std::deque<int> d;

    d.push_back(10);
    d.push_back(20);

    d.push_front(5);

    // [5, 10, 20]

    std::cout << d.front() << '\n'; // 5
    std::cout << d.back()  << '\n'; // 20

    std::cout << d[1] << '\n';      // 10

    d.pop_front();

    // [10, 20]

    d.pop_back();

    // [10]
}
```

Its main superpower compared with `vector` is:

```cpp
d.push_front(value); // O(1)
```

For `std::vector`, inserting at the beginning is **O(n)** because every existing element normally needs to move.

---

# 2. Complexity

This table is worth memorizing.

| Operation | `std::deque` |
|---|---:|
| `d[i]` | **O(1)** |
| `at(i)` | **O(1)** |
| `front()` | **O(1)** |
| `back()` | **O(1)** |
| `push_back()` | **O(1)** |
| `push_front()` | **O(1)** |
| `pop_back()` | **O(1)** |
| `pop_front()` | **O(1)** |
| insert middle | **O(n)** |
| erase middle | **O(n)** |
| `size()` | **O(1)** |
| iteration | **O(n)** |

The C++ standard explicitly requires inserting a single element at either end to take constant time. Middle insertion is linear based on how much of the deque needs to be shifted.

---

# 3. The important part: memory layout

This is probably the most important interview concept.

A `vector` looks roughly like this:

```text
std::vector<int>

       one contiguous allocation
       ↓
+----+----+----+----+----+----+
| 10 | 20 | 30 | 40 | 50 | 60 |
+----+----+----+----+----+----+
```

A `deque` is closer to:

```text
                    map
                     |
          +----------+----------+
          |          |          |
          v          v          v

       Block 0     Block 1     Block 2

      +----+----+ +----+----+ +----+----+
      | 10 | 20 | | 30 | 40 | | 50 | 60 |
      +----+----+ +----+----+ +----+----+
```

This exact internal structure is **implementation-specific**, but typical implementations use a collection of fixed-size allocated blocks plus bookkeeping pointing to those blocks.

So:

```cpp
std::deque<int> d {10, 20, 30, 40};
```

does **not** guarantee:

```text
&d[1] == &d[0] + 1
&d[2] == &d[1] + 1
...
```

across block boundaries.

That means:

```cpp
std::deque<int> d{1, 2, 3};

int* ptr = &d[0];

// DON'T assume:
// ptr + 2 == &d[2]
```

---

# 4. But how can `operator[]` still be O(1)?

Excellent interview question.

Suppose each block conceptually contains four elements:

```text
Block 0: [0 1 2 3]
Block 1: [4 5 6 7]
Block 2: [8 9 ...]
```

For:

```cpp
d[6]
```

the implementation can calculate approximately:

```cpp
block  = index / block_size;
offset = index % block_size;
```

Then:

```text
map[block] -> block pointer
block[offset]
```

So there are effectively **two levels of lookup**.

Still:

```text
O(1)
```

but usually with worse constants/cache behavior than `vector`.

Cppreference specifically notes that typical deque indexed access requires an additional pointer dereference compared with vector.

---

# 5. Why `push_front()` is O(1)

Imagine:

```text
               existing blocks

            +---------+
            |
            v

        [ ][ ][ ][ ]
        [A][B][C][D]
```

If there is room before `A`:

```text
push_front(X)

        [ ][ ][X][A]
        [B][C][D][ ]
```

If there is no room, another block can be allocated:

```text
        new block       old block

        [ ][ ][ ][X] -> [A][B][C][D]
```

Crucially, existing elements don't need to be moved into one giant new allocation.

That is fundamentally different from `vector`.

---

# 6. Compare with `vector`

For interviews:

| Property | `vector` | `deque` |
|---|---|---|
| Random access | O(1) | O(1) |
| `push_back` | amortized O(1) | O(1) |
| `push_front` | O(n) | O(1) |
| `pop_back` | O(1) | O(1) |
| `pop_front` | O(n) | O(1) |
| contiguous | ✅ | ❌ |
| CPU cache locality | excellent | good-ish |
| `.data()` | ✅ | ❌ |
| pointer/reference stability during growth | poor | better |
| memory overhead | low | higher |

Use `vector` by default.

Use `deque` when you genuinely need efficient operations at **both ends**.

That "vector by default" rule prevents a fair amount of container astrology.

---

# 7. `deque` versus `list`

This distinction also appears often.

```text
std::deque
```

uses blocks containing multiple elements.

```text
std::list
```

usually uses individually allocated nodes:

```text
deque:

[ A B C D ] -> [ E F G H ] -> [ I J K L ]


list:

[A] <-> [B] <-> [C] <-> [D]
```

Comparison:

| | `deque` | `list` |
|---|---|---|
| random access | **O(1)** | O(n) |
| front/back insert | O(1) | O(1) |
| middle insert if iterator known | O(n) movement | O(1) |
| cache locality | much better | poor |
| memory overhead | moderate | high |
| contiguous | no | no |

So this works efficiently:

```cpp
std::deque<int> d;

std::cout << d[500];
```

while:

```cpp
std::list<int> list;

// no list[500]
```

A list doesn't provide random-access iterators.

---

# 8. Random-access iterator does NOT mean contiguous

This is a nice interview trap.

`std::deque::iterator` is a **random-access iterator**.

Therefore this works:

```cpp
std::deque<int> d{5, 3, 1, 4, 2};

std::sort(d.begin(), d.end());
```

because `std::sort` needs random-access iterators.

You can also do:

```cpp
auto it = d.begin();

it += 100;

auto distance = d.end() - d.begin();
```

But deque is **not a contiguous container**.

So this does not work:

```cpp
auto ptr = d.data(); // ERROR: deque has no data()
```

And normally:

```cpp
std::span<int> s{d}; // not a contiguous range
```

is not available like it is for a vector.

---

# 9. Where does `deque` live: stack or heap?

Given your recent stack/heap questions, this distinction matters.

```cpp
void foo()
{
    std::deque<int> d;
}
```

The `deque` object itself:

```text
d
```

is a local object, therefore normally resides in the function's stack frame.

Conceptually:

```text
STACK

+----------------------+
| std::deque<int> d    |
| internal metadata    |
| block pointers/etc.  |
+----------------------+
           |
           |
           v
HEAP / allocator storage

+-------------------+
| elements block    |
+-------------------+

+-------------------+
| elements block    |
+-------------------+
```

The element storage is managed through the allocator and normally comes from dynamic storage.

So:

```cpp
std::deque<int> d;
```

does **not** mean all its elements live inside the stack object.

This is similar in principle to:

```cpp
std::vector<int> v;
```

except the allocation strategy differs.

---

# 10. Reference and iterator stability

Here `deque` gets slightly evil.

Consider:

```cpp
std::deque<int> d{10, 20, 30};

int& ref = d[1];

d.push_back(40);
```

The reference:

```cpp
ref
```

still refers to `20`.

The C++ standard says insertion at either end invalidates deque **iterators**, but does **not** invalidate references to existing elements.

So:

```cpp
int& ref = d[1];

d.push_front(5);

std::cout << ref; // still 20
```

But don't write:

```cpp
auto it = d.begin();

d.push_back(100);

// it may now be invalid
```

### Interview rule

For `push_front` / `push_back`:

```text
references/pointers to existing elements: remain valid
iterators:                            invalidated
```

That's a particularly useful difference from `vector`.

---

# 11. Middle insertion is very different

Suppose:

```cpp
std::deque<int> d{
    1, 2, 3, 4, 5, 6, 7
};

d.insert(d.begin() + 3, 100);
```

The result:

```text
1 2 3 100 4 5 6 7
```

The implementation may move elements toward whichever end is closer.

Conceptually:

```text
before:

1 2 3 | 4 5 6 7
      ^ insert

could shift:

1 2 -> 3 100 4 5 6 7
```

or shift the other direction.

The complexity is essentially:

```text
O(min(elements_before, elements_after))
```

plus inserted elements, which remains **O(n)** in Big-O notation.

And a middle insertion invalidates **all iterators and references**.

---

# 12. `emplace_front` / `emplace_back`

Same idea as other containers:

```cpp
struct Player
{
    int id;
    std::string name;

    Player(int id, std::string name)
        : id{id},
          name{std::move(name)}
    {}
};

std::deque<Player> players;

players.emplace_back(
    42,
    "Ivan"
);
```

instead of:

```cpp
players.push_back(
    Player{42, "Ivan"}
);
```

The object is constructed directly in deque storage.

You also have:

```cpp
emplace_front(...)
emplace_back(...)
emplace(position, ...)
```

---

# 13. Modern range API

Modern C++ gives deque range operations such as:

```cpp
std::deque<int> d{3, 4};

std::vector<int> front{1, 2};
std::vector<int> back{5, 6};

d.prepend_range(front);
d.append_range(back);
```

Result:

```text
1 2 3 4 5 6
```

There is also:

```cpp
d.insert_range(position, range);
d.assign_range(range);
```

These APIs are part of the modern sequence-container interface.

---

# 14. C++26-specific change: `constexpr`

One meaningful C++26 change is that the deque interface is now `constexpr`.

For example:

```cpp
constexpr void push_back(...);
constexpr void push_front(...);
constexpr reference operator[](size_type);
constexpr void pop_back();
constexpr void pop_front();
constexpr ~deque();
```

and the iterator types meet constexpr iterator requirements.

So a deque can participate in constant evaluation.

There is an important caveat: dynamic memory created during constant evaluation has to be released during that same evaluation, so simply declaring a permanent:

```cpp
constexpr std::deque<int> d = ...;
```

is generally not as straightforward as `constexpr std::array`.

For an interview, remembering **“C++26 makes deque operations constexpr-capable”** is enough unless they're explicitly digging into compile-time allocation rules.

---

# 15. `std::queue` uses `deque` by default

This is another good interview fact.

```cpp
std::queue<int> q;
```

is essentially:

```cpp
std::queue<int, std::deque<int>> q;
```

Why?

A queue needs:

```cpp
push_back()
pop_front()
```

and deque handles both efficiently:

```text
push_back  O(1)
pop_front  O(1)
```

A vector would be awful for `pop_front()`:

```text
O(n)
```

because the remaining elements need to shift.

---

# 16. `std::stack` also uses `deque` by default

Similarly:

```cpp
std::stack<int> stack;
```

normally means:

```cpp
std::stack<int, std::deque<int>> stack;
```

Although a vector can also be an excellent underlying container for stack-like behavior.

---

# 17. Main disadvantage: cache locality

This is where `vector` wins hard.

With:

```cpp
std::vector<int>
```

you have:

```text
[A][B][C][D][E][F][G][H]
```

CPU prefetchers love this.

With deque:

```text
[A][B][C][D]   [E][F][G][H]
       ↑            ↑
   allocation    allocation
```

Blocks themselves are contiguous, but the entire sequence isn't.

Therefore something like:

```cpp
for (int x : container)
{
    sum += x;
}
```

will usually favor `vector` over deque when all else is equal.

So don't choose deque because:

> "Both have O(1) access, therefore performance is identical."

Big-O has once again concealed all the interesting parts.

---

# 18. Memory overhead

Deque usually needs more memory than vector because it maintains:

- multiple allocated blocks,
- pointers/bookkeeping for those blocks,
- unused space at the front/back of boundary blocks.

Even a very small deque may carry surprisingly large allocation overhead depending on the standard-library implementation.

Thus:

```cpp
std::deque<char> d;
d.push_back('A');
```

doesn't mean only one byte of dynamic memory has been allocated.

---

# 19. Classic interview question

### Question

Why would you choose:

```cpp
std::deque<T>
```

instead of:

```cpp
std::vector<T>
```

### Strong answer

> I would normally prefer `std::vector` because its contiguous storage gives better locality and interoperability with pointer-based APIs. I would use `std::deque` when I need frequent insertion or removal at both the beginning and end while retaining O(1) indexed random access. A deque uses segmented storage, so it doesn't have vector's contiguous-memory guarantee.

That's a very solid 20-second answer.

---

# 20. Another interview question

### Why is `deque::push_front()` O(1), while `vector::insert(begin(), x)` is O(n)?

Because vector:

```text
Before:

[A][B][C][D]

push front X:

[X][A][B][C][D]
   ↑  ↑  ↑  ↑
elements have to move
```

Deque:

```text
Before:

       [A][B][C][D]

push front X:

[X] -> [A][B][C][D]
```

Conceptually it can use free space in the first block or add another block instead of moving all existing elements.

---

# 21. Interview trap: `reserve()`

Can you do:

```cpp
std::deque<int> d;

d.reserve(1000);
```

No.

`std::deque` has no:

```cpp
reserve()
capacity()
```

unlike vector.

Why?

Because its storage is segmented and expands by adding blocks rather than maintaining one contiguous allocation with:

```text
size <= capacity
```

It does have:

```cpp
shrink_to_fit();
```

but that's a non-binding request to reduce memory usage.

---

# 22. When should you actually use `deque`?

Good cases include:

```cpp
// BFS
std::deque<Node*> queue;

// sliding window
std::deque<int> window;

// scheduler
std::deque<Job> jobs;

// work queue
std::deque<Task> tasks;
```

Especially algorithms requiring:

```cpp
push_back()
pop_back()
push_front()
pop_front()
```

For example, a monotonic queue:

```cpp
std::deque<int> q;

for (int value : values)
{
    while (!q.empty() && q.back() < value)
        q.pop_back();

    q.push_back(value);
}
```

This is exactly the sort of workload deque was made for.

---

# 23. `deque` vs `vector` vs `list` cheat sheet

Memorize this:

| Feature | `vector` | `deque` | `list` |
|---|---:|---:|---:|
| random access | O(1) | O(1) | O(n) |
| push back | amortized O(1) | O(1) | O(1) |
| push front | O(n) | **O(1)** | O(1) |
| pop front | O(n) | **O(1)** | O(1) |
| middle insert | O(n) | O(n) | O(1)* |
| contiguous | ✅ | ❌ | ❌ |
| cache locality | **best** | medium | poor |
| reference stability | medium | good at ends | excellent |

`*` For list, O(1) assumes you **already have the iterator**. Finding that position can itself be O(n). An interviewer who forgets that deserves a tiny internal eye roll.

---

# 24. The mental model I recommend

Think:

```text
vector

        ONE BIG ARRAY

      [ ][ ][ ][ ][ ][ ]
```

```text
deque

       ARRAY OF BLOCKS

   -> [ ][ ][ ]
   -> [ ][ ][ ]
   -> [ ][ ][ ]
```

```text
list

       LINKED NODES

   -> [ ] -> [ ] -> [ ]
```

And therefore:

```text
vector:
    fast indexing
    fast back
    bad front
    contiguous

deque:
    fast indexing
    fast front
    fast back
    segmented

list:
    no fast indexing
    fast insertion when position known
    node-based
```

## Interview core to remember

If you retain only **six things**, retain these:

1. **`deque` = double-ended queue.**
2. **Random access is O(1).**
3. **`push_front`, `push_back`, `pop_front`, `pop_back` are O(1).**
4. **Storage is segmented, not contiguous.**
5. **End insertion can invalidate iterators but preserves references/pointers to existing elements.**
6. **Use `vector` by default; use `deque` when efficient front operations are genuinely required.**

And the **C++26-specific** detail: deque's API and iterators are now constexpr-capable.
