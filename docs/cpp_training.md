# C++ study tracker

A study path from C++11 to C++26, with C# comparisons and practice in numerical programming, embedded systems and algorithms.

## Current focus

Continue the core lessons at **“20. Why `string_view` is useful”**, then read sections **21–28**.

The next small step is to explain when a `string_view` remains valid and try a short example. Then review ownership, copy/move and type deduction before moving on to concepts and ranges.

[Topics](#topics) · [Practice](#practice) · [Review routine](#review-routine) · [Study log](#study-log) · [References](#references)

## Using the tracker

- **Pending:** not started, or reading is unconfirmed.
- **Read:** read at least once; still needs practice.
- **InProgress:** currently studying or practising.
- **Done:** can explain and apply independently, and can do so again at least a week later.

**Difficulty:** 1 = small concept, 2 = routine, 3 = needs practice, 4 = subtle, 5 = advanced. Adjust these estimates as you learn.

**Rereads** counts return visits to the explanation. **Practice** counts sessions spent recalling or applying the topic; record how they went in the log. Count a session once per topic and activity, not once per question or program run. A practice count alone does not mean the topic is learned.

Counts are **tracked since 2026-09-22**. Earlier repetition counts are **Unknown**; the zeros below cover only new, logged sessions. Existing reading is preserved, while example files alone do not establish completion.

<a id="topics"></a>
## Topics

Read statuses cover the material already encountered, including the existing notes. A grouped topic may still have newer details to learn. Years identify the relevant standard or later additions; older foundations are included for review.

### Core language

| Topic | Status | Difficulty | Rereads | Practice |
|---|---|---|---|---|
| <a id="c01"></a>Classes, values, `const`, virtual functions, `override` and `final` (C++11 additions) | Read | 2/5 | 0 | 0 |
| <a id="c02"></a>[Initialization, scope and member order](cpp/designated_initializers_scope_shadowing_interview.md) (C++11/20) | Read | 3/5 | 0 | 0 |
| <a id="c03"></a>[RAII and smart pointers](cpp/raii_smart_pointers_interview.md): ownership and resource lifetime (C++11/14) | Read | 3/5 | 0 | 0 |
| <a id="c04"></a>[Copy, move and special member functions](cpp/copy_move_traits_interview.md) (C++11/17) | Read | 4/5 | 0 | 0 |
| <a id="c05"></a>References, value categories and perfect forwarding (C++11) | Read | 4/5 | 0 | 0 |
| <a id="c06"></a>[Type deduction](cpp/auto_deduction.md): `auto`, `decltype` and `decltype(auto)` (C++11/14) | Read | 3/5 | 0 | 0 |
| <a id="c07"></a>Lambdas, captures and callable objects (C++11/14/17) | Read | 3/5 | 0 | 0 |
| <a id="c08"></a>[Using declarations and aliases](cpp/using_interview.md) (C++11/20 additions) | Read | 2/5 | 0 | 0 |
| <a id="c09"></a>Templates, specialization, parameter packs and non-type parameters (C++11/17/20 additions) | Read | 3/5 | 0 | 0 |
| <a id="c10"></a>Constant evaluation: `constexpr`, `consteval`, [constinit](cpp/cpp26_constinit_interview.md) (C++11/14/20/23) | Read | 3/5 | 0 | 0 |
| <a id="c11"></a>Fold expressions and `if constexpr` (C++17) | Read | 3/5 | 0 | 0 |
| <a id="c12"></a>[Class template deduction and structured bindings](cpp/cpp26_structured_bindings_and_matching.md) (C++17) | Read | 2/5 | 0 | 0 |
| <a id="c13"></a>Type traits, type transformations and SFINAE (C++11/14/17/20) | Read | 3/5 | 0 | 0 |

### Library and everyday C++

| Topic | Status | Difficulty | Rereads | Practice |
|---|---|---|---|---|
| <a id="c14"></a>`optional` (C++17); monadic operations still to read (C++23) | Read | 2/5 | 0 | 0 |
| <a id="c15"></a>[Variant and visit](cpp/std_variant_visit_interview.md) (C++17; member `visit` in C++26) | Read | 3/5 | 0 | 0 |
| <a id="c16"></a>**Strings and `string_view` — current topic** (`string_view`: C++17) | InProgress | 3/5 | 0 | 0 |
| <a id="c17"></a>Sequential containers and invalidation: vector, array, list, [deque](cpp/deque_interview.md) | Read | 3/5 | 0 | 0 |
| <a id="c18"></a>Maps, sets, hashing and container adaptors (unordered containers: C++11) | Read | 3/5 | 0 | 0 |
| <a id="c19"></a>Iterators, algorithms, numeric algorithms and complexity | Pending | 3/5 | 0 | 0 |
| <a id="c20"></a>[Casts](cpp/casts_interview.md), object representation, alignment and undefined behaviour | Read | 4/5 | 0 | 0 |
| <a id="c21"></a>Exceptions, error handling and `noexcept` (C++11) | Pending | 3/5 | 0 | 0 |
| <a id="c22"></a>`span` and borrowed buffers (C++20) | Pending | 3/5 | 0 | 0 |
| <a id="c23"></a>Concepts and `requires` (C++20) | Pending | 4/5 | 0 | 0 |
| <a id="c24"></a>Ranges, views and lazy evaluation (C++20/23) | Pending | 4/5 | 0 | 0 |
| <a id="c25"></a>Filesystem and text conversion (C++17), `format` (C++20), `print` (C++23) | Pending | 2/5 | 0 | 0 |
| <a id="c26"></a>[Coroutines and generators](cpp/yield_interview.md) (language: C++20; `generator`: C++23) | Read | 4/5 | 0 | 0 |
| <a id="c27"></a>`co_await`, suspension, lifetime and cancellation (C++20) | Pending | 5/5 | 0 | 0 |
| <a id="c28"></a>Headers, linkage, build boundaries and modules (modules: C++20) | Pending | 3/5 | 0 | 0 |
| <a id="c29"></a>`expected` and explicit error results (C++23) | Pending | 3/5 | 0 | 0 |
| <a id="c30"></a>Other modern APIs: `any`, attributes, comparisons, chrono/bit, `scope_exit` and callable wrappers | Pending | 3/5 | 0 | 0 |
| <a id="c31"></a>CMake, warnings, tests, debugging and sanitizers | Pending | 3/5 | 0 | 0 |

### Numerical and parallel programming

| Topic | Status | Difficulty | Rereads | Practice |
|---|---|---|---|---|
| <a id="n01"></a>Integer overflow, floating-point error and stable accumulation | Pending | 4/5 | 0 | 0 |
| <a id="n02"></a>Random numbers, sampling, RTP, variance and uncertainty (`random`: C++11) | Pending | 4/5 | 0 | 0 |
| <a id="n03"></a>Threads, mutexes, futures and coordination (C++11); `jthread` and cancellation (C++20) | Pending | 4/5 | 0 | 0 |
| <a id="n04"></a>Atomics and memory ordering (C++11); wait/notify (C++20) | Pending | 5/5 | 0 | 0 |
| <a id="n05"></a>Parallel algorithms, batches and reductions (C++17/20) | Pending | 4/5 | 0 | 0 |
| <a id="n06"></a>Profiling, benchmarks and scaling | Pending | 4/5 | 0 | 0 |

### Embedded programming

| Topic | Status | Difficulty | Rereads | Practice |
|---|---|---|---|---|
| <a id="b01"></a>YD-RP2040: Pico SDK, cross-compilation, flashing and C interoperability | Pending | 3/5 | 0 | 0 |
| <a id="b02"></a>GPIO, timers, registers and nonblocking state machines | Pending | 3/5 | 0 | 0 |
| <a id="b03"></a>Interrupts, bounded buffers, DMA and PIO | Pending | 4/5 | 0 | 0 |
| <a id="b04"></a>Multicore coordination, timing budgets and RTOS basics | Pending | 5/5 | 0 | 0 |

### After the core-language reading

| Topic | Status | Difficulty | Rereads | Practice |
|---|---|---|---|---|
| <a id="a01"></a>Static polymorphism: constrained templates, policies, CRTP and explicit object parameters (C++23) | Pending | 4/5 | 0 | 0 |
| <a id="a02"></a>L1/L2/L3, locality, working sets, layout, blocking, alignment and false sharing | Pending | 4/5 | 0 | 0 |
| <a id="a03"></a>Arenas, fixed-size pools and `pmr` (C++17): lifetime, fragmentation, retained memory and exhaustion | Pending | 4/5 | 0 | 0 |
| <a id="a04"></a>Numerical accuracy, conditioning, dense/sparse storage, `mdspan` (C++23) and `linalg` (C++26) | Pending | 5/5 | 0 | 0 |
| <a id="a05"></a>Auto-vectorization and SIMD (standard library: C++26) | Pending | 4/5 | 0 | 0 |
| <a id="a06"></a>Expression templates, temporary elimination, aliasing and lifetime | Pending | 5/5 | 0 | 0 |

### C++26 additions

| Topic | Status | Difficulty | Rereads | Practice |
|---|---|---|---|---|
| <a id="x01"></a>[Static reflection](cpp/cpp26_reflection_interview.md), splicing and expansion statements | Read | 5/5 | 0 | 0 |
| <a id="x02"></a>Contracts | Pending | 3/5 | 0 | 0 |
| <a id="x03"></a>[Pack indexing and structured-binding extensions](cpp/cpp26_structured_bindings_and_matching.md) | Read | 3/5 | 0 | 0 |
| <a id="x04"></a>[Inplace vector](cpp/inplace_vector_interview.md) | Read | 3/5 | 0 | 0 |
| <a id="x05"></a>[Hive](cpp/std_hive_interview.md) | Read | 3/5 | 0 | 0 |
| <a id="x06"></a>`#embed` for binary resources | Pending | 2/5 | 0 | 0 |
| <a id="x07"></a>Execution control: senders, receivers and scheduling | Pending | 5/5 | 0 | 0 |

### Interview practice

| Topic | Status | Difficulty | Rereads | Practice |
|---|---|---|---|---|
| <a id="i01"></a>[Maximum digital sum](../tasks/max_digital_sum_task/task.md): complexity and integer arithmetic | Pending | 3/5 | 0 | 0 |
| <a id="i02"></a>Hashing, searching, two pointers, sliding windows, stacks and heaps | Pending | 3/5 | 0 | 0 |
| <a id="i03"></a>Trees, graphs, traversal and shortest paths | Pending | 4/5 | 0 | 0 |
| <a id="i04"></a>Dynamic programming, greedy algorithms and correctness arguments | Pending | 4/5 | 0 | 0 |

### Projects

| Topic | Status | Difficulty | Rereads | Practice |
|---|---|---|---|---|
| <a id="p01"></a>Slot-game maths simulator | Pending | 4/5 | 0 | 0 |
| <a id="p02"></a>YD-RP2040 demonstration | Pending | 4/5 | 0 | 0 |
| <a id="p03"></a>Modernize the convex-optimization project | Pending | 5/5 | 0 | 0 |

The [existing notes](cpp/) and [task examples](../tasks/) are available for review. Additional reading: [deque questions](cpp/deque_interview_question.md) and [reflection with type deduction](cpp/reflection_auto_decltype_interview.md).

<a id="practice"></a>
## Practice

Use one small example when a topic needs clarification. Reuse the existing tasks, explain one difference from C#, and return to the topic later without the solution in front of you.

### Next two weeks

**Week 1:** finish `string_view`; revisit RAII, copy/move, references and deduction. Use a small example to check whichever rule is hardest to recall.

**Week 2:** continue with `span`, concepts and ranges; compare `optional`, `variant` and `expected`. Revisit `string_view` after a week and try one existing algorithm task.

Move unfinished topics forward at a comfortable pace. This is a starting sequence, not a deadline.

### Project direction

Work on one main project at a time:

1. **YD-RP2040:** start with the [Pico SDK](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html), GPIO and timers. Continue with bounded buffers, interrupts and multicore coordination. Use the board's supported language and library features.
2. **Game Math Social Slot simulator:** start with fixed C# input/output cases and a correct sequential payout calculation. Then add sampling, RTP, variance and uncertainty; profile before adding parallel batches. Check overflow and document reproducibility. Identical seeds alone do not guarantee matching C# and C++ random sequences.
3. **Convex optimization:** after core reading, preserve a numerical and performance baseline and modernize one part at a time. Compare convergence, residuals, tolerances, allocation counts and timing. Use this project for the later static-polymorphism, cache, pool, SIMD and expression-template topics.

The C# simulator and old convex project remain external references until their source is supplied. Small embedded experiments can fit between project blocks. Completing the curriculum is not a prerequisite for job applications.

### C# comparisons to keep in mind

- RAII and copy/move versus `IDisposable`, `using` and reference assignment.
- `string_view`/`span` versus `ReadOnlySpan`/`Span`: ownership and lifetime rules.
- Templates and concepts versus generics and constraints.
- Ranges versus LINQ: deferred work and the lifetime of the source.
- Coroutines versus `yield return` and `async`/`await`.
- Threads, atomics and parallel algorithms versus Tasks, `Parallel` and `Interlocked`.

<a id="review-routine"></a>
## Review routine

Aim for **60–90 minutes on five days**, with one optional longer session and one day off. In a busy week, use **three 30-minute sessions**.

Start with up to **15 minutes of recall**, then spend the rest on the current topic or a small example. Try to explain the idea before rereading it.

Revisit a newly learned topic after roughly **1, 3, 7, 14, 30 and 60 days**. If it is difficult, return in 1–3 days. If a review is missed, resume at the next available session without catch-up debt.

Once a week, mix an older topic with a recent one. Once a month, rebuild a small example from memory. A short explanation or video can replace a review session.

After a session, update the counts and add a short log entry. Mark Done only after independent explanation and application, followed by another successful check at least seven days later. Hints or a copied solution mean more practice is needed. When a topic first becomes Done, save its date and counts in the log; keep that note even if the topic later returns to InProgress.

<a id="study-log"></a>
## Study log

No sessions recorded yet. Append a short entry after studying; add corrections as new entries so the history stays visible.

| Date | Topic | What I did and how it went | Next review |
|---|---|---|---|

A note can be as simple as: “Recalled the lifetime rule, but needed a hint for the temporary-string case.” Mention help used and link an example when useful.

For a recurring mistake, write **what I expected → the correct rule → a small example**.

Optional AI quiz prompt:

> Ask me one question at a time about [topic]. Wait for my answer before explaining. Include one code-prediction question and one C# comparison. Clearly label hints, and finish with the one thing I should practise next.

<a id="references"></a>
## References

<details>
<summary>Published usage research</summary>

These findings give context for a few topics. They do not determine study priority. Code measurements and self-reported surveys measure different things; standard adoption does not establish use of every feature in that standard.

- **Containers:** a study of 30 applications, 379 historical revisions and about 149 million C++ lines found greater use-site shares for containers, utilities and strings than for the other library categories. The category comparison uses each application's category count divided by all its standard-library uses, with latest revisions from the end of **2014**. This is historical category evidence, not a percentage of projects using a particular container. [Wu et al., 2015](https://ksiresearch.org/seke/seke15paper/seke15paper_9.pdf).
- **Templates and lambdas:** in **20 selected C/C++ projects**, templates appear in all 20, counted from the positive entries in Tables II–III; lambdas appear in 10. This is historical source-code analysis, published in **2020**. Histories include 2019; an exact common collection cutoff is not established in the inspected text. These figures do not measure concepts, CRTP or individual capture forms. [Kim and Ho, 2020](https://ksiresearch.org/seke/seke20paper/paper121.pdf).
- **Concurrency:** a study of **492 open-source concurrent applications**, about 131 million C++ lines, found thread-based constructs used more than atomic/task-based constructs, and lock-based synchronization more than the compared alternatives. Published **2016**; the accessible abstract does not establish the collection cutoff. No current feature percentage is inferred. [Wu et al., 2016](https://doi.org/10.1016/j.infsof.2016.04.004).
- **Other individual topics:** **Unknown — no suitable published measurement found** in the reviewed sources.

For separate **self-reported standard adoption**, the [JetBrains 2025 survey](https://lp.jetbrains.com/the-state-of-cpp-2025/) reports regular use of C++17 at 43%, C++20 at 34%, C++23 at 21% and C++26 at 7%. Respondents could report multiple standards; the question-specific sample size is not stated on the report page. These are regular-use answers, not permitted-use or planned-adoption figures.

The [ISO C++ 2025 survey announcement](https://isocpp.org/blog/2025/05/results-summary-2025-annual-cpp-developer-survey-lite) is also available. Its linked result tables could not be verified during preparation, so no figures from it are included.

</details>

<details>
<summary>C++ version guide and compiler support</summary>

- **C++11:** move/forwarding, smart pointers, lambdas, `auto`/`decltype`, variadic templates, `constexpr`, threads, atomics and random numbers.
- **C++14:** `make_unique`, generic lambdas, init-capture, `decltype(auto)`, variable templates and relaxed `constexpr`.
- **C++17:** folds, `if constexpr`, CTAD, structured bindings, `optional`/`variant`/`any`/`string_view`, `pmr`, filesystem and parallel algorithms.
- **C++20:** concepts, ranges, `span`, coroutines, modules, `consteval`/`constinit`, designated initializers, comparisons, bit utilities, chrono extensions, `format` and `jthread`.
- **C++23:** `expected`, `generator`, `print`, `mdspan`, more ranges operations, `if consteval`, `scope_exit`, explicit object parameters and `move_only_function`.
- **C++26:** reflection, contracts, pack/binding extensions, `inplace_vector`, `hive`, `#embed`, execution control, SIMD/linear-algebra facilities and callable wrappers such as `function_ref` and `copyable_function`.

A feature's standard version and its compiler/library availability are separate. Check [GCC support](https://gcc.gnu.org/projects/cxx-status.html) and [libc++ C++26 support](https://libcxx.llvm.org/Status/Cxx26.html) when trying an example. Embedded targets may support a smaller subset.

</details>

<details>
<summary>Notes for reviewing the existing examples</summary>

- [std_hive_task.cpp](../tasks/std_hive_task.cpp) currently contains a shared-pointer cycle example, not `hive` practice.
- [cppcore_task.cpp](../tasks/cppcore_task.cpp) includes modifying a truly const object through `const_cast`; that is undefined behaviour.
- A moved-from string is valid, but it is not guaranteed to be empty.

</details>

