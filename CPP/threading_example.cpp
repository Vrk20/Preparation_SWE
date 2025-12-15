#include<iostream>
#include<mutex>
#include<atomic>
#include<thread>
#include<chrono>

using namespace std;

int counter = 0;
atomic<int> at = 0;
mutex m;
void increment(){
    for (size_t i = 0; i < 100000; i++)
    {
        lock_guard<mutex> lock(m);
        counter++;
    }
}

void atomicVar(){
    for (size_t i = 0; i < 100000; i++)
    {
        at++;
    }
}
int main(){
    thread t1(increment);
    thread t2(increment);
    thread t3(atomicVar);
    cout<<t1.get_id()<<"\n";
    cout<<t2.get_id()<<"\n";
    cout<<t3.get_id()<<"\n";
    auto start = chrono::high_resolution_clock::now();
    t1.join();
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> duration = end - start;
    cout << "Time taken by function_to_measure: " 
              << duration.count() << " milliseconds" << "\n";
    start = chrono::high_resolution_clock::now();
    t2.join();
    end = chrono::high_resolution_clock::now();
    duration = end - start;
     cout << "Time taken by function_to_measure: " 
              << duration.count() << " milliseconds" <<  "\n";
    start = chrono::high_resolution_clock::now();
    t3.join();
    end = chrono::high_resolution_clock::now();
    duration = end - start;
    cout << "Time taken by function_to_measure: " 
              << duration.count() << " milliseconds" <<  "\n";
    cout << "counter = " << counter << "\n";
    cout << "counter = " << at << "\n"; 

}


