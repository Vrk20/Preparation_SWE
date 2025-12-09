#include<iostream>

template<typename T>
class UniquePtr{
    T* ptr;
    public:
        UniquePtr(): ptr(nullptr){}
        explicit UniquePtr(T* p):ptr(p){}
        UniquePtr(const UniquePtr& other) = delete;
        UniquePtr& operator=(const UniquePtr& other) = delete;
        UniquePtr(UniquePtr&& other) noexcept:ptr(other.ptr){
            other.ptr = nullptr;
        }
        UniquePtr& operator=(UniquePtr&& other) noexcept{
            if(this!=&other){
                delete ptr;
                ptr = other.ptr;
                other.ptr = nullptr;
            }
            return *this;
        }
    ~UniquePtr()
    {
        delete ptr;
    }
    
    T& operator*() const{
        return *ptr;
    }
    T* operator->() const{
        return ptr;
    }
    T* get() const{
        return ptr;
    }

    T* release(){
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }
    
    void reset(T* p = nullptr){
        delete ptr;
        ptr = p;
    }

    explicit operator bool() const{
        return ptr!= nullptr;
    }

};

struct Foo
{
    int x;
    Foo(int v):x(v){
        std::cout<<"Foo Constructor\n";
    }
    ~Foo(){
        std::cout<<"Foo destroyed\n";
    }
};

int main(){
    UniquePtr<Foo> p1(new Foo(10));
    std::cout<< p1->x<<"\n";

    UniquePtr<Foo> p2 =std::move(p1);
    if(!p1){
        std::cout<<"p1 is null\n";
    }
    std::cout<< p2->x <<"\n";
}