# C++11–C++26 training and progress

Created: **2026-09-22**. Initial evidence review: **2026-09-22**. Repetition counters are **tracked since 2026-09-22**.

A study path from C++11 to C++26, with C# comparisons and practice in numerical programming, embedded systems and algorithms.


**Start here:** continue the core chat at **“20. Why `string_view` is useful”**, then finish sections 21–28 and attempt [E01](#e01). Keep one active language topic and one applied task. You do not need to complete this entire document before applying for jobs.

## Contents

- [Current position and evidence](#core-chat-checkpoint)
- [How to use the tracker](#how-to-use-the-tracker)
- [Roadmap and difficulty](#roadmap)
- [Current progress](#progress)
- [Repetition counters](#repetitions)
- [Published usage evidence](#usage-evidence)
- [Two-week starting sequence](#first-two-weeks)
- [Projects and later milestones](#projects)
- [Exercise briefs](#exercises)
- [C# comparison map](#csharp-comparisons)
- [Review routine and workload](#review-routine)
- [Session, mistake and AI templates](#templates)
- [Version and toolchain guide](#version-guide)
- [Sources](#sources)

<a id="core-chat-checkpoint"></a>
## Current position and evidence

Reading source: the core C++ lessons. The section-20 checkpoint is recorded below.

The conversation moves through a broad modern-C++ overview, a class/STL refresher, template notation and constant evaluation, initialization, forwarding/reference collapsing, proxy references, folds and `if constexpr`, then the practical C++17 block. That block covers CTAD, structured bindings, `optional`, `variant` and `string_view`.

The recorded reading checkpoint takes precedence over the end of the lessons. Later type-traits, concepts and ranges lessons are available, but their presence alone is not proof of reading. The separate notes establish additional exposure, including traits, generators and C++26 reflection. Reading can therefore be ahead of the main lessons in some topics.

| Source of evidence | What it establishes | What it does not establish |
|---|---|---|
| Recorded core-lesson checkpoint | Earlier material was encountered; C16 is active | Independent recall or complete practice |
| Confirmed reading of the C++ notes | Read status for the covered material | Exact reading dates or repetition counts |
| Existing task source | An example is available for review | Authorship, successful execution, correctness or unaided reproduction |
| A new logged check | The recorded attempt and its result | Permanent mastery of every related feature |

Initial state: **23 Read, 1 InProgress, 37 Pending, 0 Done**, across 61 topic groups. These are tracker states, not a percentage of the C++ language mastered. For grouped topics, Read means confirmed exposure to the material described by the evidence; the next action identifies remaining coverage. Split a group into smaller IDs if its subtopics develop at different speeds.

Existing examples are learning material, including deliberate pitfalls. In particular:

- [`std_hive_task.cpp`](../tasks/std_hive_task.cpp) currently contains the same shared-ownership-cycle example as [`raii_task.cpp`](../tasks/raii_task.cpp); it is not `std::hive` practice.
- [`cppcore_task.cpp`](../tasks/cppcore_task.cpp) attempts to modify a genuinely const object through `const_cast`. Treat that as an undefined-behaviour discussion, not evidence of a valid mutation technique.
- A moved-from `std::string` is valid but its value is generally unspecified; an observed empty string is not a universal postcondition.
- Two notes on the same subject, or two versions of an example, are not two documented study repetitions.

The simulator and convex-optimization exercises need reference cases and source code selected separately; neither implementation is included in this document.

<a id="how-to-use-the-tracker"></a>
## How to use the tracker

| Status | Definition |
|---|---|
| Pending | Studying has not started, or reading is unconfirmed. |
| Read | Confirmed reading/exposure; independent recall is unverified. |
| InProgress | Currently reading, practising or repairing a failed check. |
| Done | Explain the key idea and apply it independently, then repeat the check successfully at least seven days later. |

Reading an answer may move Pending to Read. Start actual work by selecting InProgress. Move to Done only after the completion check below. Failed substantive recall can reopen InProgress; preserve the previous achievement and its evidence.

### Completion check

For the selected topic, without solution notes or substantive AI hints:

1. Explain the purpose, one limitation and one failure case.
2. Solve a small application, debugging or design task. Verify behaviour and relevant edge cases; a compiling program alone is insufficient.
3. Explain the relevant C# similarity and a difference, where one exists.
4. Repeat an explanation and small application check after at least seven days, preferably using a changed input or variant.

Compiler diagnostics, a debugger and tests are normal tools. Looking up an unfamiliar peripheral API is acceptable if the topic being checked is something else; record it. Looking up the answer to the concept under examination makes that attempt Partial.

There is no required number of repetitions. Difficulty, familiarity and the task determine how many are needed. Done means the check was passed, not that future review is unnecessary.

### Updating a session in two minutes

1. Append one log row per topic/activity, with a shared session ID.
2. Update the topic's counters, last result and next review.
3. Change status only when the evidence supports it.
4. On the first Done transition, save the date and the counter snapshot. Never overwrite this snapshot with later totals.

No automation, external flashcard application or new exercise file is required to maintain this document.

<a id="roadmap"></a>
## Roadmap and difficulty

**Now** = review foundations. **Next** = the next learning/project blocks after their prerequisites. **Later** = advanced specialization or recognition before implementation. These are curriculum priorities, not popularity rankings.

Difficulty estimates the learning depth and complexity of each topic. It is an adjustable assessment, not a measured property of the language, an estimated number of hours, or a score derived from rereads.

| Difficulty | Interpretation |
|---|---|
| 1 | Small syntax/API concept with few prerequisites |
| 2 | Routine usage and a few important rules |
| 3 | Ownership, types or interacting concepts require deliberate practice |
| 4 | Subtle lifetime, generic programming, concurrency or performance reasoning |
| 5 | Advanced combinations requiring substantial experiments and careful verification |

Usage labels link to [published findings](#usage-evidence). **Unknown** means **Unknown — no suitable published measurement found** in the reviewed sources. It does not mean unused or unimportant. U1 is category-level evidence only; U2/U3 are historical feature measurements; U4 covers historical concurrency families. Do not read any label as a current industry-wide adoption percentage.

### Core language, library and tooling

| ID | Topic | First standard / evolution | Priority | Difficulty | Published usage |
|---|---|---|---|---|---|
| <a id="c01"></a>C01 | Class/value model, const/ref-qualified methods, virtual/override/final, enums, nullptr | Classic; additions 11 | Now | 2/5 | [Unknown](#u0) |
| <a id="c02"></a>C02 | Initialization, aggregates, member order, designated initializers and scope | Classic foundations; list initialization 11; CTAD 17; designated 20 | Now | 3/5 | [Unknown](#u0) |
| <a id="c03"></a>C03 | RAII, Rule of Zero, unique/shared/weak ownership and resource lifetime | RAII classic; smart pointers 11; make_unique 14 | Now | 3/5 | [Unknown](#u0) |
| <a id="c04"></a>C04 | Copy/move, special members, elision, moved-from state and noexcept | Copy classic; move/noexcept 11; guaranteed elision 17 | Now | 4/5 | [Unknown](#u0) |
| <a id="c05"></a>C05 | Value categories, reference collapsing, forwarding and parameter passing | 11 | Now | 4/5 | [Unknown](#u0) |
| <a id="c06"></a>C06 | auto, decltype, decltype(auto), deduction and decay | 11; return deduction/decltype(auto) 14 | Now | 3/5 | [Unknown](#u0) |
| <a id="c07"></a>C07 | Lambdas, captures, callable objects, std::function and std::invoke | 11; generic/init-capture 14; invoke 17 | Now | 3/5 | [U3: lambdas 10/20](#u3) |
| <a id="c08"></a>C08 | using, aliases, dependent names and reading template declarations | Classic using; alias templates 11; using enum 20 | Now | 2/5 | [Unknown](#u0) |
| <a id="c09"></a>C09 | Templates, specialization, parameter packs and non-type parameters | Classic templates; variadics 11; extensions 17/20 | Now | 3/5 | [U2: templates 20/20](#u2) |
| <a id="c10"></a>C10 | constexpr evolution, consteval, constinit and static initialization | constexpr 11/14; consteval/constinit 20; if consteval 23 | Now | 3/5 | [Unknown](#u0) |
| <a id="c11"></a>C11 | Fold expressions and if constexpr | 17 | Now | 3/5 | [Unknown](#u0) |
| <a id="c12"></a>C12 | CTAD, deduction guides and structured bindings | 17 | Now | 2/5 | [Unknown](#u0) |
| <a id="c13"></a>C13 | Type traits, transformations, SFINAE and constraints preparation | traits 11; _t 14; _v 17; remove_cvref 20 | Now | 3/5 | [Unknown](#u0) |
| <a id="c14"></a>C14 | optional and choosing value-based optional results | 17; optional monadic operations 23 | Next | 2/5 | [Unknown](#u0) |
| <a id="c15"></a>C15 | variant, visit and overload-based visitors | 17; member visit 26 | Now | 3/5 | [Unknown](#u0) |
| <a id="c16"></a>C16 | string and string_view: ownership, slices and API boundaries | string classic; string_view 17 | Now | 3/5 | [Unknown](#u0) |
| <a id="c17"></a>C17 | Sequential containers, deque, storage and iterator/reference invalidation | Mostly classic; array/forward_list 11; constexpr deque 26 | Now | 3/5 | [U1: container category](#u1) |
| <a id="c18"></a>C18 | Associative containers, hashing and container adaptors | Mostly classic; unordered containers 11 | Now | 3/5 | [U1: container category](#u1) |
| <a id="c19"></a>C19 | Iterators, standard algorithms, numeric algorithms and complexity | Classic; additions 11/17/20/23 | Next | 3/5 | [Unknown](#u0) |
| <a id="c20"></a>C20 | Casts, object representation, alignment, aliasing and undefined behaviour | Named casts classic; bit_cast 20 | Now | 4/5 | [Unknown](#u0) |
| <a id="c21"></a>C21 | Exception safety, noexcept contracts and error-handling policy | Classic exceptions; noexcept 11 | Next | 3/5 | [Unknown](#u0) |
| <a id="c22"></a>C22 | span, bounded views and ownership at buffer interfaces | 20 | Next | 3/5 | [Unknown](#u0) |
| <a id="c23"></a>C23 | Concepts, requires expressions/clauses and constrained overloads | 20 | Next | 4/5 | [Unknown](#u0) |
| <a id="c24"></a>C24 | Ranges/views, lazy evaluation, projections and materialization | 20; ranges::to and more views 23 | Next | 4/5 | [Unknown](#u0) |
| <a id="c25"></a>C25 | format/print, text conversion, filesystem and practical I/O | filesystem/charconv 17; format 20; print 23 | Next | 2/5 | [Unknown](#u0) |
| <a id="c26"></a>C26 | Synchronous coroutines, co_yield and std::generator | Coroutines 20; generator 23 | Next | 4/5 | [Unknown](#u0) |
| <a id="c27"></a>C27 | co_await, coroutine ownership, suspension and cancellation | 20; library protocols vary | Later | 5/5 | [Unknown](#u0) |
| <a id="c28"></a>C28 | Headers, ODR, linkage, ABI, modules and build boundaries | Classic foundations; modules 20 | Next | 3/5 | [Unknown](#u0) |
| <a id="c29"></a>C29 | expected and explicit error propagation | 23 | Next | 3/5 | [Unknown](#u0) |
| <a id="c30"></a>C30 | Modern API checklist: any, attributes, comparisons, chrono/bit, scope_exit, explicit object parameters and callable wrappers | Attributes 11/17; any 17; comparison/chrono/bit 20; scope_exit/explicit object/move_only_function 23; function_ref/copyable_function 26 | Next | 3/5 | [Unknown](#u0) |
| <a id="c31"></a>C31 | CMake targets, tests, warnings, debugger, sanitizers and measurement setup | Tooling; not a language-version feature | Now | 3/5 | [Unknown](#u0) |

### Numerical and parallel foundations

| ID | Topic | First standard / evolution | Priority | Difficulty | Published usage |
|---|---|---|---|---|---|
| <a id="n01"></a>N01 | Integer overflow, floating-point error, precision and stable accumulation | Classic numerics; modern library support | Next | 4/5 | [Unknown](#u0) |
| <a id="n02"></a>N02 | Probability, RNGs, sampling, Monte Carlo estimates and uncertainty | random library 11; mathematics version-independent | Next | 4/5 | [Unknown](#u0) |
| <a id="n03"></a>N03 | Threads, mutexes, condition variables, futures, jthread and coordination | 11; scoped_lock 17; jthread/stop tokens/latch/barrier/semaphore 20 | Next | 4/5 | [U4: family comparison](#u4) |
| <a id="n04"></a>N04 | Atomics, memory ordering, happens-before and lock-free limitations | 11; atomic wait/notify 20 | Next | 5/5 | [U4: family comparison](#u4) |
| <a id="n05"></a>N05 | Parallel algorithms, partitioning and deterministic reductions | Parallel policies/reduce/transform_reduce 17; unseq 20 | Next | 4/5 | [Unknown](#u0) |
| <a id="n06"></a>N06 | Profiling, fair benchmarks, throughput/latency and scaling | Tooling; version-independent | Next | 4/5 | [Unknown](#u0) |

### Embedded skills

| ID | Topic | First standard / evolution | Priority | Difficulty | Published usage |
|---|---|---|---|---|---|
| <a id="b01"></a>B01 | Microcontroller model, cross-compilation, C interoperability and freestanding limits | Platform skills; C++ subset depends on toolchain | Next | 3/5 | [Unknown](#u0) |
| <a id="b02"></a>B02 | GPIO, timers, MMIO, volatile and nonblocking state machines | Platform skills; not C++26-specific | Next | 3/5 | [Unknown](#u0) |
| <a id="b03"></a>B03 | Interrupts, bounded ring buffers, DMA/PIO and peripheral interfaces | Platform skills; SDK/device-specific | Later | 4/5 | [Unknown](#u0) |
| <a id="b04"></a>B04 | Multicore coordination, RTOS basics, priorities and timing budgets | Platform skills; runtime-specific | Later | 5/5 | [Unknown](#u0) |

### Advanced work after core-language reading

| ID | Topic | First standard / evolution | Priority | Difficulty | Published usage |
|---|---|---|---|---|---|
| <a id="a01"></a>A01 | Static polymorphism, policies, CRTP and explicit object parameters | Templates/CRTP classic; concepts 20; explicit object 23 | Later | 4/5 | [Unknown](#u0) |
| <a id="a02"></a>A02 | L1/L2/L3, locality, data layout, working sets, tiling and false sharing | Architecture/performance skills | Later | 4/5 | [Unknown](#u0) |
| <a id="a03"></a>A03 | Allocators, arenas, pools, std::pmr and bounded allocation | Allocator model classic; pmr 17 | Later | 4/5 | [Unknown](#u0) |
| <a id="a04"></a>A04 | Dense/sparse linear algebra, conditioning, mdspan and linalg | Numerics classic; mdspan 23; linalg 26 | Later | 5/5 | [Unknown](#u0) |
| <a id="a05"></a>A05 | Auto-vectorization, SIMD, alignment, tails and bandwidth limits | Compiler/platform-specific; standard simd library 26 | Later | 4/5 | [Unknown](#u0) |
| <a id="a06"></a>A06 | Expression templates, lazy expression lifetime and aliasing | Template technique; not introduced by C++26 | Later | 5/5 | [Unknown](#u0) |

### C++26 feature track

| ID | Topic | First standard / evolution | Priority | Difficulty | Published usage |
|---|---|---|---|---|---|
| <a id="x01"></a>X01 | Static reflection, metadata, splicing and expansion statements | 26 | Later | 5/5 | [Unknown](#u0) |
| <a id="x02"></a>X02 | Contracts and their relationship to validation/error handling | 26 | Later | 3/5 | [Unknown](#u0) |
| <a id="x03"></a>X03 | Pack indexing and structured-binding extensions | 26 | Later | 3/5 | [Unknown](#u0) |
| <a id="x04"></a>X04 | inplace_vector and fixed-capacity dynamic-size storage | 26 | Later | 3/5 | [Unknown](#u0) |
| <a id="x05"></a>X05 | hive, stable handles and container tradeoffs | 26 | Later | 3/5 | [Unknown](#u0) |
| <a id="x06"></a>X06 | #embed and embedding binary resources | 26 | Later | 2/5 | [Unknown](#u0) |
| <a id="x07"></a>X07 | Execution control, senders/receivers and scheduling | 26; distinct from C++17 execution policies | Later | 5/5 | [Unknown](#u0) |

### Algorithms and project milestones

| ID | Topic | First standard / evolution | Priority | Difficulty | Published usage |
|---|---|---|---|---|---|
| <a id="i01"></a>I01 | Complexity, integer arithmetic and maximum digital sum | Algorithms; version-independent | Next | 3/5 | [Unknown](#u0) |
| <a id="i02"></a>I02 | Arrays/strings, hashing, binary search, two pointers, windows, stacks and heaps | Algorithms; version-independent | Next | 3/5 | [Unknown](#u0) |
| <a id="i03"></a>I03 | Trees, graphs, traversal, shortest paths and disjoint sets | Algorithms; version-independent | Later | 4/5 | [Unknown](#u0) |
| <a id="i04"></a>I04 | Dynamic programming, greedy choices and proof of correctness | Algorithms; version-independent | Later | 4/5 | [Unknown](#u0) |
| <a id="p02"></a>P02 | YD-RP2040 bounded processing demonstration | Project; board-supported C++ subset | Next | 4/5 | [Unknown](#u0) |
| <a id="p01"></a>P01 | C++ slot-maths simulator with C# reference cases | Project; host C++ first | Next | 4/5 | [Unknown](#u0) |
| <a id="p03"></a>P03 | Modernization of a legacy convex-optimization project | Project; C++26 where supported | Later | 5/5 | [Unknown](#u0) |

### Prerequisite routes

- **Language route:** C16 → C03–C06 recall → C09/C11–C13 → C22/C23 → C24/C29. Reuse confirmed reading instead of restarting from page one.
- **Numerical route:** C17–C19/C21/C29/C31 → N01/N02 → sequential P01 → N03/N04/N05/N06 → measured parallel P01.
- **Embedded route:** C02/C03/C20/C22/C31 → B01/B02 → bounded P02 → B03/B04.
- **Advanced route:** finish the intended core-language reading pass, then check C03–C06/C09/C23 and N01 before A01–A06/P03. Basic RAII and allocation awareness belong in the core; detailed pool/cache/static-polymorphism experiments wait for this stage.
- **C++26 route:** read X01–X07 at recognition level alongside the core pass. Deep reflection/execution experiments follow their prerequisites. Missing compiler support can delay an exercise without preventing conceptual reading.

<a id="progress"></a>
## Current progress

These rows are an evidence-based starting point. No existing example has been promoted to Done merely because its file exists.

| Topic | Status | Evidence already available | Next action |
|---|---|---|---|
| [C01](#c01) | Read | [Core](#core-chat-checkpoint); [cppcore](../tasks/cppcore_task.cpp), [nullptr](../tasks/nullptr_task.cpp) | Explain value versus reference semantics and predict overload selection. |
| [C02](#c02) | Read | [Initialization notes](cpp/designated_initializers_scope_shadowing_interview.md); [task](../tasks/initialization_task.cpp) | Predict braces versus parentheses, narrowing and declaration-order initialization. |
| [C03](#c03) | Read | [Ownership notes](cpp/raii_smart_pointers_interview.md); [raii_task](../tasks/raii_task.cpp), [misnamed hive task](../tasks/std_hive_task.cpp) | Explain the existing ownership cycle; design a weak link and predict destruction. |
| [C04](#c04) | Read | [Copy/move notes](cpp/copy_move_traits_interview.md); [traits task](../tasks/traits_task.cpp) | Trace constructors; distinguish a move expression from actual resource transfer. |
| [C05](#c05) | Read | [Core](#core-chat-checkpoint); [reference task](../tasks/reference_task.cpp), [casts](../tasks/cast_task.cpp) | Explain why a named rvalue reference is an lvalue; reproduce forwarding cases. |
| [C06](#c06) | Read | [Deduction notes](cpp/auto_deduction.md), [reflection/deduction notes](cpp/reflection_auto_decltype_interview.md); [reference task](../tasks/reference_task.cpp) | Predict types without running code; check with static_assert. |
| [C07](#c07) | Read | [Core](#core-chat-checkpoint); [function task](../tasks/function_task.cpp), [cppcore](../tasks/cppcore_task.cpp) | Check capture lifetimes and compare a concrete callable with type erasure. |
| [C08](#c08) | Read | [Using notes](cpp/using_interview.md); [using task](../tasks/using_task.cpp) | Distinguish an alias, declaration, inherited overload and namespace directive. |
| [C09](#c09) | Read | [Core](#core-chat-checkpoint); [template task](../tasks/template_task.cpp) | Write one generic function, one specialization and one fixed-size type. |
| [C10](#c10) | Read | [constinit notes](cpp/cpp26_constinit_interview.md); [constexpr task](../tasks/constexpr_task.cpp), [initialization task](../tasks/initialization_task.cpp) | Separate compile-time evaluation, constness and initialization guarantees. |
| [C11](#c11) | Read | [Core](#core-chat-checkpoint); [fold task](../tasks/fold_expression_task.cpp), [constexpr task](../tasks/constexpr_task.cpp) | Handle an empty pack and explain discarded branches. |
| [C12](#c12) | Read | [Binding notes](cpp/cpp26_structured_bindings_and_matching.md); [binding task](../tasks/structured_bind_task.cpp), [initialization task](../tasks/initialization_task.cpp) | Predict which bindings copy and which alias their source. |
| [C13](#c13) | Read | [Copy/move traits](cpp/copy_move_traits_interview.md), [deduction](cpp/auto_deduction.md); [traits task](../tasks/traits_task.cpp) | Explain decay versus remove_cvref and a trait that accepts copying from an rvalue. |
| [C14](#c14) | Read | [Core](#core-chat-checkpoint): optional sections 9–11 | Recall optional basics; study the C++23 monadic operations, which remain unconfirmed. |
| [C15](#c15) | Read | [Variant notes](cpp/std_variant_visit_interview.md); [variant task](../tasks/variant_task.cpp) | Handle every alternative, an error case and a consistent visitor result type. |
| [C16](#c16) | InProgress | [Core checkpoint](#core-chat-checkpoint): section 20 | Continue sections 21–28; do E01 before introducing another new topic. |
| [C17](#c17) | Read | [Deque notes](cpp/deque_interview.md), [deque Q&A](cpp/deque_interview_question.md); [initialization task](../tasks/initialization_task.cpp) | Choose vector/array/deque/list by lifetime, locality and mutation requirements. |
| [C18](#c18) | Read | [Core](#core-chat-checkpoint): containers; [initialization task](../tasks/initialization_task.cpp) | Explain comparator/hash requirements, lookup cost and duplicate keys. |
| [C19](#c19) | Pending | [Maximum-digital-sum study](../tasks/max_digital_sum_task/Maximum_Digital_Sum_Algorithm_Study.md) is available, not a confirmed reading record | Use sort/find/accumulate with iterator requirements; separate complexity from timing. |
| [C20](#c20) | Read | [Cast notes](cpp/casts_interview.md); [cast task](../tasks/cast_task.cpp), [cppcore](../tasks/cppcore_task.cpp) | Explain why modifying a genuinely const object through const_cast is invalid. |
| [C21](#c21) | Pending | Ownership notes provide overlap; full independent study unconfirmed | Give basic/strong/no-throw guarantees and define cleanup after partial construction. |
| [C22](#c22) | Pending | [cppcore](../tasks/cppcore_task.cpp) contains a span example; full reading unconfirmed | Accept caller-owned storage and check lifetime, bounds and mutability. |
| [C23](#c23) | Pending | Later core-chat lesson; [concept task](../tasks/concept_task.cpp) exists | Check standard concepts first; distinguish syntactic requirements from semantic promises. |
| [C24](#c24) | Pending | Later core-chat lesson exists; no confirmed reading beyond checkpoint | Build a safe pipeline and explain ownership, repeated evaluation and invalidation. |
| [C25](#c25) | Pending | [hello](../tasks/hello_task.cpp), [utils header](../tasks/utils.hpp), [utils implementation](../tasks/utils.cpp) provide examples | Distinguish formatting from parsing; validate file and conversion failures. |
| [C26](#c26) | Read | [Yield notes](cpp/yield_interview.md); [generator task](../tasks/gcc/generator_task.cpp), [yield task](../tasks/gcc/yield_task.cpp) | Explain frame and yielded-value lifetimes; reconstruct one short pipeline. |
| [C27](#c27) | Pending | Generator exposure does not establish asynchronous-coroutine understanding | Trace suspension and destruction before designing an asynchronous abstraction. |
| [C28](#c28) | Pending | [Build configuration](../CMakeLists.txt); [task configuration](../tasks/CMakeLists.txt) | Diagnose a compile/link error; read modules without making them a project prerequisite. |
| [C29](#c29) | Pending | Initial core-chat overview only; full reading/practice unconfirmed | Represent success and domain failure without losing the reason for failure. |
| [C30](#c30) | Pending | Some overview exposure; read/check each named item separately | Make a short checklist; retain this group as InProgress until its selected items are covered. |
| [C31](#c31) | Pending | [README](../README.md); [root build](../CMakeLists.txt), [GCC targets](../tasks/gcc/CMakeLists.txt) | Build one target, debug it and document compiler/library/flags. |
| [N01](#n01) | Pending | [Digital-sum task](../tasks/max_digital_sum_task/task.md) supplies an overflow example | Choose payout/accumulator types and justify error tolerances. |
| [N02](#n02) | Pending | C# reference cases to be selected | Replay fixed inputs; validate a small exactly enumerable model before random trials. |
| [N03](#n03) | Pending | [Thread example](../tasks/complex_task/main.cpp), [alternative](../tasks/complex_task/alternative_imlementation2_task.cpp) exist | Establish joining, cancellation and a race-free producer/consumer example. |
| [N04](#n04) | Pending | No verified exercise yet | Reason about publication and invariants; do not substitute volatile for synchronization. |
| [N05](#n05) | Pending | No verified exercise yet | Partition independent batches; compare with a sequential oracle and state ordering assumptions. |
| [N06](#n06) | Pending | Existing examples are not benchmark evidence | Measure optimized builds with correctness checks and recorded hardware/flags. |
| [B01](#b01) | Pending | [YD-RP2040 board reference](https://crates.io/crates/vcc-gnd-yd-rp2040) | Identify exact board revision, SDK board configuration and a working build/flash path. |
| [B02](#b02) | Pending | No board result recorded | Produce a timed output/state machine and explain the role of volatile at device registers. |
| [B03](#b03) | Pending | No board result recorded | Keep interrupt work bounded; verify buffer overflow handling and ownership. |
| [B04](#b04) | Pending | No board result recorded | Measure a deadline and explain synchronization, priority inversion and cancellation/shutdown. |
| [A01](#a01) | Pending | Scheduled after core-language reading | Compare static and virtual designs while preserving identical behaviour. |
| [A02](#a02) | Pending | Scheduled after core-language reading | Inspect the actual CPU; compare access patterns and explain measured differences. |
| [A03](#a03) | Pending | Scheduled after core-language reading | Count allocations; test resource lifetime, upstream fallback, exhaustion and contention. |
| [A04](#a04) | Pending | Numerical reference cases to be selected | Preserve layouts and numerical tolerances while modernizing one kernel. |
| [A05](#a05) | Pending | Scheduled after core-language reading | Compare a scalar oracle with vectorized code, including tails and accuracy. |
| [A06](#a06) | Pending | Scheduled after core-language reading | Diagnose a temporary/aliasing hazard before attempting an expression abstraction. |
| [X01](#x01) | Read | [Reflection notes](cpp/cpp26_reflection_interview.md), [reflection/deduction](cpp/reflection_auto_decltype_interview.md); [GCC task](../tasks/gcc/reflection_gcc_task.cpp) | Reproduce one small example on a supporting toolchain and explain compile-time versus runtime reflection. |
| [X02](#x02) | Pending | Initial core-chat overview only | Explain a precondition and contract semantics; verify actual compiler support. |
| [X03](#x03) | Read | [Binding/matching notes](cpp/cpp26_structured_bindings_and_matching.md) | Separate C++17 decomposition from C++26 packs/conditions; no general pattern-matching assumption. |
| [X04](#x04) | Read | [inplace_vector notes](cpp/inplace_vector_interview.md) | Check capacity failure, element lifetime and allocation inside the element type. |
| [X05](#x05) | Read | [hive notes](cpp/std_hive_interview.md); current hive-named task does not use hive | Compare with vector/deque for actual mutation patterns and invalidation rules. |
| [X06](#x06) | Pending | Initial core-chat overview only | Read a small resource example and check toolchain support. |
| [X07](#x07) | Pending | Initial core-chat overview only | Understand composition and cancellation after N03/N05; check library availability. |
| [I01](#i01) | Pending | [Statement](../tasks/max_digital_sum_task/task.md), [study](../tasks/max_digital_sum_task/Maximum_Digital_Sum_Algorithm_Study.md), [vector](../tasks/max_digital_sum_task/main.cpp), [array](../tasks/max_digital_sum_task/max_digits_sum_array.cpp) | Solve a small case from a blank page; explain storage bounds and complexity. |
| [I02](#i02) | Pending | No verified solution log yet | Select one pattern and solve one small problem with edge cases. |
| [I03](#i03) | Pending | No verified solution log yet | State invariants and cover disconnected/cyclic inputs. |
| [I04](#i04) | Pending | No verified solution log yet | Define state/transition or exchange argument before writing code. |
| [P01](#p01) | Pending | C# reference implementation to be selected | Select a small shareable slice and record known input/output fixtures. |
| [P02](#p02) | Pending | No board implementation evidence recorded here | Demonstrate a timed task with bounded storage and an observable failure policy. |
| [P03](#p03) | Pending | Legacy codebase to be selected | Inventory algorithms/dependencies; capture numerical and performance baselines first. |

<a id="repetitions"></a>
## Repetition counters

**Historical rereads, recall attempts and coding repeats are Unknown for every initial topic.** The counters below cover only sessions logged **since 2026-09-22**. A zero is not a claim that you have never repeated the topic.

- **R:** additional reading sessions after the first reading. Finishing an initial reading is not a reread.
- **Q:** successful closed-notes recall sessions / all recall attempts.
- **C:** successful independent coding, debugging or applied-design sessions / all such attempts.
- Count each activity at most once per topic within one session. Five quiz questions are one Q attempt; ten compiler runs are one C attempt.
- A genuine later session may add another attempt. A session with recall followed by rereading can add one Q and one R, with the recall result recorded before help.
- Log Pass, Partial or Retry for Q/C. Only Pass increases the numerator. Reading alone has result “—”.
- Pass means the selected check succeeded independently; Partial means incomplete understanding or substantive hints/solution consultation; Retry means the check could not be completed correctly. Copying or rerunning an example does not establish a coding pass.
- **First Done** stores `date; R=n; Q=p/a; C=p/a` when the completion check first passes. All snapshots begin blank.
- “—” for dates means unrecorded/unscheduled. Start C16 at the next chosen study session; do not create an overdue queue for every Read topic.

| Topic | R | Q passed/attempted | C passed/attempted | Last review / result | Next review | First Done snapshot |
|---|---|---|---|---|---|---|
| [C01](#c01) | 0 | 0/0 | 0/0 | — | — | — |
| [C02](#c02) | 0 | 0/0 | 0/0 | — | — | — |
| [C03](#c03) | 0 | 0/0 | 0/0 | — | — | — |
| [C04](#c04) | 0 | 0/0 | 0/0 | — | — | — |
| [C05](#c05) | 0 | 0/0 | 0/0 | — | — | — |
| [C06](#c06) | 0 | 0/0 | 0/0 | — | — | — |
| [C07](#c07) | 0 | 0/0 | 0/0 | — | — | — |
| [C08](#c08) | 0 | 0/0 | 0/0 | — | — | — |
| [C09](#c09) | 0 | 0/0 | 0/0 | — | — | — |
| [C10](#c10) | 0 | 0/0 | 0/0 | — | — | — |
| [C11](#c11) | 0 | 0/0 | 0/0 | — | — | — |
| [C12](#c12) | 0 | 0/0 | 0/0 | — | — | — |
| [C13](#c13) | 0 | 0/0 | 0/0 | — | — | — |
| [C14](#c14) | 0 | 0/0 | 0/0 | — | — | — |
| [C15](#c15) | 0 | 0/0 | 0/0 | — | — | — |
| [C16](#c16) | 0 | 0/0 | 0/0 | — | Next study session (baseline) | — |
| [C17](#c17) | 0 | 0/0 | 0/0 | — | — | — |
| [C18](#c18) | 0 | 0/0 | 0/0 | — | — | — |
| [C19](#c19) | 0 | 0/0 | 0/0 | — | — | — |
| [C20](#c20) | 0 | 0/0 | 0/0 | — | — | — |
| [C21](#c21) | 0 | 0/0 | 0/0 | — | — | — |
| [C22](#c22) | 0 | 0/0 | 0/0 | — | — | — |
| [C23](#c23) | 0 | 0/0 | 0/0 | — | — | — |
| [C24](#c24) | 0 | 0/0 | 0/0 | — | — | — |
| [C25](#c25) | 0 | 0/0 | 0/0 | — | — | — |
| [C26](#c26) | 0 | 0/0 | 0/0 | — | — | — |
| [C27](#c27) | 0 | 0/0 | 0/0 | — | — | — |
| [C28](#c28) | 0 | 0/0 | 0/0 | — | — | — |
| [C29](#c29) | 0 | 0/0 | 0/0 | — | — | — |
| [C30](#c30) | 0 | 0/0 | 0/0 | — | — | — |
| [C31](#c31) | 0 | 0/0 | 0/0 | — | — | — |
| [N01](#n01) | 0 | 0/0 | 0/0 | — | — | — |
| [N02](#n02) | 0 | 0/0 | 0/0 | — | — | — |
| [N03](#n03) | 0 | 0/0 | 0/0 | — | — | — |
| [N04](#n04) | 0 | 0/0 | 0/0 | — | — | — |
| [N05](#n05) | 0 | 0/0 | 0/0 | — | — | — |
| [N06](#n06) | 0 | 0/0 | 0/0 | — | — | — |
| [B01](#b01) | 0 | 0/0 | 0/0 | — | — | — |
| [B02](#b02) | 0 | 0/0 | 0/0 | — | — | — |
| [B03](#b03) | 0 | 0/0 | 0/0 | — | — | — |
| [B04](#b04) | 0 | 0/0 | 0/0 | — | — | — |
| [A01](#a01) | 0 | 0/0 | 0/0 | — | — | — |
| [A02](#a02) | 0 | 0/0 | 0/0 | — | — | — |
| [A03](#a03) | 0 | 0/0 | 0/0 | — | — | — |
| [A04](#a04) | 0 | 0/0 | 0/0 | — | — | — |
| [A05](#a05) | 0 | 0/0 | 0/0 | — | — | — |
| [A06](#a06) | 0 | 0/0 | 0/0 | — | — | — |
| [X01](#x01) | 0 | 0/0 | 0/0 | — | — | — |
| [X02](#x02) | 0 | 0/0 | 0/0 | — | — | — |
| [X03](#x03) | 0 | 0/0 | 0/0 | — | — | — |
| [X04](#x04) | 0 | 0/0 | 0/0 | — | — | — |
| [X05](#x05) | 0 | 0/0 | 0/0 | — | — | — |
| [X06](#x06) | 0 | 0/0 | 0/0 | — | — | — |
| [X07](#x07) | 0 | 0/0 | 0/0 | — | — | — |
| [I01](#i01) | 0 | 0/0 | 0/0 | — | — | — |
| [I02](#i02) | 0 | 0/0 | 0/0 | — | — | — |
| [I03](#i03) | 0 | 0/0 | 0/0 | — | — | — |
| [I04](#i04) | 0 | 0/0 | 0/0 | — | — | — |
| [P01](#p01) | 0 | 0/0 | 0/0 | — | — | — |
| [P02](#p02) | 0 | 0/0 | 0/0 | — | — | — |
| [P03](#p03) | 0 | 0/0 | 0/0 | — | — | — |

Interpret Q and C separately. “Three rereads” and “three successful unaided solutions” describe different learning evidence. Once a First Done snapshot exists, compare it with the current totals to see acquisition effort versus maintenance effort. Do not turn these counts into a competition or a measure of ability.


<a id="usage-evidence"></a>
## Published usage evidence

These are published findings, not a new scan of GitHub. A paper's publication year is not necessarily the year its code was collected. Percentages of projects, contributors, use sites and survey answers have different denominators and cannot be averaged into a universal “usage score”.

<a id="u0"></a>
**U0 / Unknown:** **Unknown — no suitable published measurement found** for that specific topic in the sources reviewed here. General evidence about templates, containers or adoption of C++20 does not establish the prevalence of concepts, ranges, CRTP, `pmr` or any other individual facility.

| Evidence ID | Published finding used here | Method and population | Measurement period / publication | Scope and source |
|---|---|---|---|---|
| <a id="u1"></a>U1 | Containers, utilities and strings had greater use-site shares than the other library categories studied. | Source-code analysis: 30 applications; 379 historical revisions, approximately 149 million C++ lines across revisions. The category comparison uses each application's category use count divided by its total standard-library use count. | Latest revisions at end of 2014 for this comparison; historical revisions from after September 2011 for other questions. Published 2015. | Category evidence only, not a percentage of projects using vector/deque or modern containers. [Wu et al., Sections III–IV, Figure I](https://ksiresearch.org/seke/seke15paper/seke15paper_9.pdf). |
| <a id="u2"></a>U2 | Templates appear in 20/20 sampled projects, counted from the positive entries in Tables II–III. | Published source-code/history analysis of 20 selected C/C++ projects. Unit here: projects with a detected template. | Histories include events in 2019; an exact common extraction cutoff is not established in the inspected text. Published 2020. | Historical selected sample, not contemporary industry prevalence; no separate measurement of concepts, CRTP or modern NTTPs. [Kim & Ho, Tables II–III](https://ksiresearch.org/seke/seke20paper/paper121.pdf). |
| <a id="u3"></a>U3 | Lambdas were detected in 10/20 sampled projects. | Same 20-project code/history study; unit here: projects, not developers or executed operations. | Same historical dataset as U2; published 2020. | Applies to lambdas, not to std::function/std::invoke or every capture form. [Kim & Ho, Section IV](https://ksiresearch.org/seke/seke20paper/paper121.pdf). |
| <a id="u4"></a>U4 | Thread-based constructs were used more than atomics-based/task-based constructs; lock-based synchronization more than the alternatives compared. | Source-code study of 492 open-source concurrent applications, approximately 131 million C++ lines; the paper also includes a developer survey. These findings concern its code analysis. | Exact code-collection cutoff not established from the accessible publisher abstract; published 2016. | Historical concurrency families. No current percentage, no jthread/C++26-execution inference. [Wu et al., publisher abstract](https://doi.org/10.1016/j.infsof.2016.04.004). |

### Survey context: keep separate from code measurements

| Source | Reported result | Population, period and interpretation |
|---|---|---|
| [JetBrains, State of C++ 2025](https://lp.jetbrains.com/the-state-of-cpp-2025/) | Regular standard use: C++17 43%, C++20 34%, C++23 21%, C++26 7%. | Self-reported answers from C++ respondents in the 2025 Developer Ecosystem survey; question-specific sample size is not stated on the report page. Multiple standards can be reported. These are standard-use answers, not feature counts or production-code measurements. |
| [ISO C++ 2025 survey announcement/results](https://isocpp.org/blog/2025/05/results-summary-2025-annual-cpp-developer-survey-lite) | Reference for production-policy/adoption context; no numerical feature result imported here. | The announcement was identified, but the linked result tables could not be verified during preparation. Do not fill feature cells from an announcement or a differently dated survey. |

For future additions, read the original question/table. “Plan to allow”, “already permitted”, “regularly use” and “detected in source” are different outcomes. Record the source's exact population and unit. If only a qualitative comparison is supported, keep it qualitative.

Missing prevalence data must not prevent learning a topic required by your work. In particular, study safe ownership, numerical correctness, concurrency and embedded constraints for their practical role even when a suitable topic-level usage statistic is absent.

<a id="first-two-weeks"></a>
## Immediate two-week sequence

This is a proposed sequence, not a completed-session log or a deadline. Choose actual dates yourself. Each required session is 60–90 minutes including review and logging. Move unfinished work forward rather than doubling the next day's load.

| Session | Focus | Concrete outcome |
|---|---|---|
| Week 1, day 1 | C16; finish core-chat sections 21–28 | Explain substrings, temporary/source lifetime and missing null termination; attempt [E01](#e01). Log a baseline, including mistakes. |
| Week 1, day 2 | C03/C04; short C16 recall | Trace ownership/copy/move behaviour using [E02](#e02). Use existing notes only after making predictions. |
| Week 1, day 3 | C05/C06/C20 | Attempt the small deduction/lifetime cases in [E03](#e03). Pick the two weakest rules for future recall. |
| Week 1, day 4 | C09/C11/C13 | Reconstruct one template/fold and explain its traits. If this is easy, stop revising it and prepare C23 prerequisites. |
| Week 1, day 5 | C22/C31 | Pass a caller-owned buffer safely, build one target and inspect one debugger session. Write down compiler/library/flags. |
| Week 1, optional practical session | B01 orientation, up to 90–120 minutes | Identify the board and SDK build/flash procedure; attempt basic output if setup is ready. Otherwise stop at a reproducible setup note. |
| Week 1, rest day | No required study | No catch-up obligation. |
| Week 2, day 1 | Delayed C16/C03 check; C23 introduction | Check whether at least seven days have elapsed since the first successful check before considering Done. Read standard concepts and requires. |
| Week 2, day 2 | C24 | Attempt [E04](#e04): a pipeline with clear ownership and one projection. |
| Week 2, day 3 | C14/C15/C21/C29 | Compare optional, variant, expected and exceptions using [E05](#e05); focus on choices rather than all APIs. |
| Week 2, day 4 | B01/B02/P02 | Continue the YD-RP2040 setup and a timer-driven output. Begin [E10](#e10) when the build/flash path is ready. |
| Week 2, day 5 | I01 plus weekly review | Reconstruct one small digital-sum case from [E06](#e06), explain complexity, then choose next week's weakest useful topic. |
| Week 2, optional practical session | Continue P02 | Extend the board demonstration or document the next setup step. |
| Week 2, rest day | No required study | Resume with the next useful action. |

A failed baseline is useful information. Keep the affected topic InProgress and reduce the next task's size. Reading advanced material ahead of the core chat is already recorded; it need not be reread immediately just to make the route look linear.

<a id="projects"></a>
## Projects and later milestones

### P02: YD-RP2040 embedded demonstration

The board is an RP2040 microcontroller target, rather than a Linux Raspberry Pi computer. Confirm its actual board revision, pinout, flash configuration and SDK support before hardware-specific examples. Use the [Pico C/C++ SDK](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html) and [RP2040 documentation](https://www.raspberrypi.com/documentation/microcontrollers/microcontroller-chips.html).

Progress through:

1. Reproducible cross-build/flash and basic output.
2. A nonblocking timer-driven state machine using bounded storage.
3. A producer/consumer buffer with an explicit full-buffer policy.
4. Interrupt or peripheral integration, then multicore coordination if useful.
5. A small report of timing, memory use and failure behaviour.

Use simulated input if no extra sensor is available; purchases are not a prerequisite. SDK synchronization and interrupt primitives are target-specific; do not assume desktop std::thread, the full hosted library or all C++26 facilities are available.

The desktop L1/L2/L3 model is not a portable description of every target. Inspect actual hardware. RP2040's Cortex-M0+ has no hardware FPU, so floating-point costs differ substantially from the host; use the [vendor's explanation](https://www.raspberrypi.com/news/raspberry-pi-silicon-pico-now-on-sale/) when designing numerical experiments.

### P01: slot-game maths simulator

Use a C# reference implementation, supported by independently checkable small examples. Do not assume a reference implementation is automatically mathematically correct.

| Milestone | Deliverable | Acceptance evidence |
|---|---|---|
| S1: deterministic reference | A small shareable specification, fixed reel stops/outcomes or other explicit inputs, expected payouts and invalid-input cases | Exact per-case agreement where arithmetic is exact; ownership/error decisions documented |
| S2: sequential model | Correct single-thread C++ calculation and a tiny model that can be fully enumerated | Enumerated probabilities normalize; expected payout agrees with an independent calculation |
| S3: simulation statistics | Seed policy, trial count, payout totals, RTP definition, variance/uncertainty and run metadata | Zero trials and invalid stakes handled; overflow bounds justified; sample-estimation tests separate from exact logic tests |
| S4: parallel batches | A sequential oracle, stable batch IDs, documented RNG stream strategy and a reduction | Each batch processed once; no shared mutable RNG; correct totals for deterministic fixtures; repeatability for the documented configuration |
| S5: measured optimization | Profile evidence and one measured change at a time | Correctness retained; report absolute throughput and scaling with hardware, flags, allocation policy and workload |

For fixed stakes, define the simulator's RTP explicitly as total payout divided by total stake. Keep monetary units integral when appropriate and check accumulator bounds. Select numerically stable methods for moments/variance, especially when payouts have a wide range.

For C#/C++ comparisons, replay identical input fixtures first. Identical seeds do not establish identical streams across runtimes, standard-library distributions or implementations. Different seeds alone are not proof of independent streams. State the engine and stream-partitioning assumptions, and make stable batch identity independent of worker scheduling if that reproducibility is required.

Statistical tests must acknowledge sampling variation. Do not require every nominal confidence interval from every run to contain the true value. Use a tiny enumerated distribution, fixed reproducible test inputs and a separately designed multi-run statistical check. Document limitations for rare payouts. A statistical agreement cannot repair an incorrect payout evaluator.

### P03: modernize a legacy convex-optimization project

Begin after the intended core-language reading pass and a practical check of ownership, templates/concepts and numerical foundations. Select a suitable legacy codebase, then inspect its algorithms, dependencies and intended tolerances.

| Milestone | Deliverable | Acceptance evidence |
|---|---|---|
| O1: baseline | Algorithm/dependency inventory, supported inputs and representative problem fixtures | Original outputs, termination reasons, iterations, residuals and timings recorded |
| O2: safe modern interfaces | Explicit owners/views, error reporting, generic constraints where useful | Original mathematical behaviour preserved; lifetime and invalid-input checks |
| O3: one kernel at a time | Layout, allocation or dispatch experiment | Same problem set; absolute/relative tolerances justified by conditioning; report objective, feasibility/residuals and convergence |
| O4: performance work | Profile-led cache, pool, SIMD or parallel change | Accuracy and termination retained; peak memory, allocation counts and timings compared |
| O5: shareable result | Small reproducible example and explanation of tradeoffs | Compiler/library/CPU recorded; changed numerical behaviour explicitly justified |

Include well-conditioned, ill-conditioned, degenerate and invalid/infeasible cases where the existing solver supports those categories. Use analytically solvable small problems where possible. Avoid treating bit-for-bit floating-point identity as the only correctness measure, or widening tolerances merely to hide regressions.

Use C++26 where the chosen compiler and libraries support it. “Modern” can mean clear RAII, views and constrained interfaces; it does not require reflection, a custom expression-template library or a replacement for a well-tested linear-algebra backend.

### Keeping project work manageable

Project order: **YD-RP2040 (P02) first**, then the slot simulator (P01), followed by convex optimization (P03). Keep one substantial project active at a time. Topic IDs remain stable and do not indicate priority.

For interview preparation, compare the curriculum with a relevant job description and choose one useful capability to demonstrate. Examples include a tested C++ task, a board demonstration or a numerical correctness report. Completing every C++26 topic is not a prerequisite for job applications.


<a id="exercises"></a>
## Exercise briefs

This is a menu, not a requirement to write 18 new programs. Choose an exercise when it answers a real question or produces useful job/project evidence. You can adapt an existing task. No exercise implementation is added by this document.

<a id="e01"></a>
### E01 — owning strings and borrowed views

- **Purpose:** Avoid dangling views while using allocation-free slices where their lifetime is safe.
- **Prerequisites/topics:** C03, C16; introduce C22 only after string_view is clear.
- **Task/output:** Design a function that returns a token view into caller-owned text. Predict which of four sources remain valid: a literal, a live string, a temporary string and a local string returned from a function.
- **Checks:** Empty input, missing separator, substring without a trailing null, source mutation/reallocation and a safe owning alternative. Identify dangling cases without deliberately dereferencing invalid storage. Sanitizers can help, but silence does not prove safety.
- **C# comparison:** ReadOnlySpan<char> offers slicing with different lifetime restrictions and character representation. State the text encoding assumptions.

<a id="e02"></a>
### E02 — ownership, movement and destruction

- **Purpose:** Make resource cleanup and ownership transfer predictable.
- **Prerequisites/topics:** C02–C05, C21.
- **Task/output:** Predict constructor/destructor events for a small resource owner; inspect the existing shared_ptr cycle, then design a non-owning link where appropriate.
- **Checks:** Normal exit, error exit, copy/move decisions, moved-from validity and destruction exactly once for each owned resource. Do not require a moved-from string to be empty.
- **C# comparison:** Contrast IDisposable/using and reference assignment with deterministic C++ destruction and explicit copy/move semantics.

<a id="e03"></a>
### E03 — deduction, forwarding and cast boundaries

- **Purpose:** Choose parameter and return types without accidental copies or invalid references.
- **Prerequisites/topics:** C05/C06/C09/C13/C20.
- **Task/output:** Predict auto/auto&/auto&&/decltype(auto) for mutable lvalues, const lvalues and temporaries. Select a safe parameter form for each case.
- **Checks:** Verify predictions with static_assert; explain named rvalue references, decay, move versus forward, and why const_cast cannot make a genuinely const object mutable.
- **C# comparison:** Contrast C# var, ref/in parameters and value/reference types; do not infer C++ copy semantics from familiar spelling.

<a id="e04"></a>
### E04 — a constrained range pipeline

- **Purpose:** Compose readable generic operations while preserving valid storage and traversal assumptions.
- **Prerequisites/topics:** C19/C22/C23/C24.
- **Task/output:** Filter and project values from an owned collection; create a function requiring only the operations it uses.
- **Checks:** Empty input, const input, source lifetime and mutation, repeated traversal, and when materialization is necessary. Start with standard range concepts.
- **C# comparison:** Compare LINQ's deferred evaluation with C++ views, emphasizing ownership and iterator categories.

<a id="e05"></a>
### E05 — represent absence, alternatives and failure

- **Purpose:** Make callers handle the distinctions that matter to the domain.
- **Prerequisites/topics:** C14/C15/C21/C29.
- **Task/output:** Design a tiny parser/query that distinguishes “missing”, “malformed” and a valid result. Compare optional, variant, expected and exception-based interfaces.
- **Checks:** Valid input, absent value, domain error, malformed input and complete visitor coverage. Explain why absence alone cannot carry a detailed error.
- **C# comparison:** Compare nullable values, TryParse, exceptions and an explicit result type; no single mechanism maps identically.

<a id="e06"></a>
### E06 — reconstruct maximum digital sum

- **Purpose:** Practise interview reasoning and explicit large-number storage using an existing task.
- **Prerequisites/topics:** C17/C19/N01/I01.
- **Task/output:** Re-solve a small bounded version of the existing task from a blank page before consulting the stored solution.
- **Checks:** Hand-computed powers, carry propagation, storage bounds, exponent/base boundaries and comparison with C# BigInteger. State complexity in both numeric parameters and digit count.
- **C# comparison:** Explain what BigInteger provides that a vector of digits must implement explicitly. Measure only after both are correct.

<a id="e07"></a>
### E07 — a deterministic slot-maths slice

- **Purpose:** Establish a trusted payout evaluator before adding random sampling or parallelism.
- **Prerequisites/topics:** C02/C17/C21/C29/N01/P01.
- **Task/output:** Extract a minimal shareable specification and fixed outcome inputs from the C# model; implement only the deterministic payout calculation when you choose to code.
- **Checks:** Exact known results, no-win/win/boundary cases, invalid inputs and accumulator limits. A tiny independently enumerable game validates the oracle.
- **C# comparison:** Use the same input fixture and numerical units. Random generators are outside this first comparison.

<a id="e08"></a>
### E08 — sampling and numerical estimates

- **Purpose:** Distinguish model correctness from Monte Carlo sampling uncertainty.
- **Prerequisites/topics:** N01/N02, E07.
- **Task/output:** Simulate a small enumerated distribution; report trial count, mean payout, variance, uncertainty and seed/engine metadata.
- **Checks:** Exact evaluator tests, zero trials, overflow, deterministic replay, and a predeclared statistical check across multiple runs. Explain why larger samples do not fix a biased model.
- **C# comparison:** Compare results on fixed samples first; different RNG/distribution implementations need not emit the same sequence.

<a id="e09"></a>
### E09 — parallelize independent batches

- **Purpose:** Improve throughput while preserving coverage, correctness and a documented reproducibility policy.
- **Prerequisites/topics:** N03/N04/N05/N06, a correct sequential E07/E08 baseline.
- **Task/output:** Use stable batch IDs and per-batch state; aggregate results without a shared mutable RNG or hot shared counter.
- **Checks:** One worker, several workers, uneven final batch, cancellation, worker errors and no omitted/duplicate work. Document ordering and floating-point reproducibility limits. Use race detection when supported.
- **C# comparison:** Compare with Parallel.For/Tasks using equal work and data; include scheduling, warm-up and allocation costs in the benchmark description.

<a id="e10"></a>
### E10 — a nonblocking board task

- **Purpose:** Demonstrate basic embedded timing and deployment skills on the actual board.
- **Prerequisites/topics:** C02/C03/C31/B01/B02.
- **Task/output:** Build and flash a timer-driven state machine with observable output, using the verified board configuration.
- **Checks:** Timing, bounded storage, startup, state transitions and no blocking work in a latency-sensitive path. Record SDK/toolchain and the method used to observe timing.
- **C# comparison:** Relate the state machine to familiar application logic while explaining why hardware timers and register access change the execution model.

<a id="e11"></a>
### E11 — bounded producer/consumer storage

- **Purpose:** Make overload and communication behaviour explicit under fixed memory constraints.
- **Prerequisites/topics:** C22/B03; N03/N04 or target synchronization knowledge.
- **Task/output:** Specify and test a fixed-capacity buffer on the host before connecting an interrupt/peripheral producer.
- **Checks:** Empty/full, wraparound, sustained overload, ownership and an explicit drop/reject/backpressure policy. For concurrent variants, justify synchronization and supported producer/consumer counts.
- **C# comparison:** Compare with a bounded channel/queue; its implementation does not transfer directly to an interrupt context.

<a id="e12"></a>
### E12 — static versus dynamic polymorphism

- **Purpose:** Choose a dispatch design using measured tradeoffs and interface requirements.
- **Prerequisites/topics:** C09/C23/A01/N06; after core reading.
- **Task/output:** Express one interchangeable numerical operation with virtual dispatch and with a constrained callable/policy. Add CRTP only if it solves a specific problem.
- **Checks:** Same results, concrete call sites, optimized assembly where useful, build time, code size and performance. Explain whether devirtualization or inlining occurred.
- **C# comparison:** Compare interfaces, generics and static abstract interface members where applicable. Do not assume either language or dispatch form must win.

<a id="e13"></a>
### E13 — locality and cache experiments

- **Purpose:** Connect data layout and working-set size to measured memory-access costs.
- **Prerequisites/topics:** C17/N06/A02; after core reading.
- **Task/output:** Measure row/column traversal, tiled versus untiled access, and AoS/SoA for a workload that needs only selected fields.
- **Checks:** Inspect the host's cache topology; test several working-set sizes, preserve observable results, repeat measurements and report bandwidth/throughput. Use counters only where available. Add a separate false-sharing experiment after N03/N04.
- **C# comparison:** Compare equivalent data layouts and algorithms; separate allocation/GC effects from memory-access effects. No assumed universal cache-line size or automatic L3 availability.

<a id="e14"></a>
### E14 — allocation budget and resource lifetime

- **Purpose:** Understand when reuse reduces allocation cost and when it retains or wastes memory.
- **Prerequisites/topics:** C03/C17/C21/A03/N06; after core reading.
- **Task/output:** Compare ordinary allocation, reserved storage, a monotonic resource and an appropriate pool for one bounded workload. Add a counting resource.
- **Checks:** Resource outlives its users; alignment; element-internal allocations; repeated container growth; retained/peak memory; internal/external fragmentation; release invalidation; upstream fallback and deliberate exhaustion. Compare a fixed-size pool with a variable-size allocation workload. A fixed initial buffer alone does not prevent heap fallback; test an explicit no-fallback setup. Test threading separately for synchronized/unsynchronized resources.
- **C# comparison:** Compare ArrayPool/MemoryPool and explicit buffer ownership, including returns and accidental retention; pooled storage does not by itself guarantee lower total cost.

<a id="e15"></a>
### E15 — modernize one numerical kernel

- **Purpose:** Improve an existing solver incrementally without concealing numerical regressions.
- **Prerequisites/topics:** N01/N06/A04/P03; a legacy codebase available.
- **Task/output:** Choose one existing kernel, document its layout/ownership and preserve a sequential reference while introducing safer interfaces.
- **Checks:** Small analytic cases, dimensions/strides, aliasing, absolute and relative error, conditioning, allocations and timing. For a solver step, also check feasibility/residuals and convergence.
- **C# comparison:** Compare the same algorithm and precision where a reference exists. Do not invent a C# convex solver merely to populate the comparison column.

<a id="e16"></a>
### E16 — vectorization without losing correctness

- **Purpose:** Determine whether SIMD improves a relevant kernel under its required numerical semantics.
- **Prerequisites/topics:** N01/N06/A02/A04/A05.
- **Task/output:** Compare scalar, auto-vectorized and supported explicit-SIMD versions of one suitable kernel.
- **Checks:** Empty input, tail lengths, alignment assumptions, non-finite values where supported, numerical tolerance and actual compiler output. Preserve required floating-point semantics before experimenting with relaxed flags.
- **C# comparison:** Compare vector/intrinsic approaches with the same semantics and host hardware; record runtime/JIT conditions.

<a id="e17"></a>
### E17 — generator lifetime and consumption

- **Purpose:** Use lazy synchronous production without confusing suspension with ownership or scheduling.
- **Prerequisites/topics:** C03/C05/C24/C26.
- **Task/output:** Reconstruct a short synchronous generator pipeline; identify what owns each yielded value and what remains valid at suspension points.
- **Checks:** Empty sequence, early exit, moved generator and referenced external storage. Distinguish single-pass consumption from a reusable collection. Do not infer asynchronous execution from co_yield.
- **C# comparison:** Compare yield return/IEnumerable with the particular C++ generator's ownership and iteration rules.

<a id="e18"></a>
### E18 — a narrowly scoped C++26 probe

- **Purpose:** Separate a feature's usefulness and standard semantics from its toolchain availability.
- **Prerequisites/topics:** C31 plus the selected X topic's prerequisites.
- **Task/output:** Choose one useful feature, such as inplace_vector or a small reflection query. Record a minimal example, compiler/library/version, flags and relevant feature-test macro.
- **Checks:** Confirm semantics from primary documentation; distinguish unsupported syntax/library availability from an error in your reasoning. Explain a simpler implementation available in an older standard.
- **C# comparison:** Compare intent, such as fixed-capacity storage or compile-time versus runtime reflection, rather than assuming matching syntax or runtime cost.

<a id="csharp-comparisons"></a>
## C# comparison map

Use these as prompts, not one-to-one translations. Implement a C# counterpart only when it clarifies semantics or supplies a useful correctness/performance reference.

| C++ topic | Familiar C# reference | Difference to explain |
|---|---|---|
| RAII and destructors | IDisposable, using, SafeHandle | C++ destruction follows object lifetime; a C# finalizer is not the same mechanism |
| Copy/move and class values | Struct copying and class references | C++ class assignment can copy/move values/resources; std::move is a cast, not an operation guaranteed to transfer resources |
| const/reference qualifiers | readonly, in, ref | Rules apply at different levels; C++ constness does not make an arbitrary object graph immutable |
| string_view/span | ReadOnlySpan/Span | Ownership, escape restrictions, invalidation and text encodings differ |
| Templates/concepts | Generics/constraints | Instantiation, specialization, code generation and semantic requirements differ |
| Lambdas/callables | Delegates/closures | Capture lifetime and concrete closure types matter; type erasure can add costs |
| Ranges/views | LINQ | Both can defer work; iterator capabilities, borrowing and invalidation need explicit reasoning |
| optional/variant/expected | Nullable values, pattern matching, result types | No universal direct equivalent; visitors and error alternatives have different rules |
| generator/co_await | yield return and async/await | Coroutine machinery does not itself supply a scheduler; generator is synchronous |
| Threads/atomics | Task, Parallel, lock, Interlocked, Volatile | Task-based and thread-based execution differ; C++ memory-order reasoning is explicit |
| Static polymorphism | Generics and static abstract members | Compare dispatch and generated code; CRTP has no identical language-level translation |
| Memory pools | ArrayPool/MemoryPool | Resource lifetime, deallocation policy and nested allocations remain design choices |
| Reflection | System.Reflection | C++26 static reflection and typical C# runtime reflection operate at different times |
| Benchmarks | Stopwatch and managed benchmarking tools | Compare equal algorithms/data; record optimized builds, warm-up, allocations and runtime conditions |

Useful official comparison references: [C# efficient memory use](https://learn.microsoft.com/en-us/dotnet/csharp/write-safe-efficient-code), [generics](https://learn.microsoft.com/en-us/dotnet/csharp/fundamentals/types/generics), [IDisposable](https://learn.microsoft.com/en-us/dotnet/api/system.idisposable), and [task parallelism](https://learn.microsoft.com/en-us/dotnet/standard/parallel-programming/task-parallel-library-tpl).


<a id="review-routine"></a>
## Review routine and sustainable workload

The suggested routine is five 60–90-minute sessions, one optional 90–120-minute practical session and one day without required C++ study. Adjust it to available time and energy.

### A normal session

For 60 minutes: 10 minutes of closed-notes recall, 35 minutes of the main task, 10 minutes checking results/explaining a difference from C#, and 5 minutes logging and choosing the next action. For 90 minutes, extend the main task by 25 minutes and add a 5-minute break. Routine recall stays within 15 minutes.

Reading, coding, reviewing and making notes all use the same budget. Video production and maintaining the tracker also count as study work. Missed sessions do not create catch-up debt.

### Review intervals

Practice retrieval and spaced practice have stronger general support than repeated rereading alone. The schedule below is a practical default, not a research-proven optimum for every C++ topic. [Dunlosky et al., learning-techniques review](https://journals.sagepub.com/doi/10.1177/1529100612453266)

| Approximate time after the first successful learning check | Review task |
|---|---|
| Day 1 | Explain the rule and predict a tiny example, closed notes |
| Day 3 | Solve a changed case or identify an error |
| Day 7 | Repeat explanation plus a small application independently; eligible for Done only if at least seven days separate successful checks |
| Day 14 | Mix with another topic, such as ownership plus ranges |
| Day 30 | Reconstruct or use it in an actual task |
| Day 60 | Sample retention; then revisit when needed or during monthly mixed checks |

Use the actual date of the successful check, not this document's creation date, to start a topic's schedule. Initially there are no fabricated due dates. If a review is Partial/Retry, read only the missing piece and schedule another independent check in 1–3 days; return to wider spacing when successful.

If a date is missed, do one useful review in the next available session and reschedule from there. Do not stack several old reviews on the same day. When the review queue exceeds 15 minutes, pick the most relevant weak topics and postpone new material. An easy topic used successfully in a project can supply a logged applied review.

### Weekly and monthly checks

- **Weekly, inside the last session:** spend about 20 minutes on two older topics and one recent topic; inspect the mistake log; choose one language focus and one applied focus for next week.
- **Monthly, replacing a normal session:** reconstruct a small useful component, explain its lifetime/error/performance tradeoffs and compare it with a C# approach.
- **After a demanding month:** use a lighter consolidation week if needed. Reduce new material before cutting sleep, exercise or time away from work.
- **Busy-week fallback:** three 30-minute sessions: one recall/repair session, one small practical task, one mixed review and next-step note. Resume normal study without repayment.
- **Context switching:** park unrelated questions in a short list. A C# comparison can be one paragraph; it does not require another implementation of every exercise.

Keep only 3–5 useful recall prompts per active topic. Do not create flashcards for every line of an AI answer. Use mistakes that recur to choose what deserves another prompt.

<a id="templates"></a>
## Session, mistake and AI templates

### Session log

Keep this log append-only; use explicit correction rows when necessary.

No tracked study sessions have been recorded yet. Historical chat interaction and repository creation are not retrospective successful reviews.

Log one row per activity using the same session ID for a session that contains several activities. Use F for first reading, R for rereading, Q for recall and C for independent coding/debugging/design. F does not increment a repetition counter. A new topic may become Read after F; an already-read topic increments R when reread.

| Date / session ID | Topic | Activity F/R/Q/C | Result Pass/Partial/Retry/— | Assistance used | Evidence / observation |
|---|---|---|---|---|---|

Copy and fill this row; placeholders are not actual study data:

```text
YYYY-MM-DD / session-A | C16 | Q | Pass/Partial/Retry | none/notes/hint/API lookup | what was checked; link; mistake
```

Counter arithmetic:

- Each logged R adds 1 to R.
- Each Q adds 1 to Q attempts; Q/Pass also adds 1 to Q passes.
- Each C adds 1 to C attempts; C/Pass also adds 1 to C passes.
- F and planning-only activity add nothing to R/Q/C.
- Passes cannot exceed attempts. The First Done snapshot cannot exceed later cumulative counts.
- Use a correction row if a historical log entry was wrong; do not silently rewrite the learning history.

### Mistake log

| Topic | My incorrect prediction | Correct rule in my words | Minimal counterexample / source | Next check |
|---|---|---|---|---|

Capture one specific misunderstanding, not an entire copied explanation. On the next check, reproduce the counterexample from memory.

### One-screen topic note

```text
Topic ID:
What problem does it solve?
Ownership/lifetime and error rules:
One minimal example I can explain:
One trap/counterexample:
C# similarity and important difference:
Compiler/standard requirement:
Three recall prompts:
Reliable reference:
Next action:
```

Keep long existing notes as references. This short note is a retrieval aid written after attempting recall; rewriting every long note is not another project.

### AI quiz prompt

```text
Quiz me on topic [ID] at difficulty [1–5].
Ask one question at a time. Do not show the answer before my attempt.
Include a prediction/debugging case and a practical design choice.
After I answer, identify the precise mistake and ask me to repair it.
Clearly label any hint; a hinted answer is not an independent pass.
Check version-sensitive claims against primary documentation.
At the end, suggest Pass/Partial/Retry with evidence, but do not
invent study dates or update my counters without actual logged attempts.
```

### AI code-review prompt

```text
Review my attempt for [topic ID] against the stated exercise checks.
First examine correctness, ownership/lifetime, error paths and edge cases.
Then assess complexity and performance assumptions.
Ask me to explain the suspicious part before supplying replacement code.
Distinguish measured results from predictions and standard guarantees
from observed behaviour. Keep the smallest useful correction.
```

### Sharing study results

This file is the index; existing notes stay under `docs/cpp/` and selected experiments stay under `tasks/`. Add relative evidence links after you actually write or run something. A small meaningful commit can record one learning result and its tracker update; no publishing action is implied by editing this document.

For an optional 3–5-minute explanation or video:

1. State the problem and your original assumption.
2. Explain the C++ rule and a C# contrast from memory.
3. Demonstrate a tiny verified case or show measured results.
4. State one limitation and the next question.

A recording attempt counts as Q only when it actually tests unaided explanation; filming, editing and uploading do not generate extra repetitions. Include assistance honestly. Use shareable examples rather than assuming work-project code or game details are public.

The source-note generation prompt is available at [gen_doc_prompt.md](gen_doc_prompt.md); generated notes remain material to verify, not proof that a topic has been learned.

<a id="version-guide"></a>
## Version and toolchain guide

This is a map of major topics relevant to your goals, not every standards-paper wording change. The title “C++26” in an existing note does not mean every feature in it first appeared in C++26.

| Version | Main topics in this program | IDs |
|---|---|---|
| Before C++11 | RAII, templates, virtual dispatch, named casts, much of STL, algorithms and complexity | C01/C03/C08/C09/C17–C21, I01–I04 |
| C++11 | Move/forwarding, smart pointers, auto/decltype, lambdas, aliases/variadics, constexpr, nullptr, enum class, atomics/threads/random | C01–C10, C13, N02–N04 |
| C++14 | make_unique, generic lambdas/init-capture, auto return deduction, decltype(auto), variable templates, relaxed constexpr | C03/C06/C07/C10/C30 |
| C++17 | Folds, if constexpr, CTAD/bindings, optional/variant/any/string_view, pmr, filesystem/charconv, parallel algorithms | C11–C16/C25/C30, N05, A03 |
| C++20 | Concepts/ranges/span, coroutine language support, modules, consteval/constinit, designated initializers, comparisons/bit/chrono, format, jthread and coordination | C02/C10/C20/C22–C28/C30, N03/N04 |
| C++23 | expected, generator, print, ranges extensions, mdspan, if consteval, scope_exit, explicit object parameters, move_only_function | C10/C14/C24–C26/C29/C30, A01/A04 |
| C++26 | Reflection/expansion statements, contracts, pack/binding extensions, inplace_vector/hive, #embed, execution control, simd/linalg, selected constexpr/library/callable additions | X01–X07, C15/C17/C30, A04/A05 |

Classic techniques such as CRTP, cache blocking and pools are not C++26 inventions. Recognizing when they help is a separate skill from learning their newest library support.

### Current repository facts

- [CMakeLists.txt](../CMakeLists.txt) sets `CMAKE_CXX_STANDARD 26` and `CMAKE_CXX_STANDARD_REQUIRED ON`. Despite comments and older README wording, this is a requirement, not an automatic fallback to C++20/23.
- [tasks/CMakeLists.txt](../tasks/CMakeLists.txt) discovers simple tasks at configure time. Reconfigure after adding a new task. Build one selected target to avoid unrelated experimental features blocking practice.
- [GCC-specific targets](../tasks/gcc/CMakeLists.txt) include reflection flags. Availability depends on compiler **and standard library**, target, configuration and feature.
- Language features, hosted library facilities and microcontroller support are separate questions. A host compiler accepting `-std=c++26` does not establish full library or RP2040 support.

With a suitable C++26 compiler selected, build a single target from the repository root:

```sh
cmake -S . -B build-training
cmake --build build-training --target hello_task
```

Select the compiler using the local CMake configuration and record its compiler/library versions. The executable location depends on the build generator and platform.

For each experimental feature, record: standard introduction, compiler version, library version, target, needed flags, feature-test macro where applicable and actual build/run result. Mark “unsupported on this toolchain” in evidence instead of treating a compilation failure as failed understanding.

Consult [GCC language support](https://gcc.gnu.org/projects/cxx-status.html), [GCC 16 changes](https://gcc.gnu.org/gcc-16/changes.html), [libc++ C++26 status](https://libcxx.llvm.org/Status/Cxx26.html), and [WG21 papers](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/) for version-sensitive claims. Compiler documentation can change; save the checked date alongside an exercise result.

<a id="sources"></a>
## Sources and maintenance

### Learning evidence and research

- Core-lesson reading checkpoint: section 20, recorded in [Current position and evidence](#core-chat-checkpoint).
- All 16 existing C++ notes are linked from the progress table. Overlapping notes and examples do not create extra repetitions.
- U1–U4 and survey context are documented with populations and dates in [Published usage evidence](#usage-evidence). No fresh public-repository usage scan is part of this document.
- [Dunlosky et al. (2013)](https://journals.sagepub.com/doi/10.1177/1529100612453266): basis for retrieval and distributed practice; the particular calendar here is a practical default.

### Technical references for later work

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines): ownership, resource management and interfaces.
- [Pico SDK](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html) and [RP2040 documentation](https://www.raspberrypi.com/documentation/microcontrollers/microcontroller-chips.html): use target documentation for hardware decisions.
- [Intel optimization manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel64-and-ia32-architectures-optimization.html): an x86 reference for cache/performance concepts; consult target-specific documentation for other architectures.
- [Eigen aliasing guidance](https://eigen.tuxfamily.org/dox/group__TopicAliasing.html): a concrete example of lazy-expression correctness hazards, not a decision to adopt Eigen.
- The compiler/library and C# references above support further checking; none establish the performance of your own code.

### Maintaining this document

At a weekly check, update only changed topic states, actual repetition results and the next few actions. Review difficulty scores when a topic is consistently easier/harder than expected. Update empirical usage findings only when a source supports the same feature and metric; retain study dates and population definitions.

No historical success, completion date or repetition is inferred from file timestamps, conversation length or the existence of AI-generated code. Keep facts, assessments, proposed work and missing evidence visibly separate.
