#include<iostream>


/*
    Templates are used to write generic code that works with any data type
    instead of mentioning explicit data type for function or classes writing one template
    and compiler will generate the correct version while using it.
    Types of templates 
        1. Function Templates
        2. Class Templates
*/

//Function Templates
template <typename T>
T add(T a,T b){
    return a+b;
}

//Class Templates
template<class T>
class Pair{
    T a, b;
    public:
        Pair(T x,T y):a(x),b(y){}
        void display(){std::cout<<a<<" , "<<b<<"\n";}
};


//Class Templates with mulitple parameters
template<class T1,class T2>
class Myclass{
    T1 a;
    T2 b;
    public:
        Myclass(T1 x,T2 y):a(x),b(y){}
        void display(){std::cout<< a <<" and "<<b<<std::endl;}
};

int main(){
    //function examples 
    std::cout<<"Int example\t"<<add(3,5)<<"\n"; //same function as int 
    std::cout<<"Float example\t"<<add(37.35,54.6)<<"\n"; // as float
    std::cout<<"String example\t"<<add(std::string("New"),std::string(" world"))<<"\n"; //as string

    //class examples
    Pair<int> p1(10,20);
    p1.display();
    Pair<double> p2(23.44,55.6);
    p2.display();
    Pair<std::string> p3("Hello","world");
    p3.display();

    Myclass<int,double> m1(10,43.4);
    m1.display();
    Myclass<std::string,int> m2("Hello",53);
    m2.display();
    return 0;
}