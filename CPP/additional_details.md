# additional_details.md

This document is a cleaned Markdown version of `notes_prep.txt`, organized for quick interview revision and corrected for formatting, encoding, and obvious C++ syntax issues.

---

## Compilation Stages

C++ compilation typically happens in four stages:

1. Preprocessing
2. Compilation
3. Assembly
4. Linking

A standard command such as `g++ main.cpp -o main` runs all stages in sequence.

### 1. Preprocessing

The preprocessor handles directives beginning with `#`, expands headers and macros, and removes comments.

```bash
g++ -E main.cpp -o main.i
```

### 2. Compilation

The compiler translates the preprocessed source into assembly and checks syntax and semantic correctness.

```bash
g++ -S main.i -o main.s
```

### 3. Assembly

The assembler converts assembly instructions into machine code inside an object file.

```bash
g++ -c main.s -o main.o
```

### 4. Linking

The linker combines object files and libraries into the final executable.

```bash
g++ main.o -o main
```

---

## C++ Memory Layout Cheat Sheet

### Process Memory Layout

A typical C++ process is logically divided into these regions:

- stack
- heap
- `.bss`
- `.data`
- read-only data
- text segment

```text
High Address
+-----------------------------------+
| Command-line args & environment   |
+-----------------------------------+
| Stack                             |
| - Function frames                 |
| - Local variables                 |
| - Parameters                      |
| - Return addresses                |
|          grows downward           |
+-----------------------------------+
| Free virtual memory               |
+-----------------------------------+
| Heap                              |
| - new/delete                      |
| - malloc/free                     |
| - Dynamic objects                 |
|          grows upward             |
+-----------------------------------+
| BSS (.bss)                        |
| - Uninitialized globals/statics   |
+-----------------------------------+
| Data (.data)                      |
| - Initialized globals/statics     |
+-----------------------------------+
| Read-only Data (.rodata)          |
| - String literals                 |
| - Some const objects              |
+-----------------------------------+
| Text (.text)                      |
| - Machine instructions            |
+-----------------------------------+
Low Address
```

This layout is conceptual. Real placement depends on the OS, virtual memory, ASLR, and compiler behavior.

### Memory Segments

#### Text Segment (`.text`)

Stores compiled machine instructions and function bodies.

```cpp
void foo() {
    std::cout << "Hello";
}
```

Properties:

- typically read-only
- shared between processes when possible

#### Read-Only Data (`.rodata`)

Commonly stores:

- string literals
- lookup tables
- compiler metadata

```cpp
const char* str = "Hello";
```

`str` is a pointer object, while `"Hello"` usually lives in read-only memory.

#### Initialized Data (`.data`)

Stores global or static variables initialized to a non-zero value.

```cpp
int x = 10;
static int count = 5;
```

#### BSS (`.bss`)

Stores:

- uninitialized globals
- uninitialized statics
- zero-initialized globals/statics

```cpp
int x;
static int y;
int z = 0;
```

`BSS` exists so the executable does not need to store huge blocks of zeros explicitly.

#### Heap

Stores dynamically allocated objects.

```cpp
int* p = new int(10);
```

Characteristics:

- programmer-managed
- flexible size
- slower than stack allocation
- can fragment over time

#### Stack

Stores:

- local variables
- function parameters
- return addresses
- saved registers

```cpp
void foo() {
    int a = 10;
}
```

Characteristics:

- automatic allocation/deallocation
- very fast
- limited in size
- LIFO behavior

### Storage Duration vs Memory Region

These are related, but not identical:

| Storage duration | Usually stored in |
| --- | --- |
| Automatic | Stack |
| Static | `.data` / `.bss` |
| Dynamic | Heap |
| Thread-local | TLS |

The keyword determines storage duration. The exact physical placement is implementation-dependent.

### Variable Placement Examples

```cpp
int global = 10;      // .data
int global2;          // .bss
static int x = 5;     // .data
static int y;         // .bss

void foo() {
    int a = 10;       // stack
}

void bar() {
    int* p = new int(5); // p on stack, *p on heap
}

const char* str = "Hello"; // pointer may be stack or data, literal in .rodata
```

### Special Keywords

#### `static`

Changes lifetime to program duration.

```cpp
void foo() {
    static int x = 0;
}
```

Even though it appears inside a function, `x` is not recreated on every call.

#### `const`

```cpp
const int global_value = 10;
```

Global `const` objects often go into read-only storage, but placement depends on the compiler and storage duration.

#### `constexpr`

```cpp
constexpr int N = 100;
```

This is a compile-time constant and may be optimized away entirely.

#### `thread_local`

```cpp
thread_local int counter = 0;
```

Each thread gets its own instance, typically in thread-local storage.

### Lambda Captures

```cpp
int x = 10;
auto f = [x]() {
    return x;
};
```

The lambda object stores a copy of `x` as a data member. If the lambda is local, that object usually lives on the stack.

Capture by reference:

```cpp
auto f = [&x]() {
    return x;
};
```

This usually stores a reference-like member, often implemented with a pointer.

### Virtual Functions

```cpp
class Base {
public:
    virtual void foo();
    virtual ~Base() = default;
};
```

Typical implementation idea:

- each polymorphic object stores a hidden `vptr`
- the compiler emits a `vtable`
- the `vptr` points to the appropriate `vtable`

The object's `vptr` lives wherever the object itself lives. The `vtable` is usually stored in read-only memory.

### Common Memory Problems

#### Memory Leak

```cpp
int* p = new int(5);
// forgot: delete p;
```

#### Dangling Pointer

```cpp
int* p = new int(5);
delete p;
// *p = 10; // undefined behavior
```

#### Double Delete

```cpp
int* p = new int(5);
delete p;
// delete p; // undefined behavior
```

#### Stack Overflow

```cpp
void foo() {
    foo();
}
```

### Interview Prompts

- `static int x;` usually goes to `.bss`
- `static int x = 5;` usually goes to `.data`
- in `int* p = new int(10);`, the pointer is local storage if declared locally, but the integer is on the heap
- string literals like `"Hello"` usually live in read-only data

### Interview Tip

Do not stop at "where it is stored." Be ready to explain why:

- Why is stack allocation faster?
- Why do local objects get destroyed automatically?
- Why does `static` inside a function preserve value across calls?
- Why are string literals read-only?
- Why does polymorphism require a hidden `vptr`?

---

## Object Slicing

Object slicing happens when a derived object is assigned to a base object by value. The derived-only state is discarded.

```cpp
class Base {
public:
    int base_var = 1;

    virtual void print() const {
        std::cout << "Base\n";
    }
};

class Derived : public Base {
public:
    int derived_var = 2;

    void print() const override {
        std::cout << "Derived\n";
    }
};

int main() {
    Derived d;
    Base b = d;   // slicing
    b.print();    // Base
}
```

To avoid slicing, use a reference or pointer:

```cpp
Base& ref = d;
ref.print();     // Derived
```

---

## Functors

A functor is a class or struct that overloads `operator()`, letting an object behave like a function.

```cpp
class Multiplier {
private:
    int factor;

public:
    explicit Multiplier(int f) : factor(f) {}

    int operator()(int value) const {
        return value * factor;
    }
};

int main() {
    Multiplier triple(3);
    std::cout << triple(5) << "\n";
    std::cout << triple(10) << "\n";
}
```

Why they matter:

- can hold internal state
- work well with STL algorithms
- are the basis for many library abstractions

---

## Semaphores

A semaphore is a synchronization primitive that controls access to a shared resource using a counter. C++20 provides native support in `<semaphore>`.

Types:

- counting semaphore: allows a limited number of concurrent entrants
- binary semaphore: similar to a mutex-like gate with capacity `1`

```cpp
#include <chrono>
#include <iostream>
#include <semaphore>
#include <thread>

std::counting_semaphore<2> room_limit(2);

void worker(int id) {
    room_limit.acquire();
    std::cout << "Worker " << id << " entered\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "Worker " << id << " leaving\n";
    room_limit.release();
}

int main() {
    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    std::thread t3(worker, 3);

    t1.join();
    t2.join();
    t3.join();
}
```

---

## Callbacks

A callback is executable code passed into another function so it can be invoked later or at a specific event point.

Modern C++ usually models callbacks with `std::function`.

```cpp
#include <functional>
#include <iostream>
#include <vector>

void performAction(
    const std::vector<int>& target_list,
    const std::function<void(int)>& callback) {
    for (int num : target_list) {
        callback(num);
    }
}

int main() {
    std::vector<int> nums = {1, 2, 3};

    auto printSquare = [](int n) {
        std::cout << (n * n) << "\n";
    };

    performAction(nums, printSquare);
}
```

`std::function` can wrap:

- function pointers
- lambdas
- functors

---

## STL Overview

The Standard Template Library (STL) is a generic, reusable set of containers, iterators, algorithms, and function objects.

Its four major components are:

1. Containers
2. Iterators
3. Algorithms
4. Functors and adapters

### Core Idea

```text
Containers <-> Iterators <-> Algorithms
                     |
                 Functors
                 Adapters
```

---

## Containers

STL containers are commonly grouped into three categories.

### 1. Sequence Containers

- `std::vector`
- `std::deque`
- `std::list`

Key notes:

- `vector`: contiguous memory, fast random access, default choice in most cases
- `deque`: efficient insertion/removal at both ends
- `list`: linked structure, fast insertion once position is known, but no random access

### 2. Associative Containers

- `std::set`
- `std::map`
- `std::multiset`
- `std::multimap`

Typical implementation:

- balanced tree, often a red-black tree
- sorted order
- `O(log N)` lookup, insertion, and deletion

### 3. Unordered Associative Containers

- `std::unordered_set`
- `std::unordered_map`

Typical behavior:

- hash-table based
- average `O(1)` lookup
- worst-case `O(N)` if collisions are bad

---

## Iterators

Iterators provide a uniform way to traverse containers.

### Iterator Categories

1. Forward iterators: move only forward
2. Bidirectional iterators: move forward and backward
3. Random-access iterators: jump in constant time

Examples:

- `std::forward_list`: forward iterator
- `std::list`, `std::set`, `std::map`: bidirectional iterator
- `std::vector`, `std::deque`: random-access iterator

### Stream and Insert Iterators

```cpp
#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>

int main() {
    std::vector<int> source = {10, 20, 30, 40, 50};
    std::list<int> destination;

    std::copy(source.begin(), source.end(), std::back_inserter(destination));

    std::cout << "List contents: ";
    std::copy(
        destination.begin(),
        destination.end(),
        std::ostream_iterator<int>(std::cout, " -> "));
    std::cout << "NULL\n";
}
```

---

## Algorithms

STL algorithms are generic functions that operate on iterator ranges.

### Common Categories

- non-modifying: `std::count_if`, `std::all_of`, `std::search`
- modifying: `std::transform`, `std::remove_if`, `std::rotate`
- sorting/partitioning: `std::sort`, `std::stable_sort`, `std::partition`, `std::nth_element`

### Example: Replace and Sort with a Functor

```cpp
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

struct DescendingComparator {
    bool operator()(int a, int b) const {
        return a > b;
    }
};

int main() {
    std::vector<int> numbers = {15, 3, 42, 8, 21, 4, 8};

    std::replace_if(numbers.begin(), numbers.end(), [](int n) {
        return n % 2 == 0;
    }, 0);

    std::sort(numbers.begin(), numbers.end(), DescendingComparator());

    std::cout << "Processed Numbers: ";
    std::copy(numbers.begin(), numbers.end(),
              std::ostream_iterator<int>(std::cout, " "));
    std::cout << "\n";
}
```

### Erase-Remove Idiom

`std::remove_if` does not actually shrink the container. It moves unwanted elements to the end and returns a new logical end.

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto new_end = std::remove_if(vec.begin(), vec.end(), [](int n) {
        return n % 2 != 0;
    });

    vec.erase(new_end, vec.end());

    std::transform(vec.begin(), vec.end(), vec.begin(), [](int n) {
        return n * 10;
    });

    for (int x : vec) {
        std::cout << x << " ";
    }
    std::cout << "\n";
}
```

---

## Iterator Invalidation and Performance Notes

### Iterator Invalidation

Some containers invalidate iterators when they grow or restructure.

Important example:

- `std::vector::push_back()` may reallocate
- reallocation invalidates iterators, pointers, and references into the vector

Node-based containers like `std::list` usually keep iterators valid across insertion.

### Vector Growth

Vectors often over-allocate to keep append operations amortized `O(1)`.

Interview takeaway:

- use `reserve()` when you know the approximate final size
- this reduces reallocations and iterator invalidation

### Allocators

All STL containers use allocators under the hood. By default, they rely on `std::allocator`, but specialized systems may use custom allocators for performance or memory control.

---

## Stateful Functors and Adapters

### Stateful Functor

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

class RunningSum {
private:
    int sum = 0;

public:
    void operator()(int n) {
        sum += n;
        std::cout << "Current item: " << n
                  << " | Accumulation: " << sum << "\n";
    }
};

int main() {
    std::vector<int> data = {5, 10, 15};
    RunningSum tracker;
    std::for_each(data.begin(), data.end(), tracker);
}
```

### Container Adapters

Adapters wrap an existing container and restrict its interface:

- `std::stack`: LIFO
- `std::queue`: FIFO
- `std::priority_queue`: heap-based priority access

### Iterator Adapters

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};

    for (auto rit = v.rbegin(); rit != v.rend(); ++rit) {
        std::cout << *rit << " ";
    }
    std::cout << "\n";
}
```

---

## Interview-Friendly Summary

- Stack memory is fast and automatic; heap memory is flexible but manually managed.
- `static` affects lifetime, not just scope.
- Object slicing happens on by-value base assignment.
- Functors are objects that behave like functions and can keep state.
- Semaphores control concurrent access to limited resources.
- Callbacks are often modeled with `std::function`.
- STL is built around containers, iterators, algorithms, and functors/adapters.
- `std::vector` is usually the default container.
- `std::remove_if` needs `erase()` afterward for actual deletion.
- `std::vector` reallocation can invalidate iterators and pointers.

---

End of additional_details.md
