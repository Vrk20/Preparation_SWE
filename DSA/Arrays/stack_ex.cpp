/*
    The below is an example of a stack data structure
*/
#include<iostream>
using namespace std;

class Stack{
    int* arr;
    size_t capacity;
    int top;
    public:
        Stack(int a){
            capacity = a;
            arr = new int[capacity];
            top = -1;
        }
        void push(int x){
            if(isfull()){
                cout<<"Stack Overflow: Cannot push element "<<x<<"\n";
                return;
            }
            arr[++top]=x;
            cout<<"Pushed : "<<x<<endl;
        }
        int pop(){
            if(isempty()){
                cout<<"Stack Underflow: Cannot pop from empty stack\n";
                return -1;
            }
            else{
                int popped = arr[top--];
                cout<<"Popped: "<<popped<<"\n";
                return popped;
            }
        }
        int peek(){
            if(!isempty()){
                return arr[top];
            }
        }
        bool isfull(){
            return top == capacity-1;
        }
        bool isempty(){
            return top == -1;
        }
        int size(){
            return top+1;
        };
        ~Stack(){
            delete[] arr;
        }
};

int main(){
    Stack s(5);
    s.push(10);
    s.push(20);
    cout<<"Top element: "<<s.peek()<<"\n";
    s.pop();
    s.pop();
    cout << "Is stack empty? " << (s.isempty() ? "Yes" : "No") <<"\n";
    s.push(40);
    s.push(50);
    s.push(60);
    return 0;
}