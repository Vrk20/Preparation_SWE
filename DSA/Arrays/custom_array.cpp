#include<iostream>

template <typename T>
class Myvec
{
private:
    T* arr;
    size_t size;
    size_t capacity;
public:
    Myvec():size(0),capacity(1){arr = new T[capacity];};
    Myvec(const Myvec& other):size(other.size),capacity(other.capacity){
        arr = new T[capacity];
        for (size_t i = 0; i < size; i++)
        {
            arr[i]  = other.arr[i];
        }
    }
    Myvec& operator=(const Myvec& other){
        if(this == &other) return *this;
        delete[] arr;
        size = other.size;
        capacity = other.capacity;
        arr = new T[capacity];
        for (size_t i = 0; i < size; i++)
        {
            arr[i] = other.arr[i];
        }
        return *this;
    }
    Myvec(Myvec&& other)noexcept{
        arr = other.arr;
        size = other.size;
        capacity = other.capacity;
        other.arr = nullptr;
        other.size = 0;
        other.capacity =0;
    }
    Myvec& operator=(Myvec&& other) noexcept{
        if(this == &other) return *this;
        delete[] arr;
        arr = other.arr;
        size = other.size;
        capacity = other.capacity;
        other.arr = nullptr;
        other.size = 0;
        other.capacity =0;
        return *this;
    }
    void resize(){
        capacity*=2;
        T* new_arr = new T[capacity];
        for (size_t i = 0; i < size; i++)
        {
            new_arr[i] = arr[i];
        }
        delete[] arr;
        arr = new_arr;
        
    }
    void push_back(const T& element){
        if(size == capacity){
            resize();
        }
        arr[size] = element;
        ++size;
    }
    void pop_back(){
        if(size > 0){
            --size;
        }
    }
    size_t get_size() const{
        return size;
    }
    size_t get_capacity() const{
        return capacity;
    }
    T& operator[](size_t index){
        if(index>=size){
            throw std::out_of_range("Index out of bounds");
        }
        return arr[index];
    }
    ~Myvec(){delete[] arr;}

};


int main(){
    Myvec<int> vec1;
    vec1.push_back(10);
    vec1.push_back(20);

    // Test copy constructor
    Myvec<int> vec2 = vec1;
    
    // Test move constructor
    Myvec<int> vec3 = std::move(vec1);

    // Test copy assignment
    vec2 = vec3;

    // Test move assignment
    vec3 = std::move(vec2);

    std::cout << "Vector elements in vec3: ";
    for (size_t i = 0; i < vec3.get_size(); ++i) {
        std::cout << vec3[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}