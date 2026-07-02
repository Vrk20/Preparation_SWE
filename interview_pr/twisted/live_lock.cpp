#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>

std::mutex book;
std::mutex pen;

void workerA(){
    while (true)
    {
        if(book.try_lock()){
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            if(pen.try_lock()){
                std::cout<<"Worker A Completed work\n";
                pen.unlock();
                book.unlock();
                break;
            }
            else{
                book.unlock();
            }
        }
    }
}

void workerB(){
    while (true)
    {
        if(pen.try_lock()){
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            if(book.try_lock()){
                std::cout<<"Worker B Completed Work\n";
                book.unlock();
                pen.unlock();
                break;
            }
            else{
                pen.unlock();
            }
        }
    }
}

int main(){
    std::thread t1(workerA);
    std::thread t2(workerB);

    t1.join();
    t2.join();
}