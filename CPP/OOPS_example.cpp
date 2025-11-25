#include<iostream>

//basic class 
class point{
    public:
    int x,y;
    void print() const{std::cout<<"("<<x<<","<<y<<")\n";}
};

//encapsulation using private/ protected and expose only safe Interfaces
class BankAccount{
    private:
        double balance;
    public:
        BankAccount(double b=0):balance(b){}
        void deposit(double a){ if(a>0) balance+=a;}
        double get_balance()const{return balance;}
};

//Abstraction expose only necessary and hiding implementation details 
class Logger{
    public:
        virtual void log(const char* msg)=0; //pure virtual function or abstract
        virtual ~Logger()=default;
};

class ConsoleLogger : public Logger{
    public:
        void log(const char* msg) override{std::cout<<msg<<"\n";}
};


//Inheritance reuse or extend existing behaviour
class Animal{
    public:
        virtual void speak(){std::cout<<"...";}
};

class Dog:public Animal{public: void speak()override {std::cout<<"barks\n";}};

//Diamond problem & virtual Inheritance
class A {public: int v;};
class C :virtual public A{};
class B :virtual public A{};
class D : public B,public C{}; //single A subobject in D 

//Polymorphism 
// ---compile-time(function overloading/templates/operator overloading)
struct Math{
    int add(int a,int b){return a+b;}
    double add(double a,double b){return a+b;}
};
//operator overloading
struct Vec{
    int x,y;
    Vec operator+(const Vec& o) const {return {x+o.x,y+o.y;}}
};


int main(){
    point p{1,2};
    p.print();
    Animal* a = new Dog();
    a->speak(); // calls Dog:: speak because of virtual
    delete a; //must have virtual ~Animal()
}

//Abstract classes & Interfaces
class Shape{
    public:
        virtual double area() const=0; //pure virtual function 
        virtual ~Shape() = default;
};


//Rule of five
class Resource{
    int* data;
    public:
        Resource():data(new int[10]){};
        ~Resource(){delete[] data;};
        Resource(Resource& other):data(new int[10]){
            std::copy(other.data,other.data+10,data);
        }
        Resource& operator=(const Resource& other) {
            if(this == &other) return *this;
            int* tmp = new int[10];
            std::copy(other.data,other.data+10,tmp);
            delete[] data; data=tmp;
            return *this;
        }
        Resource(Resource&& other)noexcept: data(other.data){
            other.data=nullptr;
        };
        Resource& operator=(Resource&& other) noexcept{
            if(this == &other) return *this;
            delete[] data; 
            data = other.data; 
            other.data = nullptr;
            return *this;
        }
};

