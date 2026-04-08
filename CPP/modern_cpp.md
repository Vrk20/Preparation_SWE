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

One very important memory concept is:

- the container object itself can live on the stack or on the heap depending on how you create it
- most dynamic STL containers allocate their elements internally on the heap

Example:

```cpp
vector<int> v1;                    // container object on stack, elements on heap
auto v2 = make_unique<vector<int>>(); // container object on heap, elements also on heap
```

So when someone asks "Is `vector` stored on stack or heap?", the accurate answer is:

- the `vector` object itself follows normal object lifetime rules
- its internal dynamic buffer is typically allocated on the heap

The same idea applies to many STL containers.

### Container Categories

STL containers are usually grouped into:

- sequence containers
- associative containers
- unordered associative containers
- container adapters

Common examples:

- sequence: `vector`, `deque`, `list`, `array`
- associative: `map`, `set`, `multimap`, `multiset`
- unordered associative: `unordered_map`, `unordered_set`
- adapters: `stack`, `queue`, `priority_queue`

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

Memory model of `vector`:

- the `vector` object itself stores small metadata such as pointer, size, and capacity
- that small metadata is wherever the object is created, usually stack for local variables
- the actual elements are stored in a contiguous dynamic buffer on the heap

Important behavior:

- when capacity is exceeded, `vector` allocates a larger heap block
- existing elements are copied or moved to the new block
- old iterators, pointers, and references may become invalid after reallocation

Interview one-liner:

- `vector` gives dynamic size with array-like contiguous storage

### `std::array`

`std::array` is a fixed-size container wrapper around a built-in array.

```cpp
#include <array>
#include <iostream>
using namespace std;

int main() {
    array<int, 4> a{1, 2, 3, 4};
    cout << a[0] << '\n';
}
```

Memory model of `array`:

- elements are stored directly inside the `array` object itself
- if the `array` object is local, the elements are typically on the stack
- if the `array` object is a member of a heap object, those elements live inside that heap object

So unlike `vector`, `array` does not perform a separate heap allocation for its elements.

Use `array` when:

- size is known at compile time
- you want stack-friendly fixed-size storage
- you want STL compatibility over raw arrays

### `std::deque`

`deque` stands for double-ended queue.

```cpp
#include <deque>
using namespace std;

deque<int> dq;
dq.push_back(10);
dq.push_front(5);
```

Properties:

- fast insertion and deletion at both front and back
- random access is supported
- elements are not guaranteed to be stored in one contiguous block like `vector`

Memory model of `deque`:

- container object itself follows normal object placement rules
- internal storage is dynamic and typically spread across multiple heap-allocated blocks

Use `deque` when:

- you need frequent push/pop at both ends
- contiguous storage is not required

### `std::list`

`list` is a doubly linked list.

```cpp
#include <list>
using namespace std;

list<int> lst{10, 20, 30};
lst.push_front(5);
lst.push_back(40);
```

Properties:

- very fast insertion and deletion when you already have the iterator
- no random access like `v[2]`
- much worse cache locality than `vector`

Memory model of `list`:

- each node is separately allocated on the heap
- each node stores the value plus links to previous and next nodes
- extra pointer overhead makes it memory-heavier than `vector`

Interview point:

- `list` is often less efficient than people expect because pointer chasing hurts cache performance

### `std::string`

Although not always introduced as a "container" first, `std::string` behaves like a specialized dynamic character container.

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string name = "Modern C++";
    cout << name << '\n';
}
```

Memory model of `string`:

- the string object itself follows normal object placement rules
- many implementations store short strings directly inside the object using Small String Optimization
- larger strings usually allocate character storage on the heap

So `string` may avoid heap allocation for short values, depending on implementation.

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

Memory model of `map`:

- the `map` object itself is a small control object
- each inserted element is typically stored in a separate tree node on the heap
- nodes also store bookkeeping such as parent/child links and balancing information

Implications:

- not contiguous in memory
- more memory overhead than `vector`
- slower iteration than contiguous containers due to poor cache locality

#### `std::unordered_map`

- hash-based
- average `O(1)` lookup
- no sorted order

```cpp
unordered_map<string, int> freq;
freq["apple"]++;
freq["banana"]++;
```

Memory model of `unordered_map`:

- it usually has a bucket array on the heap
- inserted elements are stored in heap-allocated nodes associated with buckets
- rehashing may allocate a new bucket array and redistribute elements

Implications:

- good average lookup performance
- extra memory overhead from buckets and nodes
- references to elements usually remain valid across rehash in many implementations, but iterators can be invalidated, so always check the standard guarantees for the operation you use

Choosing between them:

- use `map` when order matters
- use `unordered_map` when fast average lookup matters and ordering is irrelevant

### `std::set` and `std::unordered_set`

These are like `map` and `unordered_map`, but they store only keys, not key-value pairs.

```cpp
set<int> ordered{3, 1, 2};
unordered_set<int> fast{3, 1, 2};
```

Memory model:

- `set` is typically tree-node based, so elements live in heap-allocated nodes
- `unordered_set` typically uses a heap-allocated bucket structure plus heap-allocated nodes

Use them when:

- uniqueness matters
- you do not need duplicate elements

### Container Adapters

Adapters provide a restricted interface on top of another container.

#### `std::stack`

```cpp
#include <stack>
using namespace std;

stack<int> st;
st.push(10);
st.push(20);
st.pop();
```

Important idea:

- `stack` is usually built on top of another container such as `deque`
- by default, `stack` uses `deque`

Memory model:

- the `stack` adapter object contains the underlying container object
- actual element storage depends on that underlying container
- with default `deque`, storage is typically dynamic and heap-backed

#### `std::queue`

`queue` is also an adapter, usually backed by `deque`.

#### `std::priority_queue`

`priority_queue` is commonly backed by `vector`.

That means:

- adapter object follows normal object placement rules
- underlying `vector` usually allocates its element buffer on the heap

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

### Stack vs Heap Summary for Common Containers

This is the cleanest interview summary:

- `std::array`: elements live inside the object itself, so no separate heap allocation
- `std::vector`: object is local or heap depending on creation, elements usually live in one contiguous heap block
- `std::deque`: object is local or heap depending on creation, elements typically live in multiple heap blocks
- `std::list`: object is local or heap depending on creation, each node is separately heap-allocated
- `std::map` and `std::set`: object is local or heap depending on creation, elements live in heap-allocated tree nodes
- `std::unordered_map` and `std::unordered_set`: object is local or heap depending on creation, buckets and nodes usually live on the heap
- `std::stack`, `std::queue`, `std::priority_queue`: storage depends on the underlying container

The short conceptual rule is:

- local object declaration decides where the container object lives
- container design decides where its internal data lives

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

Iterator category intuition:

- `vector` and `array` iterators behave much like pointers because storage is contiguous
- `list` iterators move node to node, so random jumping is not available
- associative container iterators traverse tree or hash structures rather than contiguous memory

Common invalidation idea:

- `vector` iterators can be invalidated by reallocation
- `deque` iterator invalidation rules are more subtle
- `list` iterators are generally stable except for erased elements
- `map` and `unordered_map` iterators can be invalidated by certain operations like erase or rehash

Always know invalidation behavior for the specific container before storing iterators long term.

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

## Important Keywords, Variations, and Examples

This section is meant to be a quick-reference sheet for common C++ keywords. In interviews, it is often not enough to know the definition. You should also know:

- where the keyword is used
- how it changes behavior
- what common variations look like

### `const`

`const` means the value should not be modified through that name or reference.

Basic example:

```cpp
const int x = 10;
// x = 20;   // error
```

Important variations:

```cpp
const int* p1 = &x;   // pointer to const int, value cannot be changed through p1
int* const p2 = &y;   // const pointer, pointer cannot point elsewhere
const int* const p3 = &x; // const pointer to const int
```

Member function variation:

```cpp
class A {
public:
    int get() const { return 42; }   // promises not to modify object state
};
```

Interview point:

- read pointer declarations from right to left

### `constexpr`

`constexpr` means an expression or function can be evaluated at compile time if given compile-time inputs.

```cpp
constexpr int square(int x) {
    return x * x;
}

constexpr int value = square(5);
```

Difference from `const`:

- `const` means read-only
- `constexpr` means compile-time evaluable when possible

Useful variations:

```cpp
constexpr int n = 10;
constexpr double pi = 3.14159;
```

### `consteval`

`consteval` forces compile-time evaluation.

```cpp
consteval int cube(int x) {
    return x * x * x;
}

constexpr int v = cube(3);
```

Use it when:

- a value must be computed at compile time

### `static`

`static` changes storage duration or class-level ownership depending on context.

#### Static local variable

```cpp
void counter() {
    static int count = 0;
    ++count;
}
```

Meaning:

- created once
- retains value between function calls

#### Static data member

```cpp
class User {
public:
    static int count;
};

int User::count = 0;
```

Meaning:

- shared by all objects of the class

#### Static member function

```cpp
class Math {
public:
    static int add(int a, int b) { return a + b; }
};
```

Meaning:

- belongs to the class, not an object
- cannot access non-static members directly

### `inline`

`inline` suggests inlining and also allows the same function definition to appear in multiple translation units safely.

```cpp
inline int add(int a, int b) {
    return a + b;
}
```

Important note:

- modern compilers decide actual inlining
- the keyword is also important for header-defined functions

### `virtual`

`virtual` enables runtime polymorphism.

```cpp
class Base {
public:
    virtual void show() const {
        cout << "Base\n";
    }
};
```

Meaning:

- call resolution happens using actual object type when accessed through base pointer or reference

### `override`

`override` tells the compiler that a function is intended to override a base-class virtual function.

```cpp
class Derived : public Base {
public:
    void show() const override {
        cout << "Derived\n";
    }
};
```

Why it is useful:

- catches signature mismatches
- improves readability

### `final`

`final` prevents further overriding or inheritance.

Function variation:

```cpp
class B {
public:
    virtual void f();
};

class D : public B {
public:
    void f() final;
};
```

Class variation:

```cpp
class Utility final {
};
```

Meaning:

- a `final` class cannot be inherited
- a `final` function cannot be overridden further

### `explicit`

`explicit` prevents unwanted implicit conversions through single-argument constructors or conversion operators.

Without `explicit`:

```cpp
class Number {
public:
    Number(int x) {}
};

void print(Number n) {}
// print(5);   // allowed if constructor is not explicit
```

With `explicit`:

```cpp
class Number {
public:
    explicit Number(int x) {}
};

// print(5);   // error
print(Number(5));
```

Interview point:

- use `explicit` by default for single-argument constructors unless implicit conversion is intentionally desired

### `mutable`

`mutable` allows a member to be modified even inside a `const` member function.

```cpp
class Cache {
    mutable int hits = 0;

public:
    int get() const {
        ++hits;
        return 42;
    }
};
```

Use it carefully for:

- caching
- statistics
- lazy initialization

### `volatile`

`volatile` tells the compiler that a value can change unexpectedly outside normal program flow.

```cpp
volatile int* statusRegister = reinterpret_cast<volatile int*>(0x1234);
```

Typical use:

- memory-mapped hardware registers
- low-level embedded programming

Important note:

- `volatile` does not make code thread-safe
- it is not a replacement for `atomic`

### `noexcept`

`noexcept` declares that a function does not throw exceptions.

```cpp
void cleanup() noexcept {
}
```

Move operations often use it:

```cpp
class Buffer3 {
public:
    Buffer3(Buffer3&& other) noexcept = default;
};
```

Why it matters:

- standard containers prefer `noexcept` moves during reallocation

### `auto`

`auto` asks the compiler to deduce the type from the initializer.

```cpp
auto x = 10;        // int
auto y = 3.14;      // double
auto p = make_unique<int>(5);
```

Use it when:

- the type is obvious from the right-hand side
- the type is long or template-heavy

Be careful:

```cpp
const int a = 10;
auto b = a;         // b is int, top-level const is dropped
const auto c = a;   // c is const int
```

### `decltype`

`decltype` extracts the type of an expression.

```cpp
int x = 5;
decltype(x) y = 10;   // y is int
```

Common use:

- template metaprogramming
- preserving exact types
- trailing return types

### `typename`

`typename` is used in templates to tell the compiler that a dependent name is a type.

```cpp
template <typename T>
typename T::value_type getFirst(const T& container) {
    return *container.begin();
}
```

Without `typename`, the compiler may not know whether `T::value_type` is a type or something else.

### `using`

`using` creates aliases and helps with cleaner code.

Type alias example:

```cpp
using ll = long long;
using IntVector = vector<int>;
```

Namespace usage example:

```cpp
using std::cout;
using std::string;
```

Modern preference:

- prefer `using` over `typedef` for readability

### `nullptr`

`nullptr` is the modern null pointer literal.

```cpp
int* ptr = nullptr;
```

Why use it instead of `NULL`:

- it has a real pointer type
- avoids overload ambiguity

### `friend`

`friend` allows another function or class to access private members.

```cpp
class Box {
private:
    int value = 10;

    friend void show(const Box& b);
};

void show(const Box& b) {
    cout << b.value << '\n';
}
```

Use carefully:

- it increases coupling
- but can be useful for operators or tightly related helper code

### `new` and `delete`

These allocate and free dynamic memory manually.

```cpp
int* p = new int(5);
delete p;
```

Array variation:

```cpp
int* arr = new int[10];
delete[] arr;
```

Interview point:

- in modern C++, prefer standard containers and smart pointers over manual `new` and `delete`

### `thread_local`

`thread_local` creates one separate instance per thread.

```cpp
thread_local int localCount = 0;
```

Use it when:

- each thread needs its own state
- data must not be shared across threads

### Common Keyword Combinations

Some combinations are especially common in modern C++:

#### `const auto`

```cpp
const auto value = 42;
```

Use when:

- type should be deduced
- variable should not be modified

#### `static constexpr`

```cpp
class Config {
public:
    static constexpr int maxSize = 100;
};
```

Use when:

- class-level constant is known at compile time

#### `virtual ... override`

```cpp
class Animal {
public:
    virtual void speak() const = 0;
};

class Dog : public Animal {
public:
    void speak() const override {
        cout << "Bark\n";
    }
};
```

Use when:

- base class defines polymorphic interface
- derived class provides implementation

#### `const std::unique_ptr<T>&`

```cpp
void printValue(const unique_ptr<int>& p) {
    if (p) cout << *p << '\n';
}
```

Meaning:

- you are not transferring ownership
- you are only observing the pointed object through the smart pointer

#### `T&&`

```cpp
void setName(string&& s) {
    name = move(s);
}
```

Meaning:

- accepts an rvalue reference
- enables moving instead of copying

### Quick Keyword Revision One-Liners

- `const` means read-only through that name.
- `constexpr` means compile-time evaluable when possible.
- `static` means persistent or class-level depending on context.
- `virtual` enables runtime polymorphism.
- `override` verifies that overriding is correct.
- `final` stops further inheritance or overriding.
- `explicit` prevents unwanted implicit conversions.
- `mutable` allows modification inside `const` member functions.
- `volatile` is for special low-level memory behavior, not for thread safety.
- `noexcept` tells the compiler and users that a function should not throw.
- `auto` asks the compiler to deduce the type.
- `decltype` extracts a type from an expression.
- `nullptr` is the type-safe null pointer literal.

---

End of modern_cpp.md
