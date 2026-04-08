# modern_cpp.md

This document expands the topics listed in `summarize.md` into a more detailed Modern C++ reference. It keeps the same revision-friendly style, but adds examples, practical guidance, and common interview pitfalls.

---

## Smart Pointers and Memory Ownership

Modern C++ prefers ownership-aware resource management over raw `new` and `delete`. The core idea is RAII:

- Resource Acquisition Is Initialization
- acquire the resource in a constructor
- release it in a destructor
- let object lifetime control resource lifetime

This makes code safer in the presence of:

- exceptions
- early returns
- complex control flow

### `std::unique_ptr`

`std::unique_ptr` represents exclusive ownership. Only one `unique_ptr` owns the object at a time.

Use it when:

- exactly one object should own the resource
- you want automatic cleanup
- you want move-only semantics

```cpp
#include <iostream>
#include <memory>
using namespace std;

class File {
public:
    File() { cout << "File opened\n"; }
    ~File() { cout << "File closed\n"; }
    void write() const { cout << "Writing to file\n"; }
};

int main() {
    unique_ptr<File> f = make_unique<File>();
    f->write();

    unique_ptr<File> other = move(f);   // ownership transfer

    if (!f) {
        cout << "f is now null\n";
    }
}
```

Key points:

- cannot be copied
- can be moved
- should be your default smart pointer

### `release()` vs `reset()`

```cpp
unique_ptr<int> p = make_unique<int>(42);

int* raw = p.release();   // ownership leaves unique_ptr
delete raw;               // now caller must delete it

p.reset(new int(100));    // deletes old object if present and takes new ownership
```

Guideline:

- use `reset()` in normal code
- use `release()` only when an API requires a raw pointer transfer

### `std::shared_ptr`

`std::shared_ptr` represents shared ownership. The object is destroyed only when the last shared owner goes away.

```cpp
#include <iostream>
#include <memory>
using namespace std;

class Node {
public:
    Node() { cout << "Node created\n"; }
    ~Node() { cout << "Node destroyed\n"; }
};

int main() {
    shared_ptr<Node> p1 = make_shared<Node>();
    shared_ptr<Node> p2 = p1;

    cout << "Reference count: " << p1.use_count() << '\n';
}
```

Costs and caution:

- reference counting has runtime overhead
- shared ownership can make design unclear
- cycles can leak memory

### `std::weak_ptr`

Use `weak_ptr` to observe an object managed by `shared_ptr` without extending its lifetime.

```cpp
#include <iostream>
#include <memory>
using namespace std;

class Child;

class Parent {
public:
    shared_ptr<Child> child;
};

class Child {
public:
    weak_ptr<Parent> parent;   // prevents cycle
};
```

Why it matters:

- `shared_ptr` + `shared_ptr` in both directions can cause memory leaks
- `weak_ptr` breaks the cycle

---

## Move Semantics and Value Categories

Modern C++ became much more efficient with move semantics.

### Lvalue and Rvalue

- lvalue: has a persistent identity, like a named variable
- rvalue: temporary value, usually safe to move from

### Move Constructor

Instead of copying data, a move constructor transfers ownership.

```cpp
#include <iostream>
#include <utility>
using namespace std;

class Buffer {
    int* data;

public:
    Buffer(int value) : data(new int(value)) {}

    Buffer(const Buffer& other) : data(new int(*other.data)) {
        cout << "Copy constructor\n";
    }

    Buffer(Buffer&& other) noexcept : data(other.data) {
        other.data = nullptr;
        cout << "Move constructor\n";
    }

    ~Buffer() {
        delete data;
    }
};
```

Why this matters:

- moving avoids unnecessary heap allocations
- standard containers use moves heavily for performance

### Rule of Five and Rule of Zero

If your class manually manages a resource, you often need:

- destructor
- copy constructor
- copy assignment
- move constructor
- move assignment

But in modern C++, prefer the Rule of Zero:

- let `std::string`, `std::vector`, and smart pointers manage resources
- then you usually do not need special member functions

---

## Multithreading Essentials

C++11 introduced standard threading support through:

- `std::thread`
- `std::mutex`
- `std::lock_guard`
- `std::atomic`
- condition variables

### Basic Thread Creation

```cpp
#include <iostream>
#include <thread>
using namespace std;

void worker() {
    cout << "Worker thread running\n";
}

int main() {
    thread t(worker);
    t.join();
}
```

### `join()` vs `detach()`

- `join()` waits for the thread to finish
- `detach()` lets it run independently

Use `detach()` carefully because:

- the detached thread may outlive referenced objects
- debugging becomes harder
- ownership and shutdown become unclear

### Race Condition

A race condition happens when:

- multiple threads access shared data
- at least one thread writes
- there is no synchronization

```cpp
#include <iostream>
#include <thread>
using namespace std;

int counter = 0;

void increment() {
    for (int i = 0; i < 100000; ++i) {
        ++counter;   // not thread-safe
    }
}
```

The result is undefined behavior.

### Mutex and `lock_guard`

```cpp
#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

int counter = 0;
mutex m;

void safeIncrement() {
    for (int i = 0; i < 100000; ++i) {
        lock_guard<mutex> lock(m);
        ++counter;
    }
}
```

Why `lock_guard` is good:

- lock is acquired in constructor
- lock is released automatically in destructor
- safer than manual `lock()` and `unlock()`

### `std::atomic`

For simple counters or flags, `std::atomic` is often lighter than a mutex.

```cpp
#include <atomic>
#include <iostream>
#include <thread>
using namespace std;

atomic<int> counter{0};

void incrementAtomic() {
    for (int i = 0; i < 100000; ++i) {
        ++counter;
    }
}
```

Use `atomic` for:

- counters
- flags
- simple state changes

Use mutexes for:

- multiple related variables
- complex invariants
- critical sections larger than a single atomic operation

### Deadlock

Deadlock occurs when threads wait forever for each other.

Typical cause:

- thread A locks `m1` then `m2`
- thread B locks `m2` then `m1`

Avoid it by:

- taking locks in a consistent order
- using `std::lock`
- minimizing lock scope

---

## STL Containers and Iterators

The Standard Template Library gives you efficient, reusable containers and algorithms.

### `std::vector`

`vector` is usually the default container choice.

Why:

- contiguous memory
- cache-friendly
- fast iteration
- fast push-back amortized

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> nums;
    nums.reserve(5);

    nums.push_back(10);
    nums.push_back(20);
    nums.push_back(30);

    for (int x : nums) {
        cout << x << ' ';
    }
}
```

Use `reserve()` when you roughly know the final size. It reduces reallocations and copies or moves.

### `std::map` vs `std::unordered_map`

#### `std::map`

- ordered
- usually tree-based
- `O(log n)` lookup, insert, erase

```cpp
map<string, int> marks;
marks["Alice"] = 95;
marks["Bob"] = 88;
```

#### `std::unordered_map`

- hash-based
- average `O(1)` lookup
- no sorted order

```cpp
unordered_map<string, int> freq;
freq["apple"]++;
freq["banana"]++;
```

Choosing between them:

- use `map` when order matters
- use `unordered_map` when fast average lookup matters and ordering is irrelevant

### Important `map` Trap

```cpp
map<string, int> m;

if (m["missing"] == 0) {
    cout << "Inserted accidentally\n";
}
```

Why this is a trap:

- `m["missing"]` inserts a default value if key is absent

Safer alternatives:

```cpp
if (m.find("missing") != m.end()) {
    cout << "Found\n";
}

if (m.contains("missing")) {
    cout << "Found\n";
}
```

`contains()` requires C++20.

### Iterators

Iterators generalize pointer-like traversal of containers.

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v{1, 2, 3, 4};

    for (auto it = v.begin(); it != v.end(); ++it) {
        cout << *it << ' ';
    }
}
```

Why iterators matter:

- algorithms use them
- they decouple generic code from specific containers

Example with algorithm:

```cpp
#include <algorithm>
#include <vector>
using namespace std;

vector<int> v{5, 1, 9, 3};
sort(v.begin(), v.end());
```

---

## Design Patterns in Practice

Patterns are reusable design ideas, not strict rules. In C++, they should be applied carefully and only when they simplify the design.

### Singleton

Ensures only one instance exists.

```cpp
class Logger {
public:
    static Logger& instance() {
        static Logger obj;   // thread-safe since C++11
        return obj;
    }

    void log(const string& msg) {
        cout << msg << '\n';
    }

private:
    Logger() = default;
};
```

Good:

- config manager
- global logging access in small systems

Caution:

- hidden global state can hurt testability

### Factory

Factory separates object creation from object use.

```cpp
#include <iostream>
#include <memory>
using namespace std;

class Shape {
public:
    virtual void draw() const = 0;
    virtual ~Shape() = default;
};

class Circle : public Shape {
public:
    void draw() const override { cout << "Circle\n"; }
};

class Square : public Shape {
public:
    void draw() const override { cout << "Square\n"; }
};

unique_ptr<Shape> createShape(const string& type) {
    if (type == "circle") return make_unique<Circle>();
    if (type == "square") return make_unique<Square>();
    return nullptr;
}
```

### Observer

Observer allows one object to notify many dependents about changes.

Typical examples:

- event systems
- GUI updates
- state monitoring

Design caution:

- raw pointers can dangle
- `weak_ptr` is often safer in modern C++

### Strategy

Strategy encapsulates interchangeable behavior.

```cpp
#include <functional>
#include <iostream>
using namespace std;

class PaymentProcessor {
    function<void(double)> strategy;

public:
    explicit PaymentProcessor(function<void(double)> s) : strategy(move(s)) {}

    void pay(double amount) {
        strategy(amount);
    }
};
```

This is often cleaner in modern C++ because lambdas reduce boilerplate.

### Decorator

Decorator adds behavior without modifying the original class.

Common use cases:

- logging
- validation
- compression
- encryption

---

## Object Semantics, Copying, and Memory Safety

This is one of the most common interview areas because it mixes:

- object lifetime
- ownership
- correctness
- performance

### Shallow Copy vs Deep Copy

If a class owns a raw pointer, default copying can be dangerous.

```cpp
#include <cstring>
using namespace std;

class BadString {
    char* data;

public:
    BadString(const char* s) {
        data = new char[strlen(s) + 1];
        strcpy(data, s);
    }

    ~BadString() {
        delete[] data;
    }
};
```

Problem:

- compiler-generated copy constructor copies only the pointer value
- both objects point to the same memory
- both destructors attempt to delete the same memory

### Proper Deep Copy

```cpp
#include <cstring>
using namespace std;

class SafeString {
    char* data;

public:
    SafeString(const char* s) {
        data = new char[strlen(s) + 1];
        strcpy(data, s);
    }

    SafeString(const SafeString& other) {
        data = new char[strlen(other.data) + 1];
        strcpy(data, other.data);
    }

    SafeString& operator=(const SafeString& other) {
        if (this != &other) {
            char* temp = new char[strlen(other.data) + 1];
            strcpy(temp, other.data);
            delete[] data;
            data = temp;
        }
        return *this;
    }

    ~SafeString() {
        delete[] data;
    }
};
```

### Copy Constructor vs Copy Assignment

Copy constructor:

- creates a new object from an existing one

Copy assignment:

- assigns one existing object to another existing object

```cpp
SafeString a("hello");
SafeString b = a;   // copy constructor

SafeString c("world");
c = a;              // copy assignment
```

Important interview point:

- self-assignment check is needed in copy assignment
- not needed in copy constructor

### Copy-and-Swap Idiom

This is a strong pattern for exception-safe assignment.

```cpp
class Buffer2 {
    int* data;

public:
    Buffer2(int value = 0) : data(new int(value)) {}

    Buffer2(const Buffer2& other) : data(new int(*other.data)) {}

    Buffer2& operator=(Buffer2 other) {
        swap(data, other.data);
        return *this;
    }

    ~Buffer2() { delete data; }
};
```

Why it is good:

- handles self-assignment naturally
- gives strong exception safety

### Memory Alignment and Padding

A struct may occupy more memory than the sum of its members.

```cpp
struct Example {
    int x;    // 4 bytes
    char c;   // 1 byte
};
```

This may still take 8 bytes because padding is inserted for alignment.

Why it matters:

- affects memory usage
- affects binary protocols and hardware interfaces
- matters in embedded systems

---

## Dynamic Memory Basics

Raw dynamic memory still appears in interviews and legacy code even if smart pointers are preferred in production.

### Single Object vs Array Allocation

```cpp
int* p1 = new int(5);      // one int
int* p2 = new int[5];      // array of 5 ints

delete p1;
delete[] p2;
```

Rule:

- `new` must pair with `delete`
- `new[]` must pair with `delete[]`

Mixing them causes undefined behavior.

Modern advice:

- prefer `std::vector<int>` for dynamic arrays
- prefer smart pointers for ownership

---

## Inline Functions, Lambdas, and Fundamentals

### Inline Functions

An inline function suggests that the compiler may expand the function body at the call site.

```cpp
inline int square(int x) {
    return x * x;
}
```

Important note:

- `inline` is also important for multiple definitions across translation units
- the compiler is free to inline or not inline regardless of the keyword

### Lambdas

Lambdas are anonymous function objects.

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v{1, 2, 3, 4, 5};

    int countEven = count_if(v.begin(), v.end(), [](int x) {
        return x % 2 == 0;
    });

    cout << countEven << '\n';
}
```

Why lambdas are important in modern C++:

- compact custom logic
- common with STL algorithms
- often used for callbacks and async work

### Command-Line Arguments

```cpp
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    cout << "Argument count: " << argc << '\n';
    for (int i = 0; i < argc; ++i) {
        cout << argv[i] << '\n';
    }
}
```

Example:

- running `app.exe hello world`
- `argc` becomes 3
- `argv[0]` is program name
- `argv[1]` is `hello`
- `argv[2]` is `world`

---

## Const and Static

These are small keywords with big interview value.

### `const`

Use `const` to express immutability.

```cpp
class Student {
    int age;

public:
    Student(int a) : age(a) {}

    int getAge() const {
        return age;
    }
};
```

Benefits:

- improves correctness
- improves readability
- allows use with const objects

### `static`

Static data belongs to the class, not each object.

```cpp
#include <iostream>
using namespace std;

class User {
    static int count;

public:
    User() { ++count; }
    static int getCount() { return count; }
};

int User::count = 0;
```

Use cases:

- shared counters
- global-like class-level data
- helper functions related to class state

---

## Modern C++ Standards

This is a common interview question: what changed across language standards?

### C++11

Major additions:

- move semantics
- smart pointers
- threads
- lambdas
- `auto`
- `nullptr`
- `override`
- `constexpr`

Why it matters:

- this is the foundation of modern C++

### C++14

Improvements:

- generic lambdas
- `make_unique`
- relaxed `constexpr`

### C++17

Very widely used in industry.

Key features:

- structured bindings
- `std::optional`
- `std::variant`
- `std::string_view`
- `if constexpr`
- filesystem library

Example:

```cpp
#include <iostream>
#include <map>
using namespace std;

int main() {
    map<string, int> scores{{"Alice", 90}, {"Bob", 85}};

    for (const auto& [name, score] : scores) {
        cout << name << " -> " << score << '\n';
    }
}
```

### C++20

Key features:

- concepts
- ranges
- coroutines
- `std::span`
- calendar and formatting improvements

Concept example:

```cpp
template <typename T>
concept Addable = requires(T a, T b) {
    a + b;
};

template <Addable T>
T add(T a, T b) {
    return a + b;
}
```

### C++23

Refinements continue, including:

- `std::expected`
- more ranges support
- library improvements

Interview answer:

- C++17 is often the safest practical answer for production use
- it gives modern features with strong compiler support

---

## Design in Embedded and Automotive Contexts

Some of the topics in your summary are not pure language features, but they are highly relevant in real C++ system work.

### CAN Messages and Signals

CAN frame:

- the actual packet sent on the CAN bus

CAN signal:

- a field inside that packet representing one logical value

Example:

- message ID `0x100`
- signal `VehicleSpeed`
- raw value `120`
- factor `0.1`
- offset `0`

Physical value:

```text
physical = raw * factor + offset
physical = 120 * 0.1 + 0
physical = 12.0 km/h
```

Why this matters in C++ work:

- embedded software often extracts signals from binary frames
- correctness depends on bit positions, endianness, scaling, and signedness

### Watchdog and Logging

Watchdog:

- monitors whether software is still alive
- resets or recovers the system if it hangs

Logging:

- records what happened before failure
- helps root-cause analysis

In automotive or embedded systems:

- watchdog improves availability and safety
- persistent logging improves diagnosability

The pair works well together:

- watchdog recovers the system
- logs explain why recovery was needed

---

## Testing Frameworks

Modern C++ development is incomplete without testing.

### Google Test

Google Test is commonly used for C++ unit testing.

```cpp
#include <gtest/gtest.h>

int add(int a, int b) {
    return a + b;
}

TEST(AddTest, PositiveNumbers) {
    EXPECT_EQ(add(2, 3), 5);
}
```

Why teams use it:

- expressive assertions
- test fixtures
- easy integration with CI

### PyTest

PyTest is for Python, but it often complements C++ projects.

Typical use:

- automation around a C++ binary
- integration tests
- system-level validation

Interview comparison:

- GTest is strong for low-level C++ logic
- PyTest is excellent for scripting, integration, and rapid automation

---

## High-Value Interview Fundamentals

The most common misses are not always advanced topics. They are often fundamentals that need crisp, confident explanations.

### Must Be Comfortable With

- `argc` and `argv`
- `const` and `static`
- iterators
- `new` vs `new[]`
- `delete` vs `delete[]`
- shallow copy vs deep copy
- copy constructor vs copy assignment
- move semantics
- smart pointer ownership
- `join()` vs `detach()`

### Good One-Liners

- `unique_ptr` is exclusive ownership, `shared_ptr` is shared ownership, and `weak_ptr` is non-owning observation.
- `vector` is usually the default container because it offers contiguous storage and strong performance.
- Self-assignment checks belong in copy assignment operators, not copy constructors.
- C++17 is often the practical sweet spot between modern language power and toolchain stability.
- A watchdog recovers the system, while logging explains the failure afterward.

---

## Practical Guidance

When writing modern C++ in interviews or production:

- prefer `std::vector` over raw dynamic arrays
- prefer `std::string` over raw C strings
- prefer `std::unique_ptr` over raw owning pointers
- use `const` aggressively where mutation is not needed
- use RAII for locks, files, and memory
- avoid `detach()` unless lifetime is very clear
- use `override` for every overridden virtual function
- use standard library facilities before writing manual memory code

---

## Mini Revision Checklist

Before an interview, make sure you can explain and write code for:

- smart pointers and ownership transfer
- race conditions, mutexes, and atomics
- `vector`, `map`, and `unordered_map`
- shallow copy, deep copy, and Rule of Five
- command-line arguments
- lambda expressions
- C++11 to C++23 feature progression
- testing basics with GTest
- CAN signal decoding and watchdog purpose in embedded systems

---

End of modern_cpp.md
