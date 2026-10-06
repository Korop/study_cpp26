# C++26 Interview Study: `thread_local`

## 1. What is `thread_local`?

`thread_local` gives an object **thread storage duration**.

Each thread has its **own distinct instance** of the variable, and that instance normally lives until the thread terminates.

```cpp
#include <print>
#include <thread>

thread_local int counter = 0;

void work()
{
    ++counter;
    ++counter;

    std::println("thread = {}, counter = {}", std::this_thread::get_id(), counter);
}

int main()
{
    std::jthread t1{work};
    std::jthread t2{work};
}
```

Conceptually:

```text
Thread A -> counterA = 2
Thread B -> counterB = 2
```

The result is **not** one shared counter equal to `4`.

---

## 2. How does `thread_local` differ from automatic and `static` storage?

```cpp
void foo()
{
    int a = 0;
    static int b = 0;
    thread_local int c = 0;

    ++a;
    ++b;
    ++c;
}
```

| Declaration | Instances | Lifetime |
|---|---:|---|
| `int a` | one per function invocation | until scope exit |
| `static int b` | one shared object | program lifetime |
| `thread_local int c` | one object per thread | thread lifetime |

Mental model:

```text
automatic     -> one object / invocation
static        -> one object / program
thread_local  -> one object / thread
```

### Shared `static`

```text
Thread A ─┐
Thread B ─┼──> b
Thread C ─┘
```

Concurrent modification usually requires synchronization.

### `thread_local`

```text
Thread A -> cA
Thread B -> cB
Thread C -> cC
```

The threads modify different objects.

---

## 3. Function-local `thread_local`

A function-local `thread_local` survives repeated calls **within the same thread**.

```cpp
#include <print>

void process()
{
    thread_local int calls = 0;
    std::println("{}", ++calls);
}
```

If one thread calls:

```cpp
process();
process();
process();
```

it sees:

```text
1
2
3
```

Another thread starts with its own `calls`.

This makes it similar to function-local `static`, except the object is **per thread rather than shared by all threads**.

---

## 4. Where is a `thread_local` object stored?

Do not say:

> `thread_local` means the variable is stored on the thread stack.

The C++ language specifies **storage duration**, not a required physical memory region.

Implementations typically use a dedicated **TLS (Thread Local Storage)** area associated with each thread.

Conceptually:

```text
Process
├── global/static storage
├── heap
├── Thread A
│   ├── stack
│   └── TLS
│       └── thread_local objects
└── Thread B
    ├── stack
    └── TLS
        └── thread_local objects
```

### Interview answer

> `thread_local` does not mean stack or heap. It specifies thread storage duration. Implementations normally keep such objects in per-thread TLS storage.

---

## 5. Can `thread_local` contain class objects?

Yes.

```cpp
#include <print>

struct Context
{
    Context()  { std::println("Context()"); }
    ~Context() { std::println("~Context()"); }

    int requests = 0;
};

thread_local Context context;
```

Each thread gets its own `Context`.

Conceptually:

```text
Thread A: Context A
Thread B: Context B
Thread C: Context C
```

The object can have normal constructors and destructors.

A thread-local object is destroyed when its thread terminates.

---

## 6. Common use case: reusable per-thread buffer

Instead of repeatedly allocating:

```cpp
void process()
{
    std::vector<char> buffer(64 * 1024);
    // use buffer
}
```

a worker can reuse its own buffer:

```cpp
#include <vector>

void process()
{
    thread_local std::vector<char> buffer;
    buffer.clear();

    if (buffer.capacity() < 64 * 1024)
        buffer.reserve(64 * 1024);

    // use buffer
}
```

This can reduce:

- repeated allocation,
- allocator contention,
- synchronization,
- cache-line contention caused by shared state.

Each worker owns its own buffer.

---

## 7. Common use case: per-thread random generator

A shared PRNG would normally require synchronization.

```cpp
#include <random>

int random_value()
{
    thread_local std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> distribution{1, 100};

    return distribution(rng);
}
```

Each thread has an independent generator.

Be careful with identical deterministic seeds:

```cpp
thread_local std::mt19937 rng{42};
```

Every thread then starts with the same pseudo-random sequence.

---

## 8. Common use case: local counters and metrics

A global atomic counter:

```cpp
std::atomic<std::uint64_t> total{0};

void process()
{
    total.fetch_add(1, std::memory_order_relaxed);
}
```

is shared by all threads.

For a very hot path, a program may instead collect per-thread values:

```cpp
thread_local std::uint64_t local_count = 0;

void process()
{
    ++local_count;
}
```

This removes synchronization from the hot increment path.

However:

> `thread_local` does not automatically produce a global total.

The program still needs an aggregation mechanism.

Typical uses:

- metrics,
- profilers,
- allocators,
- logging,
- server worker statistics,
- caches.

---

## 9. Static data members

A class can have a thread-local static member.

```cpp
class Worker
{
public:
    inline static thread_local int current_task = 0;
};
```

`current_task` belongs to the class, but every thread gets a separate instance.

```text
Thread A -> Worker::current_task A
Thread B -> Worker::current_task B
```

It is **not** one value per `Worker` object.

---

## 10. `static thread_local`

These specifiers solve different problems.

```cpp
static thread_local int counter = 0;
```

At namespace scope:

- `thread_local` -> one instance per thread,
- `static` -> internal linkage, visible only inside that translation unit.

So `static` here does **not** change thread lifetime into program lifetime.

---

## 11. `extern thread_local`

A thread-local variable can be declared in a header and defined in one source file.

```cpp
// context.hpp
extern thread_local int current_request;
```

```cpp
// context.cpp
thread_local int current_request = 0;
```

All translation units refer to the same logical TLS variable, but every thread still gets its own instance.

---

## 12. Header-defined `inline thread_local`

Modern C++ can define a TLS variable directly in a header:

```cpp
namespace telemetry
{
    inline thread_local std::uint64_t events = 0;
}
```

The two keywords address different concerns:

```text
inline       -> ODR / multiple translation units
thread_local -> one instance per thread
```

---

## 13. Important trap: a thread-local pointer does not make the pointed object thread-local

```cpp
int global = 0;

thread_local int* ptr = &global;
```

Each thread owns a different pointer object:

```text
Thread A -> ptrA ─┐
Thread B -> ptrB ─┼──> global
Thread C -> ptrC ─┘
```

But all pointers still refer to the same `global`.

Therefore:

```cpp
++*ptr;
```

can still cause a data race.

The same applies to:

```cpp
thread_local std::shared_ptr<State> state;
```

The `shared_ptr` object is thread-local. The `State` object may still be shared.

---

## 14. Is `thread_local` automatically thread-safe?

A better answer than simply "yes":

> Each thread has a separate instance of the thread-local object, so different threads do not race on that object merely by accessing the same declared name. Objects referenced from it may still be shared and require synchronization.

Example:

```cpp
struct SharedState
{
    int value = 0;
};

SharedState shared;

thread_local SharedState* state = &shared;
```

`state` is per-thread.

`shared` is not.

---

## 15. Thread pools: the important lifetime trap

`thread_local` belongs to the **physical thread**, not to a job or request.

```cpp
thread_local int user_id = 0;

void handle_request(int id)
{
    user_id = id;
    // ...
}
```

Imagine a thread pool:

```text
Worker 1 handles Request A
user_id = 42

Request A ends

Worker 1 later handles Request B
user_id is still 42 unless overwritten/reset
```

The worker thread survived, therefore its TLS state survived.

So:

```text
thread_local lifetime == thread lifetime
```

not:

```text
request lifetime
task lifetime
job lifetime
```

This is one of the most important practical limitations.

---

## 16. Coroutines and asynchronous execution

A coroutine or logical task can resume on another physical thread:

```text
Thread A
   |
   | suspend / await
   v
Thread B
```

If it accesses:

```cpp
thread_local RequestContext context;
```

then before suspension it sees Thread A's `context`, while after migration it may see Thread B's `context`.

Therefore `thread_local` is usually unsuitable for data whose identity belongs to:

- a request,
- coroutine,
- async operation,
- logical execution context.

It belongs to the **thread**.

---

## 17. Memory cost

Memory usage grows roughly with:

```text
TLS object size × number of threads
```

Example:

```cpp
thread_local std::array<std::byte, 1024 * 1024> buffer;
```

Approximately:

```text
1 MiB × 100 threads ≈ 100 MiB
```

before considering other overhead.

Large TLS objects can therefore become expensive in large thread pools.

---

## 18. Initialization and destruction

Simple TLS:

```cpp
thread_local int value = 10;
```

gives each thread its own initialized object.

For a block-scope thread-local object:

```cpp
Cache& cache()
{
    thread_local Cache instance;
    return instance;
}
```

each thread constructs its own `Cache` when that thread reaches the declaration and initialization is required.

Each constructed instance is later destroyed when that thread exits.

This is useful for RAII, but destructors should be designed carefully if they depend on other long-lived global objects during shutdown.

---

# `thread_local` vs C# thread-local mechanisms

## 19. Is C++ `thread_local` similar to C# `ThreadLocal<T>`?

Yes. This is the closest general mental mapping:

```text
C++ thread_local T x
        ≈
C# ThreadLocal<T>
```

### C++

```cpp
thread_local int counter = 0;

void work()
{
    ++counter;
}
```

### C#

```csharp
static ThreadLocal<int> Counter = new(() => 0);

static void Work()
{
    Counter.Value++;
}
```

Both provide different values for different physical threads.

The API differs:

```text
C++: variable itself has thread storage duration
C#:  ThreadLocal<T> is an object that manages per-thread values
```

---

## 20. C# `[ThreadStatic]`

C# also has:

```csharp
[ThreadStatic]
static int counter;
```

This is conceptually very close to:

```cpp
thread_local int counter;
```

However, C# has an important initialization trap:

```csharp
[ThreadStatic]
static int counter = 10;
```

The initializer is part of static type initialization and does not mean that every thread gets an independently initialized value of `10`.

`ThreadLocal<T>` is clearer when per-thread initialization is required:

```csharp
static ThreadLocal<int> Counter = new(() => 10);
```

C++:

```cpp
thread_local int counter = 10;
```

initializes each thread-local instance according to the TLS initialization rules.

---

## 21. `AsyncLocal<T>` is NOT equivalent

C#:

```csharp
AsyncLocal<T>
```

belongs to a **logical asynchronous execution context**, not simply to an OS thread.

Conceptually:

```text
C++ thread_local      -> physical thread
C# ThreadLocal<T>     -> physical thread
C# [ThreadStatic]     -> physical thread
C# AsyncLocal<T>      -> logical async context
```

This matters around `await`.

```csharp
static AsyncLocal<int> UserId = new();

static async Task Foo()
{
    UserId.Value = 42;

    await SomethingAsync();

    Console.WriteLine(UserId.Value);
}
```

The continuation may resume on another worker thread while the logical async context still carries the value.

By contrast, `ThreadLocal<T>` or C++ `thread_local` follows the physical thread.

---

# When should `thread_local` be used?

Good candidates are objects that are:

```text
frequently accessed
+
expensive or unsafe to share
+
naturally owned by a physical thread
+
long-lived
```

Typical examples:

- scratch buffers,
- parsers,
- random generators,
- caches,
- allocators,
- profiler state,
- metrics,
- logging state,
- low-level runtime state.

Avoid using TLS merely to hide ownership problems or pass request state implicitly through a program.

---

# Core points to remember

1. `thread_local` means **one instance per thread**.
2. Its lifetime is tied to the **thread**, not a function call or request.
3. It is not necessarily stored on the thread stack.
4. Implementations normally use dedicated TLS storage.
5. Class objects can be thread-local and use normal RAII.
6. Thread-local state usually requires no cross-thread synchronization because the objects are distinct.
7. A pointer being thread-local does not make the pointed-to object thread-local.
8. Thread pools reuse TLS state.
9. Coroutine/task execution may migrate between threads.
10. TLS memory cost grows with the number of threads.
11. `static thread_local` combines internal linkage with thread storage duration.
12. `inline thread_local` is useful for header-defined TLS variables.
13. Closest C# equivalent: `ThreadLocal<T>` / `[ThreadStatic]`.
14. C# `AsyncLocal<T>` has different semantics: logical async context rather than physical thread.

---

# Compact mental map

```text
int x;
  local variable
  one object / invocation

static int x;
  one shared object / program

thread_local int x;
  one object / physical thread

static thread_local int x;
  one object / physical thread
  + internal linkage at namespace scope

inline thread_local int x;
  one object / physical thread
  + header-friendly definition across translation units
```

---

# Interview answer in ~30 seconds

> `thread_local` is a C++ storage-class specifier that gives an object thread storage duration. Each physical thread has its own distinct instance, which normally lives until that thread terminates. It is useful for per-thread caches, scratch buffers, RNGs, allocators and hot-path counters because independent instances can avoid synchronization. The main caveats are memory cost, hidden state, thread-pool reuse, and async or coroutine code because TLS belongs to a physical thread rather than a logical task. In C#, the closest equivalents are `ThreadLocal<T>` and `[ThreadStatic]`; `AsyncLocal<T>` is different because it follows logical async context.

---

# Cheat Sheet

```cpp
// Global TLS
thread_local int counter = 0;

// Function-local TLS
void foo()
{
    thread_local Cache cache;
}

// Class TLS member
class Worker
{
public:
    inline static thread_local int current_task = 0;
};

// Header declaration
extern thread_local int current_request;

// Source definition
thread_local int current_request = 0;

// Header-only namespace variable
inline thread_local std::uint64_t events = 0;
```

```text
Best uses:
  cache / scratch buffer / RNG / allocator / metrics

Main traps:
  thread pool reuse
  async task migration
  large memory footprint
  hidden dependencies
  thread-local pointer -> shared pointee can still race
```
