# twisted_notes.md

This document is a cleaned Markdown version of `twisted.txt`, with corrections for accuracy and a few extra examples relevant to the existing files in this folder.

Some of these behaviors are legal C++. Some are only "possible" in the sense that code may appear to run, but they still count as undefined behavior. That distinction matters a lot in interviews.

---

## Twisted C++ OOP Behaviors

These are the kinds of things that feel wrong at first glance but are useful for understanding how C++ actually works under the hood.

---

## 1. Calling a Member Function Through a Null Pointer

This is one of the most commonly repeated C++ myths.

### Important correction

Calling a non-static member function through a null pointer is **undefined behavior**, even if the function does not use member data and even if it appears to work on your compiler.

People often say this "works because the function does not touch `this`." In practice, some compilers may generate code that appears to run, but the C++ language does not guarantee it.

### Example that may appear to work

```cpp
#include <iostream>

class Ghost {
public:
    void sayHello() {
        std::cout << "Boo\n";
    }
};

int main() {
    Ghost* ptr = nullptr;
    ptr->sayHello();   // undefined behavior
}
```

### Safe alternative

If behavior does not depend on object state, make it `static` or use a free function.

```cpp
#include <iostream>

class Ghost {
public:
    static void sayHello() {
        std::cout << "Boo\n";
    }
};

int main() {
    Ghost::sayHello();
}
```

### Interview takeaway

- "It may appear to work" is not the same as "it is legal C++".
- A good answer is: non-static member calls through `nullptr` are undefined behavior.

---

## 2. The Dominance Rule in Virtual Inheritance

In diamond-shaped inheritance with virtual bases, name lookup can resolve to the most-derived override without ambiguity.

```cpp
#include <iostream>

class A {
public:
    virtual void foo() { std::cout << "A::foo\n"; }
};

class B : virtual public A {
public:
    void foo() override { std::cout << "B::foo\n"; }
};

class C : virtual public A {
};

class D : public B, public C {
};

int main() {
    D object;
    object.foo();   // resolves to B::foo
}
```

### Why it matters

Because `A` is a virtual base, there is only one shared `A` subobject. Since `B` overrides `foo()` and `C` does not, lookup resolves to `B::foo()`.

### Interview takeaway

- virtual inheritance can remove duplicate base subobjects
- override resolution in these hierarchies can behave differently from simple multiple inheritance

---

## 3. Overriding a Private Virtual Function

This is valid and surprises many people.

```cpp
#include <iostream>

class Base {
public:
    void runProcess() {
        setup();
    }

private:
    virtual void setup() {
        std::cout << "Base setup\n";
    }
};

class Derived : public Base {
private:
    void setup() override {
        std::cout << "Derived setup\n";
    }
};

int main() {
    Base* ptr = new Derived();
    ptr->runProcess();
    delete ptr;
}
```

### Why this works

Access control and dynamic dispatch are separate ideas:

- access control is checked at compile time
- virtual dispatch happens at runtime

`runProcess()` is publicly callable, and once it invokes `setup()`, runtime dispatch can legally land in `Derived::setup()`.

### Interview takeaway

- `private` does not stop overriding
- it only affects who may name or call the function directly

---

## 4. `delete this`

An object can call `delete this`, but only under strict conditions.

```cpp
#include <iostream>

class SelfDestructor {
public:
    void suicide() {
        std::cout << "Self-destructing...\n";
        delete this;
    }
};

int main() {
    SelfDestructor* obj = new SelfDestructor();
    obj->suicide();
    obj = nullptr;
}
```

### Important correction

This is not "generally fine." It is only safe when all of the following are true:

- the object was allocated with `new`
- no one uses the object after deletion
- the member function does not access members after `delete this`
- object lifetime/ownership rules are very carefully controlled

If the object lives on the stack, `delete this` is invalid and dangerous.

### Interview takeaway

- legal in narrow cases
- usually a design smell
- smart pointers and clearer ownership are preferred in modern C++

---

## 5. Accessing Private Members of Another Object of the Same Class

In C++, privacy is enforced at the class level, not the object-instance level.

```cpp
#include <iostream>

class SecretAgent {
private:
    int classified_code;

public:
    explicit SecretAgent(int code) : classified_code(code) {}

    void spyOn(const SecretAgent& other) const {
        std::cout << "Stole code: " << other.classified_code << "\n";
    }
};

int main() {
    SecretAgent james(7);
    SecretAgent alec(6);
    james.spyOn(alec);
}
```

### Interview takeaway

- one object of a class can access private members of another object of the same class
- privacy is about the class interface, not about each instance isolating itself from peers

---

## 6. Pure Virtual Functions Can Still Have Definitions

A pure virtual function can still have a function body.

```cpp
#include <iostream>

class AbstractBase {
public:
    virtual void logError() = 0;
};

void AbstractBase::logError() {
    std::cout << "Default fallback logger\n";
}

class Derived : public AbstractBase {
public:
    void logError() override {
        AbstractBase::logError();
    }
};

int main() {
    Derived d;
    d.logError();
}
```

### Why this is useful

This is especially important for pure virtual destructors:

```cpp
class Interface {
public:
    virtual ~Interface() = 0;
};

Interface::~Interface() = default;
```

The class remains abstract, but the destructor still needs a definition because derived destructors must call it.

### Interview takeaway

- `= 0` means the class is abstract
- it does not mean the function body is forbidden

---

## Extra Example: Breaking `shared_ptr` Cycles with `weak_ptr`

This matches the theme in [leak_smart.cpp](E:/interview_prep/Preparation_SWE/interview_pr/twisted/leak_smart.cpp).

If two objects own each other with `shared_ptr`, their reference counts never reach zero.

### Problematic design

```cpp
class Person {
public:
    std::shared_ptr<Person> friendPtr;
};
```

If `alice->friendPtr = bob` and `bob->friendPtr = alice`, both objects stay alive forever.

### Correct pattern

```cpp
#include <iostream>
#include <memory>

class Person {
public:
    std::string name;
    std::weak_ptr<Person> friendPtr;

    explicit Person(std::string n) : name(std::move(n)) {
        std::cout << name << " created\n";
    }

    ~Person() {
        std::cout << name << " destroyed\n";
    }
};

int main() {
    auto alice = std::make_shared<Person>("Alice");
    auto bob = std::make_shared<Person>("Bob");

    alice->friendPtr = bob;
    bob->friendPtr = alice;
}
```

### Why it works

`weak_ptr` observes an object without increasing its ownership count.

### Interview takeaway

- use `shared_ptr` for shared ownership
- use `weak_ptr` to break cycles

---

## Extra Example: Livelock

This matches the theme in [live_lock.cpp](E:/interview_prep/Preparation_SWE/interview_pr/twisted/live_lock.cpp).

A livelock is different from a deadlock:

- deadlock: threads are stuck and do nothing
- livelock: threads keep reacting and retrying, but no one makes progress

### Simplified idea

```cpp
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex book;
std::mutex pen;

void workerA() {
    while (true) {
        if (book.try_lock()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            if (pen.try_lock()) {
                std::cout << "Worker A completed work\n";
                pen.unlock();
                book.unlock();
                break;
            }
            book.unlock();
        }
    }
}

void workerB() {
    while (true) {
        if (pen.try_lock()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            if (book.try_lock()) {
                std::cout << "Worker B completed work\n";
                book.unlock();
                pen.unlock();
                break;
            }
            pen.unlock();
        }
    }
}
```

### Better approach

Use consistent lock ordering or lock both together:

```cpp
std::scoped_lock lock(book, pen);
```

### Interview takeaway

- `try_lock()` loops can avoid deadlock but still create livelock
- consistent lock ordering is the simplest fix

---

## What Was Wrong in `twisted.txt`

The original note had good topics, but a few things needed correction:

- the null-pointer member function section treated undefined behavior as if it were guaranteed behavior
- the text formatting was collapsed into one long block
- there was broken character encoding in several places
- some examples were too absolute and needed stronger safety disclaimers

---

## Quick Revision Lines

- Non-static member calls through `nullptr` are undefined behavior.
- Private virtual functions can still be overridden.
- `delete this` is legal only under strict ownership rules.
- Private access in C++ is class-based, not object-based.
- Pure virtual functions may still have definitions.
- `weak_ptr` breaks `shared_ptr` ownership cycles.
- Livelock means threads are active but still not making progress.

---

End of twisted_notes.md
