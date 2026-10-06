# C++26 Atomics, Memory Ordering, `seq_cst`, and Mutexes

## Study goals

This note answers the following interview-oriented questions:

- What does `std::atomic` actually guarantee?
- Is `atomic::load()` blocking? How is it different from `atomic::wait()`?
- Why does `fetch_add(1, std::memory_order_relaxed)` never lose increments?
- Why can `load(relaxed)` observe an older value while `fetch_add(relaxed)` still remains correct?
- What is the difference between `relaxed`, `release`, `acquire`, and `seq_cst`?
- Why can another thread observe one shared value but not yet have ordering guarantees for other shared values?
- Why does `std::mutex` avoid this problem?
- How do pauses such as `sleep_for()` affect execution?
- When should a counter use `relaxed`, and when is synchronization required?

---

# 1. Start with the important distinction: atomicity vs ordering

These are different concepts.

```cpp
std::atomic<int> counter{0};
counter.fetch_add(1, std::memory_order_relaxed);
```

`memory_order_relaxed` does **not** mean "partially atomic".

The operation is still fully atomic.

It means:

```text
atomicity of this operation: YES
ordering/synchronization of other memory accesses: NO
```

This distinction is the foundation of the whole topic.

---

# 2. `load()` does not wait

```cpp
std::atomic<int> counter{0};

auto value = counter.load(std::memory_order_acquire);
```

`load()` performs one read and returns.

It does **not**:

- wait for another thread;
- wait until the value changes;
- block like a mutex;
- behave like an event.

If the value is `0` at the moment the load observes it, it returns `0`.

If actual waiting is required, C++ provides `atomic::wait()`:

```cpp
std::atomic<int> state{0};

// Wait while state is still 0.
state.wait(0);
```

Mental model:

```text
load() -> read now
wait() -> wait while the atomic still equals the supplied value
```

---

# 3. Why normal `++counter` is unsafe

```cpp
int counter = 0;
```

Two threads execute:

```cpp
++counter;
```

Conceptually, a normal increment requires multiple steps:

```cpp
auto value = counter; // read
++value;              // modify
counter = value;      // write
```

Possible interleaving:

```text
counter = 0

Thread A                 Thread B

read 0                   read 0
add 1                    add 1
write 1                  write 1

final counter = 1
```

One increment was lost.

More importantly, concurrent unsynchronized read/write access to a normal `int` is a **data race**, therefore the C++ program has undefined behavior.

---

# 4. Atomic `fetch_add()` is one indivisible RMW operation

```cpp
std::atomic<int> counter{0};

counter.fetch_add(1, std::memory_order_relaxed);
```

`fetch_add()` is an atomic **read-modify-write** operation.

Think of it as one indivisible transition:

```text
read current value
      +
modify it
      +
write the new value
```

Another atomic modification of the same object cannot interleave "inside" this RMW operation.

For one atomic object, all modifications have a single **modification order**.

Two concurrent increments can therefore be ordered as:

```text
counter = 0

Thread A: 0 -> 1
Thread B: 1 -> 2
```

or:

```text
Thread B: 0 -> 1
Thread A: 1 -> 2
```

but not as two independent successful:

```text
A: 0 -> 1
B: 0 -> 1
```

Hence:

```cpp
std::atomic<int> counter{0};

void Worker()
{
    for (int i = 0; i < 1'000'000; ++i)
        counter.fetch_add(1, std::memory_order_relaxed);
}
```

does not lose increments.

---

# 5. Clean experiment: relaxed increments

```cpp
#include <atomic>
#include <print>
#include <thread>
#include <vector>

constexpr int Threads = 8;
constexpr int IncrementsPerThread = 1'000'000;

std::atomic<int> counter{0};

void Worker()
{
    for (int i = 0; i < IncrementsPerThread; ++i)
        counter.fetch_add(1, std::memory_order_relaxed);
}

int main()
{
    std::vector<std::thread> threads;

    for (int i = 0; i < Threads; ++i)
        threads.emplace_back(Worker);

    for (auto& thread : threads)
        thread.join();

    constexpr int Expected = Threads * IncrementsPerThread;
    static_assert(Expected == 8'000'000);

    std::println("Expected: {}", Expected);
    std::println("Actual:   {}", counter.load(std::memory_order_relaxed));
}
```

Expected result:

```text
Expected: 8000000
Actual:   8000000
```

`relaxed` is sufficient because the counter itself is the only state being coordinated.

---

# 6. Why `load() + store()` is not equivalent to `fetch_add()`

This is atomic:

```cpp
counter.fetch_add(1, std::memory_order_relaxed);
```

This is **not one atomic operation**:

```cpp
auto value = counter.load(std::memory_order_relaxed);
counter.store(value + 1, std::memory_order_relaxed);
```

Each individual operation is atomic, but the pair is not.

Possible execution:

```text
counter = 0

Thread A                 Thread B

load -> 0                load -> 0
                         |
                         |
store 1                  store 1

final counter = 1
```

There is no data race because the accesses are atomic.

The algorithm is simply wrong.

Important rule:

```text
atomic load + atomic store != atomic read-modify-write
```

Use an RMW operation such as:

```cpp
fetch_add()
fetch_sub()
exchange()
compare_exchange_weak()
compare_exchange_strong()
```

when the read and update must form one atomic state transition.

---

# 7. "A load can see an old value" does not mean `fetch_add()` can lose updates

Consider:

```cpp
std::atomic<int> counter{0};
```

Thread A:

```cpp
counter.fetch_add(1, std::memory_order_relaxed);
```

Thread B:

```cpp
auto value = counter.load(std::memory_order_relaxed);
```

If B executes before A's increment becomes the modification it observes:

```text
B: load -> 0
A: 0 -> 1
```

then B legitimately gets `0`.

If A is observed first:

```text
A: 0 -> 1
B: load -> 1
```

then B gets `1`.

This does **not** threaten `fetch_add()` correctness.

The operations have different semantics:

```text
load()
    -> observe one value

fetch_add()
    -> atomic read + modify + write
```

A load only observes.

A read-modify-write participates in the modification order and atomically produces the next value.

---

# 8. `sleep_for()` changes timing, not the memory model

Example:

```cpp
std::atomic<int> counter{0};

void Producer()
{
    std::this_thread::sleep_for(std::chrono::seconds{2});
    counter.fetch_add(1, std::memory_order_relaxed);
}

void Reader()
{
    std::println("{}", counter.load(std::memory_order_relaxed));
}
```

The reader will often print:

```text
0
```

because it executes before the increment.

Move the pause to the reader:

```cpp
void Reader()
{
    std::this_thread::sleep_for(std::chrono::seconds{2});
    std::println("{}", counter.load(std::memory_order_relaxed));
}
```

and it will very likely observe `1`.

But:

```text
sleep_for() is timing
sleep_for() is not a synchronization protocol
```

Never design correctness around:

> "The other thread probably had enough time."

Use a mutex, atomic synchronization, condition variable, semaphore, latch, barrier, or another real synchronization mechanism.

---

# 9. Why memory ordering matters: one atomic plus other shared data

Now the atomic is no longer just a counter.

It is also used as a **publication marker**.

```cpp
struct SpinResult
{
    int payout{};
    int auditId{};
};

SpinResult result;
std::atomic<int> completed{0};
```

Producer:

```cpp
void Calculate()
{
    result.payout = 500;
    result.auditId = 42;

    completed.store(1, std::memory_order_release);
}
```

Consumer:

```cpp
void Read()
{
    if (completed.load(std::memory_order_acquire) == 1)
        std::println("payout={}, audit={}", result.payout, result.auditId);
}
```

The atomic `completed` now has two roles:

```text
1. stores a value
2. publishes other memory writes
```

The second role is where `release/acquire` matters.

---

# 10. What if both operations use `relaxed`?

Producer:

```cpp
void Calculate()
{
    result.payout = 500;
    result.auditId = 42;

    completed.store(1, std::memory_order_relaxed);
}
```

Consumer:

```cpp
void Read()
{
    if (completed.load(std::memory_order_relaxed) == 1)
        std::println("payout={}, audit={}", result.payout, result.auditId);
}
```

Looking only at `completed`, everything is fine.

The consumer can observe:

```text
completed == 1
```

But `relaxed` does not establish a synchronization relation for the ordinary writes to:

```cpp
result.payout
result.auditId
```

Therefore `completed == 1` does **not** by itself create the rule:

```text
consumer must now observe every ordinary write
that producer executed before completed = 1
```

For ordinary non-atomic `result` fields, concurrent unsynchronized access may form a data race, which is undefined behavior.

This is why `relaxed` is correct for a standalone statistics counter but is generally not sufficient when the atomic is being used to publish other data.

---

# 11. Critical hardware foundation: program order is not cross-core observation order

This is the hardware fact that makes the C++ memory model necessary.

Consider two different atomic locations so the example contains **no data race**:

```cpp
std::atomic<int> payout{0};
std::atomic<int> completed{0};
```

Producer:

```cpp
void Calculate()
{
    payout.store(500, std::memory_order_relaxed);   // A
    completed.store(1, std::memory_order_relaxed); // B
}
```

Consumer:

```cpp
void Read()
{
    if (completed.load(std::memory_order_relaxed) == 1)
        std::println("{}", payout.load(std::memory_order_relaxed));
}
```

Inside the producer thread the source-code order is clear:

```text
A: payout = 500
        |
        v
B: completed = 1
```

`A` is sequenced-before `B`.

But that does **not** automatically create this guarantee for another CPU core:

```text
"If Core B can observe completed == 1,
 then Core B must already observe payout == 500."
```

With relaxed ordering, C++ provides no such cross-location synchronization rule.

The important distinction is:

```text
program order in one thread
        !=
guaranteed observation order in another thread
```

This is not because each thread has its own heap. Threads of one process share the same logical address space.

The reason is that a modern multicore CPU does not behave like several cores directly reading and writing one simple RAM array after every instruction.

A simplified machine looks more like:

```text
Thread A
   |
Core A
   |
registers
   |
store/write buffers
   |
L1 cache
   |
cache-coherence / interconnect
   |
shared cache / memory system
   |
L1 cache
   |
Core B
   |
Thread B
```

Exact microarchitecture differs between CPUs, but the important mechanisms are common:

- private per-core caches;
- cache lines rather than byte-by-byte RAM transactions;
- store/write buffers;
- speculative execution;
- out-of-order execution;
- load/store queues;
- cache-coherence protocols;
- compiler reordering before the code even reaches the CPU.

These mechanisms exist because waiting synchronously for every memory access to become globally visible would destroy performance.

## 11.1 ARM is explicitly weakly ordered

Armv8-A/AArch64 uses a **weakly ordered memory model**.

Arm's own memory-model documentation distinguishes instruction execution order from memory-access ordering: even if instructions appear to execute in program order, caches and write buffers can cause the corresponding memory accesses to be observed in another order by the memory system.

This is directly relevant to Apple Silicon because C++ programs on Apple Silicon execute on an AArch64/Arm64 architecture.

Therefore the following intuition is invalid:

```text
Core A executed:
    payout = 500
    completed = 1

therefore every other core must necessarily observe:
    payout = 500
    completed = 1
in exactly that order
```

Without the required synchronization, that conclusion is not supplied by the C++ memory model.

## 11.2 A useful hardware-level picture

Do **not** treat this as an exact implementation diagram of a specific Apple M3 core. It is only a conceptual model.

Producer:

```text
Core A

payout = 500
    |
    v
store/write machinery
    |
    | update to payout is still progressing
    |
completed = 1
    |
    +---------------------------> Core B can observe completed == 1
```

Consumer:

```text
Core B

completed.load() -> 1

payout.load()    -> ordering relative to that other location
                    is not established by relaxed operations
```

The key point is not that the first write "did not execute".

It did execute from the producer's point of view.

The missing guarantee is:

```text
When another core observes B,
it must also already observe A.
```

`memory_order_relaxed` does not establish that relationship.

## 11.3 Cache coherence is not memory ordering

This distinction is essential.

**Cache coherence** answers approximately:

> How do cores eventually maintain a coherent history for one memory location/cache line?

For one atomic object:

```cpp
std::atomic<int> counter{0};
```

its modifications have one modification order:

```text
0 -> 1 -> 2 -> 3
```

A relaxed atomic does not permit arbitrary corruption of that one object's modification history.

But **memory ordering** asks a different question:

> If Core A accesses X and then Y, in what order must another core observe the effects on X and Y?

For different locations:

```cpp
std::atomic<int> payout{0};
std::atomic<int> completed{0};
```

coherence of `payout` and coherence of `completed` do not by themselves create a cross-variable rule:

```text
observe completed == 1
        =>
must already observe payout == 500
```

That is an **ordering/synchronization** guarantee, not merely a coherence guarantee.

Compact form:

```text
coherence:
    consistency of one location

ordering:
    relationship between multiple memory accesses/locations
```

## 11.4 "They are fields of the same object" does not help

This still contains separate memory locations:

```cpp
struct State
{
    std::atomic<int> payout{0};
    std::atomic<int> completed{0};
};

State state;
```

And this still does not create synchronization:

```cpp
state.payout.store(500, std::memory_order_relaxed);
state.completed.store(1, std::memory_order_relaxed);
```

The facts that the fields:

- belong to the same C++ object;
- were allocated by one `new`;
- live in the same heap allocation;
- are adjacent in memory;

do not establish a C++ happens-before relation.

Even if they happen to occupy the same cache line on a particular machine, **cache-line placement is not a C++ synchronization primitive**.

Never design correctness around:

```text
"They are close together in memory,
so the other core will probably see them together."
```

That may appear to work on one compiler/CPU/build and still have no language-level correctness guarantee.

## 11.5 Hardware reordering and compiler reordering are separate issues

There are two optimizers in the story:

```text
C++ source
    |
    v
compiler
    |
    | may transform/reorder when the C++ memory model permits it
    v
machine instructions
    |
    v
CPU
    |
    | may use caches, buffers, speculation,
    | and architecture-permitted memory ordering
    v
observable multicore behavior
```

So even on a CPU architecture with relatively strong hardware ordering, correct C++ synchronization must still be expressed in the language.

`std::memory_order` tells **both**:

1. the compiler which transformations would violate the C++ memory model;
2. the backend/CPU mapping which machine ordering instructions or barriers are required.

## 11.6 ARM vs x86

Do not memorize the oversimplification:

```text
"all modern CPUs reorder everything"
```

Architectures provide different guarantees.

A useful high-level comparison is:

```text
x86/x86-64:
    relatively strong ordering (TSO-style)

ARM/AArch64:
    weaker ordering; more reorderings are architecturally permitted

C++:
    portable language memory model above both
```

On strongly ordered architectures, some acquire/release operations may compile to ordinary-looking loads/stores because the hardware already supplies much of the required ordering.

On weakly ordered ARM systems, the compiler may need acquire/release instructions or barriers such as architecture-specific load-acquire/store-release forms.

The C++ source code should describe the required semantics rather than depending on accidental properties of one CPU.

## 11.7 Why CPUs are designed this way

A globally synchronous model like:

```text
store X
wait until every relevant core/cache agrees
store Y
wait again
```

would serialize enormous amounts of work.

Instead, modern CPUs try to keep execution units busy while memory operations are still progressing.

That permits:

- stores to wait in buffers;
- future instructions to execute while older memory operations are pending;
- speculative loads;
- cache-line transfers to happen asynchronously;
- independent memory operations to overlap.

The performance gain is enormous.

The price is that concurrent software must explicitly request synchronization when cross-thread ordering matters.

This is why C++ has:

```text
relaxed
acquire
release
acq_rel
seq_cst
mutexes
fences
```

They are not arbitrary language decoration. They express which ordering constraints the compiler and CPU must preserve.

## 11.8 What release/acquire adds to this exact example

Producer:

```cpp
void Calculate()
{
    payout.store(500, std::memory_order_relaxed);

    completed.store(1, std::memory_order_release);
}
```

Consumer:

```cpp
void Read()
{
    if (completed.load(std::memory_order_acquire) == 1)
        std::println("{}", payout.load(std::memory_order_relaxed));
}
```

The `payout` operations can remain relaxed.

The synchronization is carried by `completed`:

```text
Core A / Producer

payout = 500
    |
    | sequenced-before
    v
completed = 1 [RELEASE]
    |
    | synchronizes-with
    v
completed load [ACQUIRE] -> 1
    |
    | sequenced-before
    v
payout load

Core B / Consumer
```

Now C++ establishes:

```text
payout.store(500)
        happens-before
payout.load()
```

provided the acquire observes the appropriate release value/release sequence.

That is the bridge that did **not** exist with relaxed publication.

## 11.9 Why a mutex also fixes the hardware-ordering problem

A mutex is not merely:

```text
"only one thread may execute this code"
```

It also supplies memory synchronization.

Conceptually:

```text
Thread A / Core A

write shared data
write shared data
    |
mutex.unlock()
    |
    | release/acquire-style synchronization
    v
mutex.lock()
    |
read shared data
read shared data

Thread B / Core B
```

The mutex implementation and the C++ memory model ensure that the critical-section writes before `unlock()` become visible to the thread that subsequently acquires the same mutex.

So a mutex solves **both**:

```text
1. mutual exclusion
2. cross-core memory ordering/visibility
```

This is why correctly mutex-protected ordinary data does not need to be individually atomic.

## 11.10 Hardware takeaway

The critical interview mental model is:

```text
One shared virtual address
does NOT mean
one instantly synchronized physical observation across all cores.
```

Instead:

```text
shared logical memory
        +
per-core execution/cache/buffering
        +
architecture memory-order rules
        +
compiler transformations
        =
need for an explicit C++ synchronization contract
```

And therefore:

```text
program order
    is not automatically
cross-thread observation order
```

That single fact explains why `release/acquire`, `seq_cst`, fences, and mutex synchronization exist at all.

References for this hardware motivation:

- Arm, *Armv8-A memory model*: https://developer.arm.com/-/media/Arm%20Developer%20Community/PDF/Learn%20the%20Architecture/Armv8-A%20memory%20model%20guide.pdf
- Arm, *Armv8 sequential consistency*: https://developer.arm.com/community/arm-community-blogs/b/tools-software-ides-blog/posts/armv8-sequential-consistency
- cppreference, `std::memory_order`: https://en.cppreference.com/w/cpp/atomic/memory_order
# 12. Same object or same heap allocation does not change the rule

Even if both values are fields of one object:

```cpp
struct State
{
    std::atomic<int> payout{0};
    std::atomic<int> completed{0};
};

State state;
```

they are still distinct atomic objects / memory locations:

```cpp
state.payout
state.completed
```

This:

```cpp
state.payout.store(500, std::memory_order_relaxed);
state.completed.store(1, std::memory_order_relaxed);
```

does not create cross-object synchronization.

A consumer may not infer a guaranteed ordering for `payout` merely because it observed `completed == 1`.

Object layout, heap allocation, and being in the same `struct` do not create synchronization.

---

# 13. What `release` means

Producer:

```cpp
result.payout = 500;
result.auditId = 42;

completed.store(1, std::memory_order_release);
```

Mental model:

```text
ordinary writes
    |
    v
RELEASE publication point
```

A release operation says, conceptually:

> Writes sequenced before this release can be published through this synchronization point.

It does **not** block.

It does **not** wait.

---

# 14. What `acquire` means

Consumer:

```cpp
if (completed.load(std::memory_order_acquire) == 1)
{
    std::println("{}", result.payout);
}
```

`load(acquire)` still performs one immediate load.

It does not wait.

Its extra meaning is conditional:

> If this acquire observes the value published by the relevant release operation, then writes before that release are ordered before reads after this acquire.

Conceptually:

```text
Producer

result.payout = 500
result.auditId = 42
        |
        v
completed.store(1, RELEASE)
        |
        | synchronizes-with
        v
completed.load(ACQUIRE) -> 1
        |
        v
read result.payout
read result.auditId

Consumer
```

The resulting relation is:

```text
producer ordinary writes
        happens-before
consumer ordinary reads
```

That is the purpose of release/acquire.

---

# 15. `release/acquire` vs `relaxed`

## Only a counter

```cpp
std::atomic<int> spins{0};

void RecordSpin()
{
    spins.fetch_add(1, std::memory_order_relaxed);
}
```

`relaxed` is usually enough.

There are no other data being published through the counter.

---

## Counter used as a publication marker

```cpp
SpinResult result;
std::atomic<int> completed{0};

void Calculate()
{
    result = {.payout = 500, .auditId = 42};
    completed.store(1, std::memory_order_release);
}

void Read()
{
    if (completed.load(std::memory_order_acquire) == 1)
        std::println("{} {}", result.payout, result.auditId);
}
```

Now release/acquire matters because `completed` communicates the readiness of other memory.

Compact rule:

```text
relaxed:
    atomicity only for the atomic object

release/acquire:
    atomicity
    +
    cross-thread ordering / publication
```

---

# 16. Ordering direction matters

Correct publication:

```cpp
result.payout = 500;
completed.store(1, std::memory_order_release);
```

Consumer:

```cpp
if (completed.load(std::memory_order_acquire) == 1)
    use(result.payout);
```

Data is written **before** release and read **after** acquire.

---

Incorrect:

```cpp
completed.store(1, std::memory_order_release);

std::this_thread::sleep_for(std::chrono::seconds{1});

result.payout = 500;
```

Consumer:

```cpp
if (completed.load(std::memory_order_acquire) == 1)
    use(result.payout);
```

The write to `payout` happens after the release, so that release does not publish it.

The consumer may observe `completed == 1` before `payout = 500` happens.

Release/acquire is directional:

```text
writes BEFORE release
        |
        v
     release
        |
        v
     acquire
        |
        v
reads AFTER acquire
```

---

# 17. What does a mutex add?

Consider ordinary data:

```cpp
struct State
{
    int payout{};
    int completed{};
};

State state;
std::mutex mutex;
```

Producer:

```cpp
void Calculate()
{
    std::lock_guard lock{mutex};

    state.payout = 500;
    state.completed = 1;
}
```

Consumer:

```cpp
void Read()
{
    std::lock_guard lock{mutex};

    if (state.completed == 1)
        std::println("{}", state.payout);
}
```

A mutex provides two major properties:

```text
1. mutual exclusion
2. memory synchronization
```

---

# 18. Mutual exclusion

If Thread A owns the mutex:

```cpp
mutex.lock();
```

Thread B cannot successfully enter a section protected by the same mutex until A unlocks it.

This protects an entire critical section:

```cpp
{
    std::lock_guard lock{mutex};

    player.balance -= bet;
    player.totalBet += bet;
    ++player.spins;
}
```

The group of operations is protected from concurrent execution by other users of that mutex.

This is different from several independent atomic operations.

---

# 19. Mutex also provides memory visibility

The important part often omitted in beginner explanations:

```text
Thread A: mutex.unlock()
             |
             | synchronizes-with
             v
Thread B: successful mutex.lock()
```

Therefore:

```text
writes before unlock
        happens-before
reads after the corresponding lock
```

Conceptually:

```text
Thread A

payout = 500
completed = 1
    |
    v
mutex.unlock()
    |
    | synchronization
    v
mutex.lock()
    |
    v
read completed
read payout

Thread B
```

So mutex synchronization prevents the visibility problem that a purely relaxed publication protocol would have.

---

# 20. Useful mental relation: mutex and release/acquire

At the memory-model level, a useful mental approximation is:

```text
mutex.unlock() -> release-like operation
mutex.lock()   -> acquire-like operation
```

This is not a claim that every mutex implementation is literally one release store plus one acquire load.

It means that successful hand-off of the mutex provides the required release/acquire-style synchronization semantics.

---

# 21. Atomics are not tiny mutexes

Suppose:

```cpp
struct Player
{
    std::atomic<int> balance{100};
    std::atomic<int> totalBet{0};
    std::atomic<int> spins{0};
};
```

Then:

```cpp
player.balance.fetch_sub(10);
player.totalBet.fetch_add(10);
player.spins.fetch_add(1);
```

Each operation is atomic.

But the three operations together are **not one transaction**.

A reader may observe an intermediate logical state:

```text
balance  = 90
totalBet = 0
spins    = 0
```

Even changing all operations to `memory_order_seq_cst` does not combine them into one atomic transaction.

If a business invariant spans several values, consider:

- one mutex-protected critical section;
- one packed atomic state;
- CAS-based state machine;
- another explicit synchronization protocol.

---

# 22. Atomic check-then-update can still be wrong

Incorrect:

```cpp
std::atomic<int> balance{100};

bool TryBet(int amount)
{
    if (balance.load() >= amount)
    {
        balance.fetch_sub(amount);
        return true;
    }

    return false;
}
```

Two threads may both load `100`, both accept a bet of `80`, and finally produce:

```text
balance == -60
```

Every individual operation can be perfectly atomic and even `seq_cst`.

The business invariant is still broken because:

```text
load
check
modify
```

was not one atomic state transition.

---

# 23. CAS implementation for a balance invariant

```cpp
#include <atomic>

bool TryBet(std::atomic<int>& balance, int amount)
{
    int current = balance.load(std::memory_order_relaxed);

    while (current >= amount)
    {
        if (balance.compare_exchange_weak(
                current,
                current - amount,
                std::memory_order_relaxed,
                std::memory_order_relaxed))
        {
            return true;
        }
    }

    return false;
}
```

Here the invariant concerns only one atomic object.

Therefore `relaxed` can be sufficient.

This demonstrates an important lesson:

```text
correct algorithm + relaxed
    can be correct

incorrect algorithm + seq_cst
    can still be wrong
```

Memory ordering cannot repair a logically non-atomic multi-step algorithm.

---

# 24. What is `std::memory_order_seq_cst`?

`std::memory_order_seq_cst` means **sequential consistency**.

It is the default memory order when no explicit order is supplied:

```cpp
std::atomic<int> counter{0};

counter.store(1);     // seq_cst
counter.load();       // seq_cst
counter.fetch_add(1); // seq_cst
```

For practical interview reasoning:

```text
seq_cst store -> release semantics
seq_cst load  -> acquire semantics
seq_cst RMW   -> acquire + release semantics
```

and additionally:

```text
all seq_cst operations participate in one global SC total order
```

That additional total order is the main semantic difference from ordinary release/acquire.

---

# 25. Store-buffering example

Initial state:

```cpp
std::atomic<int> x{0};
std::atomic<int> y{0};
```

Thread A:

```cpp
x.store(1, Order);
r1 = y.load(Order);
```

Thread B:

```cpp
y.store(1, Order);
r2 = x.load(Order);
```

Interesting result:

```text
r1 == 0
r2 == 0
```

With:

```cpp
constexpr auto Order = std::memory_order_relaxed;
```

the result is allowed.

The two atomics remain individually correct, but there is no global cross-object SC ordering.

---

# 26. Why `release/acquire` still does not forbid `0 / 0` here

Suppose:

```cpp
x.store(1, std::memory_order_release);
r1 = y.load(std::memory_order_acquire);
```

and:

```cpp
y.store(1, std::memory_order_release);
r2 = x.load(std::memory_order_acquire);
```

`r1 == 0 && r2 == 0` is still allowed.

Why?

Because an acquire operation synchronizes through the release only when it observes the corresponding released value / release sequence.

If both loads see the old `0`:

```text
A did not acquire B's release
B did not acquire A's release
```

Therefore no release/acquire cross-thread synchronization edge was formed.

This is a major interview distinction:

```text
release/acquire
    synchronization through actual communication

seq_cst
    release/acquire-style semantics
    +
    one global total order for SC operations
```

---

# 27. Why `seq_cst` forbids `0 / 0`

Use:

```cpp
x.store(1, std::memory_order_seq_cst);
r1 = y.load(std::memory_order_seq_cst);
```

and:

```cpp
y.store(1, std::memory_order_seq_cst);
r2 = x.load(std::memory_order_seq_cst);
```

Name the operations:

```text
A1 = x.store(1)
A2 = y.load()

B1 = y.store(1)
B2 = x.load()
```

Program order requires:

```text
A1 < A2
B1 < B2
```

For `A2` to read `0`, the SC order must place it before `B1`:

```text
A2 < B1
```

For `B2` to read `0`:

```text
B2 < A1
```

Together:

```text
A1 < A2 < B1 < B2 < A1
```

This is impossible.

Therefore the `0 / 0` result is forbidden when all four operations are `seq_cst`.

---

# 28. M3 Max / ARM64 experiment

A simple store-buffering experiment can be run repeatedly on Apple Silicon.

The outcome being *allowed* by the language or architecture does not mean it must appear on every run.

Use many iterations and avoid I/O inside the critical store/load pair.

Conceptual core:

```cpp
x.store(1, std::memory_order_relaxed);
r1 = y.load(std::memory_order_relaxed);
```

and in another thread:

```cpp
y.store(1, std::memory_order_relaxed);
r2 = x.load(std::memory_order_relaxed);
```

Then count:

```cpp
if (r1 == 0 && r2 == 0)
    ++bothZero;
```

Important:

```text
allowed != guaranteed to reproduce
```

Scheduling, compiler output, CPU state, and the exact test harness affect how frequently an allowed weak-memory outcome is observed.

---

# 29. `seq_cst` does not make multiple variables a transaction

Even this:

```cpp
player.balance.fetch_sub(10, std::memory_order_seq_cst);
player.totalBet.fetch_add(10, std::memory_order_seq_cst);
player.spins.fetch_add(1, std::memory_order_seq_cst);
```

still consists of three separate atomic operations.

Another thread can execute between them.

`seq_cst` gives strong ordering.

It does **not** provide:

```text
multi-object transaction
```

Use a mutex or another suitable state-transition design when several values form one invariant.

---

# 30. Cheat sheet

| Mechanism | Atomic operation | Blocks/waits | Orders other data | Mutual exclusion | Global SC order |
|---|---:|---:|---:|---:|---:|
| `load(relaxed)` | yes | no | no | no | no |
| `fetch_add(relaxed)` | yes, RMW | no | no | no | no |
| `store(release)` | yes | no | publishes prior operations | no | no |
| `load(acquire)` | yes | no | acquires published operations | no | no |
| `seq_cst` atomic | yes | no | yes | no | yes |
| `atomic::wait()` | yes | yes/may block | depends on chosen order | no | no |
| `mutex` | lock state is synchronized | lock may block | yes | yes | not an SC transaction |

---

# 31. Compact mental map

```text
std::atomic<T>
|
+-- load()
|      read one atomic value
|
+-- store()
|      write one atomic value
|
+-- fetch_add()/CAS/...
|      atomic read-modify-write
|
+-- wait()
|      wait while value is unchanged
|
+-- memory_order
       controls ordering/synchronization
       around the atomic operation
```

Memory orders:

```text
relaxed
    atomicity only

release
    publish operations before this point

acquire
    after observing the matching publication,
    order subsequent operations after it

acq_rel
    both directions for an RMW operation

seq_cst
    acquire/release guarantees
    +
    global SC total order
```

Mutex:

```text
lock
  |
  | arbitrary critical section
  |
unlock

same mutex:
unlock -> next successful lock
creates synchronization and visibility
```

---

# 32. Core points to remember

1. `memory_order_relaxed` does **not** weaken atomicity.
2. `fetch_add(relaxed)` does not lose increments.
3. `load()` never waits; `atomic::wait()` is a different operation.
4. `load() + store()` is not one atomic RMW operation.
5. An atomic counter can use `relaxed` when only the counter matters.
6. Seeing one relaxed atomic value does not automatically publish/order other memory locations.
7. `release/acquire` is primarily about establishing cross-thread ordering for other memory.
8. Producer writes must be before release; consumer reads must be after acquire.
9. One object/heap allocation/cache line does not automatically create synchronization.
10. A mutex gives both mutual exclusion and memory synchronization.
11. `unlock()` -> later successful `lock()` on the same mutex establishes the required visibility relation.
12. `seq_cst` additionally imposes one total order across SC atomic operations.
13. `seq_cst` does not make several independent operations one transaction.
14. Correct concurrency requires both a correct algorithm and an appropriate memory-ordering protocol.

---

# Interview answer in ~30 seconds

`std::atomic` guarantees atomic operations on the atomic object itself. `memory_order_relaxed` keeps that atomicity, so a relaxed `fetch_add()` still cannot lose increments, but it does not synchronize unrelated memory accesses. `release` and `acquire` are used when an atomic operation publishes other data: if an acquire load observes the value written by the corresponding release operation, writes before the release happen-before reads after the acquire. A mutex goes further by providing both mutual exclusion for a whole critical section and release/acquire-style synchronization between `unlock()` and the next successful `lock()`. `memory_order_seq_cst` is stronger still because SC operations also participate in one global total order, but it still does not turn multiple atomic operations into a transaction.
