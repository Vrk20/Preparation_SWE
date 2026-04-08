# OOPs in C++

This document is a focused guide to Object-Oriented Programming in C++. It covers the core OOP principles, the supporting sub-concepts, and practical C++ examples that are useful for interviews as well as day-to-day understanding.

---

## What Is OOP?

Object-Oriented Programming is a way of designing software by modeling the program around objects. An object combines:

- data, also called state
- functions, also called behavior

In C++, OOP is built using classes, objects, access specifiers, inheritance, virtual functions, constructors, destructors, and operator/function overloading.

At a high level, the four main OOP principles are:

- Encapsulation
- Abstraction
- Inheritance
- Polymorphism

---

## Class and Object

A `class` is a blueprint. An `object` is a real instance of that blueprint.

```cpp
#include <iostream>
#include <string>
using namespace std;

class Car {
private:
    string brand;
    int speed;

public:
    Car(string b, int s) : brand(b), speed(s) {}

    void display() const {
        cout << "Brand: " << brand << ", Speed: " << speed << '\n';
    }
};

int main() {
    Car c1("BMW", 120);   // object
    c1.display();
}
```

Important idea:

- class defines the structure
- object occupies memory at runtime

---

## Encapsulation

Encapsulation means binding data and methods together inside a class, and controlling direct access to the data.

In C++, encapsulation is mainly achieved using:

- `private`
- `protected`
- `public`

Why it matters:

- prevents invalid modification of object state
- improves maintainability
- hides implementation details

```cpp
#include <iostream>
using namespace std;

class BankAccount {
private:
    double balance;

public:
    BankAccount(double initial) : balance(initial) {}

    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
        }
    }

    bool withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            return true;
        }
        return false;
    }

    double getBalance() const {
        return balance;
    }
};
```

Here, `balance` is private, so outside code cannot modify it directly like `account.balance = -5000;`.

Interview point:

- Encapsulation is not only data hiding
- it also means protecting invariants through controlled APIs

---

## Abstraction

Abstraction means showing only essential behavior and hiding internal implementation details.

A user of the class should know what the object does, not how it does it internally.

Real-world analogy:

- when you drive a car, you use steering, brake, and accelerator
- you do not need to know the engine internals to use it

In C++, abstraction is commonly implemented using:

- classes with clean public interfaces
- abstract classes
- pure virtual functions

```cpp
#include <iostream>
using namespace std;

class Payment {
public:
    virtual void pay(double amount) = 0;
    virtual ~Payment() = default;
};

class UpiPayment : public Payment {
public:
    void pay(double amount) override {
        cout << "Paid " << amount << " using UPI\n";
    }
};

class CardPayment : public Payment {
public:
    void pay(double amount) override {
        cout << "Paid " << amount << " using Card\n";
    }
};
```

The client code only depends on `Payment`, not the low-level payment mechanism.

Difference between encapsulation and abstraction:

- Encapsulation focuses on restricting access and bundling data with methods
- Abstraction focuses on exposing only the relevant interface

---

## Inheritance

Inheritance allows one class to acquire the properties and behavior of another class.

The existing class is called:

- base class
- parent class

The new class is called:

- derived class
- child class

Why it matters:

- code reuse
- logical hierarchy
- extensibility

```cpp
#include <iostream>
using namespace std;

class Animal {
public:
    void eat() const {
        cout << "Animal is eating\n";
    }
};

class Dog : public Animal {
public:
    void bark() const {
        cout << "Dog is barking\n";
    }
};

int main() {
    Dog d;
    d.eat();
    d.bark();
}
```

### Types of Inheritance

#### 1. Single Inheritance

One derived class inherits from one base class.

```cpp
class A {};
class B : public A {};
```

#### 2. Multilevel Inheritance

A class derives from another derived class.

```cpp
class A {};
class B : public A {};
class C : public B {};
```

#### 3. Multiple Inheritance

One class inherits from more than one base class.

```cpp
class Camera {
public:
    void click() const { cout << "Photo taken\n"; }
};

class MusicPlayer {
public:
    void playMusic() const { cout << "Playing music\n"; }
};

class SmartPhone : public Camera, public MusicPlayer {};
```

#### 4. Hierarchical Inheritance

Multiple classes inherit from the same base class.

```cpp
class Vehicle {};
class Car2 : public Vehicle {};
class Bike : public Vehicle {};
```

#### 5. Hybrid Inheritance

Combination of multiple inheritance types.

This often leads to the diamond problem.

---

## Diamond Problem and Virtual Inheritance

When two classes inherit from the same base class, and another class inherits from both, the final class may get duplicate copies of the common base.

```cpp
class Person {
public:
    string name;
};

class Teacher : virtual public Person {};
class Student : virtual public Person {};
class TeachingAssistant : public Teacher, public Student {};
```

Why `virtual` matters:

- without virtual inheritance, `TeachingAssistant` would contain two `Person` parts
- with virtual inheritance, it contains only one shared `Person` base

---

## Polymorphism

Polymorphism means "many forms". The same function call can behave differently depending on the object or arguments involved.

In C++, polymorphism is of two main types:

- Compile-time polymorphism
- Runtime polymorphism

---

## Compile-Time Polymorphism

This is resolved by the compiler.

Common forms:

- function overloading
- operator overloading
- templates

### Function Overloading

Same function name, different parameter lists.

```cpp
#include <iostream>
using namespace std;

class Print {
public:
    void show(int x) {
        cout << "Integer: " << x << '\n';
    }

    void show(double x) {
        cout << "Double: " << x << '\n';
    }

    void show(const string& x) {
        cout << "String: " << x << '\n';
    }
};
```

### Operator Overloading

Operators can be given custom meaning for user-defined types.

```cpp
#include <iostream>
using namespace std;

class Point {
public:
    int x, y;

    Point(int x, int y) : x(x), y(y) {}

    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }
};
```

Interview point:

- compile-time polymorphism is generally faster because binding is decided earlier

---

## Runtime Polymorphism

This is achieved using:

- inheritance
- virtual functions
- base class pointer or reference

```cpp
#include <iostream>
using namespace std;

class Shape {
public:
    virtual void draw() const {
        cout << "Drawing shape\n";
    }

    virtual ~Shape() = default;
};

class Circle : public Shape {
public:
    void draw() const override {
        cout << "Drawing circle\n";
    }
};

class Rectangle : public Shape {
public:
    void draw() const override {
        cout << "Drawing rectangle\n";
    }
};

int main() {
    Shape* s1 = new Circle();
    Shape* s2 = new Rectangle();

    s1->draw();
    s2->draw();

    delete s1;
    delete s2;
}
```

Here, the call to `draw()` depends on the actual object type, not just the pointer type.

Interview point:

- runtime polymorphism uses dynamic binding
- it is typically implemented internally using a vtable mechanism

---

## Virtual Function

A virtual function allows a derived class to provide a specific implementation of a function already defined in the base class.

Rules to remember:

- declare the function as `virtual` in the base class
- use `override` in the derived class for safety
- use base pointer or base reference to observe runtime polymorphism

```cpp
class Base {
public:
    virtual void show() const {
        cout << "Base show\n";
    }
};

class Derived : public Base {
public:
    void show() const override {
        cout << "Derived show\n";
    }
};
```

---

## Pure Virtual Function and Abstract Class

A pure virtual function has no implementation in the base class and forces derived classes to implement it.

```cpp
class Device {
public:
    virtual void start() = 0;
    virtual ~Device() = default;
};
```

A class containing at least one pure virtual function is called an abstract class.

Properties of abstract classes:

- objects of abstract class cannot be created
- they are used as interfaces or common contracts
- derived classes must implement all pure virtual functions unless they also remain abstract

---

## Function Overriding vs Function Overloading

### Overloading

- same function name
- same scope
- different parameter list
- compile-time polymorphism

### Overriding

- function in derived class redefines virtual function from base class
- same signature
- different implementation
- runtime polymorphism

```cpp
class Base2 {
public:
    virtual void greet() const {
        cout << "Hello from Base\n";
    }
};

class Derived2 : public Base2 {
public:
    void greet() const override {
        cout << "Hello from Derived\n";
    }
};
```

---

## Access Specifiers

Access specifiers control visibility of class members.

### `private`

- accessible only inside the same class

### `protected`

- accessible inside the class and derived classes
- not accessible directly from outside code

### `public`

- accessible from anywhere through the object interface

```cpp
class Example {
private:
    int a;

protected:
    int b;

public:
    int c;
};
```

Interview point:

- classes use `private` by default
- structs use `public` by default

---

## Modes of Inheritance

In C++, inheritance mode affects how base class members appear in the derived class.

### Public Inheritance

- public remains public
- protected remains protected
- models "is-a" relationship

### Protected Inheritance

- public becomes protected
- protected remains protected

### Private Inheritance

- public becomes private
- protected becomes private

Public inheritance is the most common one in OOP design.

---

## Constructor

A constructor initializes an object when it is created.

Types of constructors:

- default constructor
- parameterized constructor
- copy constructor
- move constructor

```cpp
class StudentInfo {
private:
    string name;
    int age;

public:
    StudentInfo() : name("Unknown"), age(0) {}

    StudentInfo(string n, int a) : name(n), age(a) {}

    StudentInfo(const StudentInfo& other) : name(other.name), age(other.age) {}
};
```

Important points:

- constructor name is same as class name
- constructors have no return type
- constructors can be overloaded

---

## Destructor

A destructor is called automatically when an object goes out of scope or is deleted.

It is mainly used for cleanup.

```cpp
class FileHandler {
public:
    FileHandler() {
        cout << "Resource acquired\n";
    }

    ~FileHandler() {
        cout << "Resource released\n";
    }
};
```

Important points:

- destructor name is `~ClassName()`
- it has no parameters and no return type
- only one destructor per class

### Virtual Destructor

If a base class is meant to be used polymorphically, its destructor should usually be virtual.

```cpp
class Base3 {
public:
    virtual ~Base3() {
        cout << "Base destructor\n";
    }
};

class Derived3 : public Base3 {
public:
    ~Derived3() {
        cout << "Derived destructor\n";
    }
};
```

Why this matters:

- deleting a derived object through a base pointer without a virtual destructor causes undefined behavior

---

## `this` Pointer

`this` is an implicit pointer available inside non-static member functions. It points to the current object.

```cpp
class Box {
private:
    int length;

public:
    void setLength(int length) {
        this->length = length;
    }
};
```

Common use cases:

- resolving name conflicts
- returning current object using `return *this`
- checking self-assignment in copy assignment operator

---

## Static Members in OOP

Static data members are shared by all objects of a class.

Static member functions belong to the class, not to a specific object.

```cpp
class Employee {
private:
    static int count;

public:
    Employee() {
        ++count;
    }

    static int getCount() {
        return count;
    }
};

int Employee::count = 0;
```

Use cases:

- counting objects
- storing class-level configuration
- utility methods related to the class

---

## Association, Aggregation, and Composition

These are important object relationships often discussed with OOP design.

### Association

General relationship between two independent classes.

Example:

- Teacher teaches Student

### Aggregation

A "has-a" relationship where the contained object can exist independently.

Example:

- Department has Professors
- professors can still exist even if the department is removed

### Composition

A strong "has-a" relationship where the contained object depends on the owner for lifetime.

Example:

- House has Rooms
- if house is destroyed, rooms are also destroyed with it

```cpp
class Engine {
public:
    void start() const {
        cout << "Engine started\n";
    }
};

class CarWithEngine {
private:
    Engine engine;   // composition

public:
    void drive() {
        engine.start();
        cout << "Car is moving\n";
    }
};
```

---

## Shallow Copy vs Deep Copy

This is a very important OOP sub-concept in C++ because objects may manage dynamic memory.

### Shallow Copy

- copies member values as they are
- if a member is a pointer, only the pointer address is copied
- two objects may end up pointing to the same memory

### Deep Copy

- allocates separate memory
- copies the actual data
- each object owns its own resource

```cpp
#include <cstring>

class StringHolder {
private:
    char* data;

public:
    StringHolder(const char* str) {
        data = new char[strlen(str) + 1];
        strcpy(data, str);
    }

    StringHolder(const StringHolder& other) {
        data = new char[strlen(other.data) + 1];
        strcpy(data, other.data);
    }

    ~StringHolder() {
        delete[] data;
    }
};
```

Interview point:

- if your class manages a resource manually, copying behavior must be designed carefully

---

## Rule of Three, Five, and Zero

### Rule of Three

If a class defines any one of these, it likely needs all three:

- destructor
- copy constructor
- copy assignment operator

### Rule of Five

With move semantics, two more are added:

- move constructor
- move assignment operator

### Rule of Zero

Best modern approach:

- avoid manual resource management
- use standard library types like `std::string`, `std::vector`, and smart pointers
- then you usually do not need to write special member functions manually

---

## Real Interview Example Combining Multiple OOP Concepts

```cpp
#include <iostream>
#include <memory>
using namespace std;

class Vehicle {
public:
    virtual void start() const = 0;
    virtual ~Vehicle() = default;
};

class CarVehicle : public Vehicle {
public:
    void start() const override {
        cout << "Car starts with key ignition\n";
    }
};

class BikeVehicle : public Vehicle {
public:
    void start() const override {
        cout << "Bike starts with self-start\n";
    }
};

int main() {
    unique_ptr<Vehicle> v1 = make_unique<CarVehicle>();
    unique_ptr<Vehicle> v2 = make_unique<BikeVehicle>();

    v1->start();
    v2->start();
}
```

Concepts used here:

- abstraction through `Vehicle`
- inheritance through derived classes
- runtime polymorphism through `start()`
- safe cleanup through virtual destructor and `unique_ptr`

---

## Common OOP Interview Questions

### What is the difference between class and object?

- class is a blueprint
- object is an instance of a class

### What is the difference between encapsulation and abstraction?

- encapsulation hides data and controls access
- abstraction hides implementation details and shows essential behavior

### Why do we use virtual functions?

- to enable runtime polymorphism

### Why should base class destructor be virtual?

- to ensure proper cleanup when deleting derived objects through base pointers

### What is the difference between overloading and overriding?

- overloading happens in the same scope with different parameters
- overriding happens across base and derived classes with virtual functions

### What problem does virtual inheritance solve?

- it solves duplicate base-class instances in diamond inheritance

---

## Quick Revision Points

- OOP in C++ is centered around classes and objects
- the 4 pillars are encapsulation, abstraction, inheritance, and polymorphism
- encapsulation protects state using access control
- abstraction exposes only what is needed
- inheritance supports reuse and hierarchy
- polymorphism allows one interface to behave differently
- virtual functions enable runtime polymorphism
- abstract classes define common contracts
- destructors must be virtual in polymorphic base classes
- deep copy is necessary when a class owns dynamic memory
- modern C++ prefers Rule of Zero whenever possible

---

End of oops.md
