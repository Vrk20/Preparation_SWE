#include<iostream>
#define MAX_SIZE 5
using namespace std;

class Queue{
    int arr[MAX_SIZE];
    int front,rear,count;
    public:
        Queue(){
            front = 0;
            rear = -1;
            count=0;
        }
        void enqueue(int item){
            if(isfull()){
                cout<<"queue is full\n";
                return;
            }
            rear = rear+1;
            arr[rear] = item;
            count++;
            cout<<"enqueued element is "<< item<<endl;

        }
        void dequeue(){
            if(isempty()){
                cout<<"queue is empty\n";
                return;
            }
            int res = arr[front];
            front = front+1;
            count--;
            cout<<"dequeued element is "<< res<<endl;
            
        }
        int peek(){
            if(isempty()){
                cout<<"Queue is empty\n";
                return -1;
            }
            return arr[front];
        }
        bool isempty(){
            return (count == 0);
        };
        bool isfull(){
            return (count == MAX_SIZE);
        };
        void display(){
            if(isempty()){
                cout<<"queue is empty\n";
                return;
            }
            for(int i=0;i<count;i++){
                cout<<arr[i]<<" ";
            }
            cout<<endl;
        };
        ~Queue(){}

};

int main(){
    Queue q;
    q.enqueue(10);
    q.enqueue(20);
    q.enqueue(30);
    q.enqueue(40);
    q.display();
    cout<<"peeking "<<q.peek()<<endl;
    q.dequeue();
    q.display();
    q.enqueue(50);
    q.enqueue(60);
    q.display();
    return 0;
}