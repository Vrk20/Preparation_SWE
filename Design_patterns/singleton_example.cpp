#include<iostream>

class Singleton{
    private:
        Singleton(){};
        Singleton(const Singleton&) = default;
        Singleton& operator=(const Singleton&) = default;
    public:
        static Singleton& getInstance(){
            static Singleton instance;
            return instance;
        }
};


int main(){
    Singleton& s1 = Singleton::getInstance();
    Singleton& s2 = Singleton::getInstance();
    return 0;
}
