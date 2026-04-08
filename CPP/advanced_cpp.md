# advanced_cpp.md

This document covers advanced and niche C++ topics that are often skipped in basic preparation but are very useful for senior interviews, library work, performance-sensitive code, and embedded or systems programming.

It is not meant to replace `modern_cpp.md`. Instead, it extends it with deeper topics such as:

- memory ordering
- template metaprogramming ideas
- advanced ownership and forwarding rules
- coroutines
- allocators and `std::pmr`
- modules
- compile-time techniques

---

## Atomic Memory Ordering

Many people know `std::atomic`, but fewer understand memory ordering.

Atomic variables solve two related but different problems:

- atomicity: an operation happens indivisibly
- ordering/visibility: when writes from one thread become visible to another

### Common Memory Orders

#### `memory_order_relaxed`

Provides atomicity, but very weak ordering guarantees.

```cpp
#include <atomic>
using namespace std;

atomic<int> counter{0};

void increment() {
    counter.fetch_add(1, memory_order_relaxed);
}
```

Use when:

- you only need atomic updates
- you do not need synchronization with other data

Typical example:

- statistics counters
- telemetry counters

#### `memory_order_acquire` and `memory_order_release`

These are usually used together for producer-consumer style synchronization.

```cpp
#include <atomic>
using namespace std;

atomic<bool> ready{false};
int data = 0;

void producer() {
    data = 42;
    ready.store(true, memory_order_release);
}

void consumer() {
    if (ready.load(memory_order_acquire)) {
        // guaranteed to see data = 42 here
    }
}
```

Idea:

- release makes prior writes visible
- acquire ensures subsequent reads observe those writes

#### `memory_order_seq_cst`

This is the strongest and simplest memory order to reason about.

```cpp
atomic<int> x{0};
x.store(1, memory_order_seq_cst);
```

Use when:

- correctness matters more than squeezing every bit of performance
- you want the simplest mental model

Interview point:

- most code should not jump to weak memory orders unless there is a real need and strong understanding

---

## False Sharing

False sharing happens when separate variables used by different threads live on the same cache line.

Even if threads are touching different variables, performance may suffer because the cache line keeps bouncing between CPU cores.

```cpp
struct Counters {
    int a;
    int b;
};
```

If two threads repeatedly update `a` and `b`, they may still interfere if both values sit in the same cache line.

A mitigation is alignment or padding.

```cpp
struct alignas(64) PaddedCounter {
    int value;
};
```

Why it matters:

- does not break correctness
- can seriously hurt multithreaded performance

---

## `std::atomic` Is Not a Lock

Atomics are powerful, but they do not automatically replace mutexes.

Atomics are good for:

- counters
- flags
- simple state transitions

Atomics are not enough when:

- multiple variables must change together
- invariants span several fields
- operations are logically bigger than one atomic step

Example:

```cpp
struct Account {
    atomic<int> balance{0};
    atomic<int> version{0};
};
```

Even if each field is atomic, updating them consistently together may still need stronger synchronization.

---

## Perfect Forwarding and Reference Collapsing

This is a key topic in advanced generic programming.

### Forwarding Reference

When a template parameter is declared as `T&&` in a deduced context, it becomes a forwarding reference.

```cpp
template <typename T>
void wrapper(T&& value) {
    process(std::forward<T>(value));
}
```

Why not always use `std::move`?

- `std::move` always turns the argument into an rvalue
- `std::forward<T>` preserves whether the original caller passed an lvalue or rvalue

### Reference Collapsing Rules

In template instantiation:

- `T& &` becomes `T&`
- `T& &&` becomes `T&`
- `T&& &` becomes `T&`
- `T&& &&` becomes `T&&`

This is what makes forwarding references work.

### Common Trap

```cpp
template <typename T>
void badWrapper(T&& value) {
    process(std::move(value));   // wrong for lvalues
}
```

Problem:

- if caller passed an lvalue, this code may move from it unexpectedly

---

## SFINAE

SFINAE stands for:

- Substitution Failure Is Not An Error

It is a template rule where invalid template substitutions can silently remove candidates from overload resolution instead of causing hard compile errors.

Classic example:

```cpp
#include <type_traits>
using namespace std;

template <typename T>
enable_if_t<is_integral_v<T>, T> addOne(T value) {
    return value + 1;
}
```

Here, the function participates only if `T` is an integral type.

Why it matters:

- older generic libraries relied on SFINAE heavily
- many legacy codebases still use it

Modern replacement:

- concepts are often clearer than SFINAE

---

## Concepts

Concepts are a cleaner modern way to constrain templates.

```cpp
template <typename T>
concept Incrementable = requires(T x) {
    ++x;
};

template <Incrementable T>
T nextValue(T x) {
    return ++x;
}
```

Why concepts are better than old-style SFINAE:

- clearer error messages
- more readable intent
- easier maintenance

---

## Type Traits

Type traits are compile-time utilities that describe or transform types.

Examples:

- `std::is_integral_v<T>`
- `std::is_same_v<A, B>`
- `std::remove_reference_t<T>`
- `std::decay_t<T>`

```cpp
#include <type_traits>
using namespace std;

static_assert(is_integral_v<int>);
static_assert(is_same_v<remove_reference_t<int&>, int>);
```

Why they matter:

- they are building blocks of generic programming
- they appear often in template-heavy libraries

---

## CRTP

CRTP means Curiously Recurring Template Pattern.

It looks unusual at first:

```cpp
template <typename Derived>
class Printable {
public:
    void print() const {
        static_cast<const Derived*>(this)->printImpl();
    }
};

class Report : public Printable<Report> {
public:
    void printImpl() const {
    }
};
```

Why use CRTP:

- static polymorphism without virtual dispatch
- compile-time interface reuse
- mixin-style behavior

Use cases:

- expression templates
- mixins
- performance-sensitive code

Tradeoff:

- harder to read than normal inheritance

---

## ADL

ADL means Argument-Dependent Lookup.

When you call a function, C++ may search not only the current scope but also the namespaces associated with the argument types.

```cpp
namespace mathlib {
    struct Vec {};
    void normalize(Vec) {}
}

mathlib::Vec v;
normalize(v);   // ADL may find mathlib::normalize
```

Why it matters:

- important for operator overloads
- important in generic code
- can create surprising lookup behavior

Common library advice:

- call `swap(a, b)` after `using std::swap;` so ADL can find custom swaps

---

## Placement `new`

Placement `new` constructs an object in already-allocated memory.

```cpp
#include <new>
using namespace std;

struct Widget {
    int value;
    Widget(int v) : value(v) {}
};

alignas(Widget) unsigned char buffer[sizeof(Widget)];
Widget* p = new (buffer) Widget(42);
```

Use cases:

- custom allocators
- memory pools
- embedded systems
- low-level performance code

Important caution:

- placement `new` does not allocate memory
- you may need to call the destructor manually

```cpp
p->~Widget();
```

---

## Custom Allocators and `std::pmr`

Allocators control how containers obtain memory.

This matters in:

- low-latency systems
- embedded systems
- memory-constrained environments
- applications that want predictable allocation behavior

### `std::pmr`

`std::pmr` stands for polymorphic memory resources.

It gives a more flexible allocator model.

```cpp
#include <memory_resource>
#include <vector>
using namespace std;

pmr::monotonic_buffer_resource pool;
pmr::vector<int> values{&pool};
values.push_back(1);
values.push_back(2);
```

Why it is useful:

- containers can share an allocation strategy
- memory can come from custom pools
- helps reduce allocation overhead

### Monotonic Buffer Resource

`pmr::monotonic_buffer_resource` allocates memory in growing chunks and usually frees it all together at the end.

Good for:

- short-lived batch allocations
- parsing
- request-scoped temporary memory

Tradeoff:

- individual deallocation is not the strength of this resource

---

## Coroutines

Coroutines were introduced in C++20 and allow functions to suspend and resume.

They are useful for:

- asynchronous programming
- generators
- cooperative task scheduling

Key coroutine keywords:

- `co_await`
- `co_yield`
- `co_return`

### Generator-Like Intuition

```cpp
// conceptual example
// a coroutine can yield values one by one instead of returning all at once
```

Why they matter:

- they can express async workflows more cleanly than callback chains
- they separate logical flow from manual state-machine code

Why they are considered advanced:

- coroutine types involve promise types and compiler transformations
- implementation details are significantly more complex than the syntax suggests

Interview point:

- it is enough in many interviews to explain that coroutines allow suspension/resumption without blocking a thread

---

## Exception Safety Guarantees

Advanced C++ code often talks about exception safety in levels.

### Basic Guarantee

- no resource leaks
- object remains valid
- state may change

### Strong Guarantee

- operation either succeeds completely or has no visible effect

### No-Throw Guarantee

- operation is guaranteed not to throw

Why this matters:

- copy-and-swap is popular partly because it can provide strong exception safety
- `noexcept` move operations help standard containers optimize safely

---

## `std::launder`

`std::launder` is a niche low-level utility related to object lifetime and compiler optimization assumptions.

It appears in very advanced memory-manipulation code, especially when objects are reconstructed in place.

You usually do not need it unless:

- you are doing placement `new`
- you are managing raw storage directly
- you are working on allocator or library internals

Interview note:

- know that it exists, but do not force it into normal application code

---

## `std::byte`

`std::byte` is a type for raw memory, introduced to be clearer than using `char` for byte-oriented logic.

```cpp
#include <cstddef>
using namespace std;

std::byte b{0x1F};
```

Why it matters:

- makes intent clearer in serialization and low-level memory work
- avoids pretending raw bytes are text

---

## PImpl Idiom

PImpl means Pointer to Implementation.

The idea:

- keep implementation details out of the header
- expose only a forward declaration and pointer in the class definition

Example idea:

```cpp
class Widget {
public:
    Widget();
    ~Widget();

private:
    class Impl;
    unique_ptr<Impl> impl;
};
```

Benefits:

- reduces compile-time dependencies
- hides implementation details
- improves ABI stability in some designs

Tradeoff:

- extra indirection
- slightly more complexity

---

## Modules

Modules are a modern alternative to the traditional header/include model.

Goals:

- faster builds
- less macro leakage
- clearer interfaces

Very basic idea:

```cpp
export module math;

export int add(int a, int b) {
    return a + b;
}
```

And then:

```cpp
import math;
```

Why they matter:

- they improve large-codebase hygiene
- they can reduce repeated parsing work

Reality check:

- many real projects still rely heavily on headers
- module adoption depends on toolchain maturity

---

## ABI and ODR

These are niche but very important in large systems and libraries.

### ABI

ABI stands for Application Binary Interface.

It covers binary-level compatibility such as:

- name mangling
- calling conventions
- object layout expectations

Why it matters:

- changing class layout can break binary compatibility
- library upgrades can fail even when source code still compiles

### ODR

ODR stands for One Definition Rule.

You must not provide multiple conflicting definitions of the same entity across the program.

Why it matters:

- violating ODR can create subtle undefined behavior
- common pain points come from headers, inline definitions, and templates

---

## Template Instantiation and Code Bloat

Templates are powerful, but each unique instantiation can generate code.

Example:

```cpp
template <typename T>
T multiply(T a, T b) {
    return a * b;
}
```

If used with many types, separate instantiations may be emitted.

Why it matters:

- can increase compile times
- can increase binary size

Mitigation ideas:

- reduce unnecessary template breadth
- move stable abstractions behind non-template boundaries when appropriate

---

## `enable_shared_from_this`

This utility helps an object safely create a `shared_ptr` to itself when it is already owned by `shared_ptr`.

```cpp
#include <memory>
using namespace std;

class Session : public enable_shared_from_this<Session> {
public:
    shared_ptr<Session> getSelf() {
        return shared_from_this();
    }
};
```

Why it exists:

- constructing `shared_ptr<this>` manually is dangerous because it creates a separate control block

Important condition:

- the object must already be managed by a `shared_ptr`

---

## Common Advanced Interview Traps

### `std::move` does not move by itself

It only casts to an rvalue expression. Actual moving depends on the receiving operation.

### `volatile` is not for thread safety

It is mainly for special memory access semantics, especially hardware-facing code.

### `shared_ptr` is not free

It adds reference-counting overhead and can hide poor ownership design.

### `vector` reallocation invalidates pointers and iterators

Never keep long-lived raw pointers into a `vector` if growth may happen.

### A weaker memory order is not automatically better

It may be faster, but it is also easier to get wrong.

### Generic code is not automatically efficient

Template abstractions can still create:

- copies
- code bloat
- poor diagnostics

---

## When To Study These Topics Deeply

You should prioritize these advanced areas if you want to work in:

- embedded systems
- low-latency systems
- trading systems
- database engines
- compiler or library development
- infrastructure-heavy backend systems

If your current interviews are more entry or mid-level, it is usually enough to:

- know the definitions
- understand the motivation
- explain one or two examples clearly

---

## Quick Revision One-Liners

- `memory_order_relaxed` gives atomicity without strong ordering.
- acquire-release is a common synchronization pair.
- perfect forwarding uses `std::forward`, not always `std::move`.
- SFINAE removes invalid template candidates without hard failure.
- concepts are a cleaner way to constrain templates.
- CRTP gives static polymorphism.
- `std::pmr` helps control allocation strategy.
- coroutines can suspend and resume without blocking a thread.
- modules aim to improve build hygiene over headers.
- ABI is binary compatibility, ODR is definition consistency.

---

End of advanced_cpp.md
