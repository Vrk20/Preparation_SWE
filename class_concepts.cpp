#include <iostream>
#include <string>
#include <utility>

class Person {
private:
    std::string name;
    int age;
    int* scores;                // raw pointer (to demonstrate shallow vs deep copy)
    static int population;

public:
    // 1. Default constructor
    Person() : name("Unknown"), age(0), scores(new int(0)) {
        ++population;
        std::cout << "Default constructor called\n";
    }

    // 2. Parameterized constructor
    Person(std::string n, int a, int score)
        : name(std::move(n)), age(a), scores(new int(score)) {
        ++population;
        std::cout << "Parameterized constructor called\n";
    }

    // 3. Copy constructor (Deep Copy)
    Person(const Person& other)
        : name(other.name), age(other.age), scores(new int(*other.scores)) {
        ++population;
        std::cout << "Copy constructor (deep copy) called\n";
    }

    // 4. Move constructor
    Person(Person&& other) noexcept
        : name(std::move(other.name)), age(other.age), scores(other.scores) {
        other.scores = nullptr; // transfer ownership
        ++population;
        std::cout << "Move constructor called\n";
    }

    // 5. Copy assignment (Deep Copy)
    Person& operator=(const Person& other) {
        if (this != &other) {
            name = other.name;
            age = other.age;
            delete scores;                       // free existing
            scores = new int(*other.scores);     // allocate new memory
        }
        std::cout << "Copy assignment (deep copy) called\n";
        return *this;
    }

    // 6. Move assignment
    Person& operator=(Person&& other) noexcept {
        if (this != &other) {
            name = std::move(other.name);
            age = other.age;

            delete scores;
            scores = other.scores;
            other.scores = nullptr;
        }
        std::cout << "Move assignment called\n";
        return *this;
    }

    // 7. Destructor
    virtual ~Person() {
        --population;
        delete scores; // safe cleanup
        std::cout << "Destructor called for " << name << "\n";
    }

    // 8. Virtual function
    virtual void introduce() const {
        std::cout << "Hi, I'm " << name
                  << ", age " << age
                  << ", score " << *scores << ".\n";
    }

    // 9. Const member function
    int getAge() const { return age; }

    // 10. Static member function
    static int getPopulation() { return population; }

    // 11. Operator overload (==)
    bool operator==(const Person& other) const {
        return name == other.name && age == other.age && *scores == *other.scores;
    }

    // 12. Operator overload (<<)
    friend std::ostream& operator<<(std::ostream& os, const Person& p) {
        return os << p.name << " (" << p.age << ", score=" << *p.scores << ")";
    }

    // Utility function to update score
    void setScore(int s) { *scores = s; }
};

// Initialize static member
int Person::population = 0;

// Derived class
class Student : public Person {
    std::string course;
public:
    Student(std::string n, int a, std::string c, int score)
        : Person(std::move(n), a, score), course(std::move(c)) {}

    void introduce() const override {
        std::cout << "I'm a student in " << course << ".\n";
    }
};

int main() {
    Person p1("Alice", 25, 90);
    Person p2 = p1;         // Copy constructor (deep copy)

    std::cout << "Before modifying p1:\n";
    std::cout << p1 << "\n" << p2 << "\n";

    p1.setScore(100);       // change only p1’s score

    std::cout << "After modifying p1:\n";
    std::cout << p1 << "\n" << p2 << "\n";

    Person p3 = std::move(p1);  // Move constructor

    std::cout << "Population: " << Person::getPopulation() << "\n";

    return 0;
}
