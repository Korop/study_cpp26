# C++26 Interview Study: `std::stop_token`, `std::jthread`, cancellation and related template mechanics

> The core stop-token API (`std::stop_token`, `std::stop_source`, `std::stop_callback`, `std::jthread`) was introduced in C++20.  
> This note studies it from a modern C++26 interview perspective and also covers related C++23/C++26 language mechanics used in the library wording.

---

## 1. What problem does `std::stop_token` solve?

`std::stop_token` provides **cooperative cancellation**.

It does not terminate a thread. It allows one part of the program to request cancellation and another part to observe that request.

Mental model:

```text
std::stop_source  --->  shared stop-state  <---  std::stop_token
   request_stop()          requested?              stop_requested()
```

- `std::stop_source` is the **controller / writer side**.
- `std::stop_token` is the **observer / reader side**.
- Several tokens can refer to the same stop-state.
- A stop request is one-way: once requested, it remains requested.

Example:

```cpp
#include <stop_token>

int main()
{
    std::stop_source source;

    const std::stop_token t1 = source.get_token();
    const std::stop_token t2 = source.get_token();

    source.request_stop();

    return !(t1.stop_requested() && t2.stop_requested());
}
```

---

## 2. How does `std::jthread` inject a `std::stop_token`?

Given:

```cpp
void Worker(std::stop_token token)
{
    while (!token.stop_requested())
    {
        // work
    }
}

int main()
{
    std::jthread worker{Worker};
    worker.request_stop();
}
```

`std::jthread` owns an internal stop source / stop-state.

Conceptually:

```text
std::jthread
    |
    +-- internal std::stop_source
            |
            +-- shared stop-state
                    |
                    +-- std::stop_token passed to Worker
```

The constructor checks whether the callable can be invoked with an injected `std::stop_token` as its first argument.

Conceptually:

```cpp
if constexpr (
    std::is_invocable_v<
        std::decay_t<F>,
        std::stop_token,
        std::decay_t<Args>...
    >
)
{
    // invoke callable with stop_token first
}
else
{
    // invoke callable without stop_token
}
```

For:

```cpp
void Worker(std::stop_token, int);

std::jthread t{Worker, 42};
```

the effective invocation is conceptually:

```cpp
Worker(t.get_stop_token(), 42);
```

The `std::stop_token` is **not part of `Args...`**. It is injected by `std::jthread`.

---

## 3. Does `std::stop_token` work with `std::thread`?

Yes.

`std::stop_token` is not conceptually tied to `std::jthread`.

With `std::thread`, create the stop-state yourself:

```cpp
#include <chrono>
#include <print>
#include <stop_token>
#include <thread>

using namespace std::chrono_literals;

void Worker(std::stop_token token)
{
    int iteration{};

    while (!token.stop_requested())
    {
        std::println("Work {}", iteration++);
        std::this_thread::sleep_for(100ms);
    }

    std::println("Worker stopped");
}

int main()
{
    std::stop_source source;

    std::thread worker{Worker, source.get_token()};

    std::this_thread::sleep_for(500ms);

    source.request_stop();
    worker.join();
}
```

Difference:

```text
std::thread:
    manual stop_source
    explicit token passing
    explicit join

std::jthread:
    internal stop_source
    optional automatic token injection
    automatic join in destructor
```

---

## 4. Can one stop request stop several threads?

Yes.

Several threads can observe tokens connected to the same stop-state.

```cpp
#include <chrono>
#include <print>
#include <stop_token>
#include <thread>

using namespace std::chrono_literals;

void Worker(std::stop_token token, int id)
{
    while (!token.stop_requested())
    {
        std::println("Worker {} running", id);
        std::this_thread::sleep_for(100ms);
    }

    std::println("Worker {} stopped", id);
}

int main()
{
    std::stop_source source;
    const auto token = source.get_token();

    std::thread t1{Worker, token, 1};
    std::thread t2{Worker, token, 2};
    std::thread t3{Worker, token, 3};

    std::this_thread::sleep_for(500ms);
    source.request_stop();

    t1.join();
    t2.join();
    t3.join();
}
```

Mental model:

```text
                 shared stop-state
                       ^
                       |
                std::stop_source
                       |
                  request_stop()
                       |
       +---------------+---------------+
       |               |               |
     token           token           token
       |               |               |
    thread 1        thread 2        thread 3
```

---

## 5. Important: `request_stop()` does not terminate the thread

Cancellation is cooperative.

This works:

```cpp
while (!token.stop_requested())
{
    DoWork();
}
```

This ignores cancellation:

```cpp
void Worker(std::stop_token)
{
    while (true)
    {
        DoWork();
    }
}
```

`request_stop()` only changes the stop-state. The worker must observe it.

---

## 6. `std::jthread` destructor

A useful conceptual model:

```cpp
if (joinable())
{
    request_stop();
    join();
}
```

Therefore:

```cpp
{
    std::jthread worker{Worker};
} // requests stop and joins
```

This is one major difference from `std::thread`.

---

# 7. Why use `std::condition_variable_any` with `std::stop_token`?

Polling:

```cpp
while (!token.stop_requested())
{
    std::this_thread::sleep_for(std::chrono::seconds{10});
}
```

can react to cancellation very late.

A stop-aware wait can sleep efficiently and wake when:

- useful shared state changes, or
- stop is requested.

Example signature:

```cpp
cv.wait(lock, token, predicate);
```

---

## 8. Why are `std::mutex` and `std::unique_lock` needed?

A condition variable normally waits for a condition involving **shared state** protected by a mutex.

Example:

```cpp
std::queue<int> jobs;
std::mutex jobsMutex;
std::condition_variable_any jobsCv;
```

The mutex protects the queue.

`std::unique_lock` is used because `wait()` must be able to:

```text
check predicate
unlock mutex
sleep
wake
lock mutex again
check predicate again
```

`std::lock_guard` cannot be temporarily unlocked/relocked in this way.

Important:

> The mutex protects the shared application state, not the `std::stop_token`.

---

## 9. What happens while `jobsCv.wait(...)` is waiting?

Suppose:

```cpp
jobs.empty() == true;
```

and Worker executes:

```cpp
const bool hasJob = jobsCv.wait(
    lock,
    token,
    []
    {
        return !jobs.empty();
    });
```

The worker remains **inside the `wait()` call**.

Conceptually:

```text
Worker already owns mutex
        |
        v
call wait(...)
        |
        v
predicate == false
        |
        v
unlock mutex
        |
        v
thread blocks / sleeps
        |
        |   still inside wait(...)
        |
   notify / stop / spurious wakeup
        |
        v
lock mutex again
        |
        v
check predicate again
```

The line after `wait()` is executed only when `wait()` returns.

---

# 10. Complete producer/consumer example

Here `main()` is the producer and `Worker()` is the consumer.

```cpp
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <print>
#include <queue>
#include <stop_token>
#include <thread>

using namespace std::chrono_literals;

std::queue<int> jobs;
std::mutex jobsMutex;
std::condition_variable_any jobsCv;

void Worker(std::stop_token token)
{
    while (true)
    {
        std::unique_lock lock{jobsMutex};

        const bool hasJob = jobsCv.wait(
            lock,
            token,
            []
            {
                return !jobs.empty();
            });

        if (!hasJob)
        {
            std::println("[Worker] Stop requested");
            return;
        }

        const int job = jobs.front();
        jobs.pop();

        // Keep the critical section small.
        lock.unlock();

        std::println("[Worker] Processing {}", job);
        std::this_thread::sleep_for(300ms);
        std::println("[Worker] Finished {}", job);
    }
}

int main()
{
    std::jthread worker{Worker};

    for (int job = 1; job <= 5; ++job)
    {
        {
            std::lock_guard lock{jobsMutex};
            jobs.push(job);
            std::println("[Main] Added {}", job);
        }

        jobsCv.notify_one();
        std::this_thread::sleep_for(100ms);
    }

    std::this_thread::sleep_for(2s);

    worker.request_stop();
    worker.join();
}
```

---

## 11. Producer/consumer execution flow

Producer:

```text
lock mutex
    |
jobs.push()
    |
unlock mutex
    |
notify_one()
```

Consumer:

```text
lock mutex
    |
predicate: jobs available?
    |
    +-- yes --> pop job --> unlock --> process
    |
    +-- no --> unlock --> sleep
                         |
                    notify / stop
                         |
                       wake
                         |
                     lock again
```

The mutex is intentionally released while the worker is:

- sleeping inside `wait()`;
- processing the job.

Otherwise the producer would be blocked unnecessarily.

---

## 12. Why `notify_one()` is usually done after unlocking

Preferred pattern:

```cpp
{
    std::lock_guard lock{jobsMutex};
    jobs.push(42);
}

jobsCv.notify_one();
```

This avoids waking a worker only for it to immediately block again on the still-owned mutex.

---

## 13. Spurious wakeups and predicates

A condition variable may wake even when no useful condition became true.

Therefore avoid:

```cpp
cv.wait(lock);
auto job = jobs.front();
```

Prefer:

```cpp
cv.wait(lock, []
{
    return !jobs.empty();
});
```

Conceptually:

```cpp
while (!predicate())
{
    wait();
}
```

With a stop token there is an additional exit reason: cancellation.

---

# 14. `std::jthread` constructor template and `Args...`

Conceptually:

```cpp
template<class F, class... Args>
explicit jthread(F&& f, Args&&... args);
```

`Args...` is a **template parameter pack**, not an array.

Example:

```cpp
std::jthread t{
    Worker,
    42,
    name,
    std::move(data)
};
```

Conceptually the compiler deduces independent parameter types for each supplied argument.

Pack expansion:

```cpp
std::forward<Args>(args)...
```

means:

> Repeat `std::forward<Arg_i>(arg_i)` for every argument in the pack.

---

# 15. What does `auto(std::forward<Args>(args))...` mean?

Modern standard wording uses expressions conceptually like:

```cpp
auto(std::forward<Args>(args))...
```

The repeated pattern is:

```cpp
auto(std::forward<Arg_i>(arg_i))
```

Each expanded argument gets its **own independent `auto` deduction**.

Example:

```cpp
Args... = <int&, std::string, Widget&>
```

expands conceptually to:

```cpp
auto(std::forward<int&>(arg0)),
auto(std::forward<std::string>(arg1)),
auto(std::forward<Widget&>(arg2))
```

There is no single common `auto` type.

---

## 16. Does `auto(int&&)` produce `int&&`?

No.

Plain `auto(...)` performs value deduction.

Examples:

```cpp
int x = 42;

auto a = auto(x);
auto b = auto(std::move(x));

static_assert(std::is_same_v<decltype(a), int>);
static_assert(std::is_same_v<decltype(b), int>);
```

Similarly:

```cpp
std::string name = "Ivan";

auto a = auto(name);            // copy
auto b = auto(std::move(name)); // move

static_assert(std::is_same_v<decltype(a), std::string>);
static_assert(std::is_same_v<decltype(b), std::string>);
```

Mental model:

```text
std::forward
    preserves value category
        |
        +-- lvalue
        +-- rvalue/xvalue

auto(...)
    materializes an independent value
        |
        +-- copy from lvalue
        +-- move from rvalue
```

---

## 17. Why both `std::forward` and `auto(...)`?

They solve different problems.

`std::forward<T>(x)`:

```text
preserve original value category
```

`auto(...)`:

```text
create a decayed/materialized value
```

Together:

```cpp
auto(std::forward<Args>(args))
```

means conceptually:

> Preserve whether the caller supplied an lvalue or rvalue, then construct an independent value from it.

This lets thread arguments be copied or moved into thread-owned storage.

---

## 18. Why not keep references automatically?

For:

```cpp
std::string name = "Ivan";
std::jthread t{Worker, name};
```

the thread normally gets its own copied value.

If a real reference is required, make that explicit:

```cpp
std::jthread t{Worker, std::ref(name)};
```

Now the stored value is a `std::reference_wrapper<std::string>`.

This makes lifetime and shared-state decisions explicit.

---

# 19. Compile-time detection of the `stop_token` overload

Conceptual code:

```cpp
if constexpr (
    std::is_invocable_v<
        std::decay_t<F>,
        std::stop_token,
        std::decay_t<Args>...
    >
)
{
    // invoke with stop_token
}
else
{
    // invoke without stop_token
}
```

For:

```cpp
void Worker(std::stop_token, int);
std::jthread t{Worker, 42};
```

the compiler asks, during template instantiation:

```text
Would Worker(std::stop_token, int) be a valid invocation?
```

No function is actually executed during compilation.

`std::is_invocable_v<...>` performs a compile-time validity test.

---

## 20. When is `if constexpr` evaluated?

The template is first parsed without concrete `F` and `Args...`.

When the compiler sees:

```cpp
std::jthread t{Worker, 42};
```

it conceptually performs:

```text
1. deduce F and Args...
2. instantiate the matching constructor template
3. evaluate std::is_invocable_v<...>
4. select the matching if constexpr branch
5. discard the other branch for this instantiation
```

This discarded-branch behavior is the crucial difference from a normal runtime `if`.

---

## 21. `if constexpr` example

```cpp
#include <type_traits>

template<class T>
constexpr int Process(T value)
{
    if constexpr (std::is_integral_v<T>)
    {
        return value + 1;
    }
    else
    {
        return static_cast<int>(value);
    }
}

static_assert(Process(41) == 42);
```

`if constexpr` means:

> Choose a branch based on a compile-time condition and discard the other branch for this template instantiation.

---

# 22. `constexpr` vs `consteval`

Compact rule:

```text
constexpr -> CAN be evaluated at compile time
consteval -> MUST be evaluated at compile time
```

Example:

```cpp
constexpr int Square(int x)
{
    return x * x;
}

consteval int Cube(int x)
{
    return x * x * x;
}

static_assert(Square(4) == 16);
static_assert(Cube(3) == 27);

int RuntimeSquare(int x)
{
    return Square(x); // valid runtime use
}
```

This is invalid when `x` is runtime data:

```cpp
// int RuntimeCube(int x)
// {
//     return Cube(x); // error
// }
```

`consteval` applies to functions; it means every potentially-evaluated call must produce a constant expression.

---

# 23. Can `consteval` replace `if constexpr` in `jthread`?

No.

They solve different problems.

```text
if constexpr
    -> compile-time branch selection

consteval
    -> force a function call to be evaluated at compile time
```

One could wrap the trait:

```cpp
template<class F, class... Args>
consteval bool AcceptsStopToken()
{
    return std::is_invocable_v<
        std::decay_t<F>,
        std::stop_token,
        std::decay_t<Args>...
    >;
}
```

and use:

```cpp
if constexpr (AcceptsStopToken<F, Args...>())
{
    // ...
}
```

but it adds no useful behavior. `std::is_invocable_v` is already a compile-time constant.

A `std::jthread` constructor itself cannot meaningfully be `consteval`, because starting an OS thread is runtime work.

---

# 24. `if consteval`

`if consteval` is valid since C++23.

It asks:

> Is this invocation currently being evaluated as part of constant evaluation?

Example:

```cpp
#include <print>

constexpr int Square(int x)
{
    if consteval
    {
        return x * x;
    }
    else
    {
        std::println("Runtime Square({})", x);
        return x * x;
    }
}

int main()
{
    constexpr int a = Square(4); // consteval branch

    int x = 5;
    const int b = Square(x);     // runtime branch

    std::println("{} {}", a, b);
}
```

Do not confuse:

```cpp
if constexpr (condition)
```

with:

```cpp
if consteval
```

Difference:

```text
if constexpr
    -> Which branch is valid for this compile-time condition/type?

if consteval
    -> Is this particular evaluation happening at compile time?
```

---

# 25. `if consteval` with an immediate (`consteval`) function

```cpp
consteval int CompileTimeSquare(int x)
{
    return x * x;
}

constexpr int Square(int x)
{
    if consteval
    {
        return CompileTimeSquare(x);
    }
    else
    {
        return x * x;
    }
}

static_assert(Square(4) == 16);
```

The `consteval` branch is used during constant evaluation; the runtime branch remains available for ordinary calls.

---

# Interview answer in ~30 seconds

`std::stop_token` is a cooperative cancellation observer connected to a shared stop-state; `std::stop_source` requests cancellation. `std::jthread` owns its own stop source and can automatically inject its token as the first callable argument when `std::is_invocable_v` says that signature is valid. The decision is made at template-instantiation time with `if constexpr`.

`std::condition_variable_any::wait(lock, token, predicate)` can efficiently block while releasing the mutex, then reacquire it and re-check the predicate after notification, spurious wakeup, or stop request.

Modern `jthread` wording uses patterns like `auto(std::forward<Args>(args))...`: `std::forward` preserves lvalue/rvalue category, while `auto(...)` materializes an independent value, causing a copy from lvalues or a move from rvalues.

---

# Compact mental map

```text
Cancellation:
    stop_source -> shared stop-state <- stop_token

jthread:
    thread + internal stop_source + RAII join

Producer/consumer:
    mutex              -> protects queue
    condition_variable -> sleep/wake
    stop_token         -> cancellation

Templates:
    Args...                        -> parameter pack
    std::forward<Args>(args)...   -> preserve value category
    auto(...)                     -> materialize value
    if constexpr                  -> compile-time branch selection
    std::is_invocable_v           -> compile-time invocation validity

Constant evaluation:
    constexpr  -> may run at compile time
    consteval  -> must run at compile time
    if consteval -> detect current constant-evaluation context
```

---

# Core points to remember

1. `std::stop_token` does not kill a thread; the worker must cooperate.
2. `std::jthread` can inject `std::stop_token` as the first callable parameter.
3. `std::stop_token` can also be used explicitly with `std::thread`.
4. One shared stop-state can cancel several threads.
5. `condition_variable_any::wait()` remains blocked inside the function until its wait completes.
6. `std::unique_lock` is needed because `wait()` must unlock and re-lock the mutex.
7. Keep expensive work outside the mutex-protected critical section.
8. `auto(std::forward<T>(x))` produces a value, not `T&&`.
9. `if constexpr` and `std::is_invocable_v` make the `jthread` token-injection decision at compile time.
10. `constexpr`, `consteval`, and `if consteval` are related to constant evaluation but solve different problems.
