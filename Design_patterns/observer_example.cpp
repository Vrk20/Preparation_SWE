#include<iostream>
#include<vector>
class Observer{
    public:
        virtual void update(int value) = 0;
        virtual ~Observer()=default;
};

class Subject
{
private:
    std::vector<Observer*> observers;
    int state;
    void notify(){
        std::cout<<"notified\n";
        for (auto* obs:observers){
            obs->update(state);
        }
    }
public:
    void attach(Observer* obs){
        observers.push_back(obs);
    }
    void setState(int s){
        state = s;
        notify();
    }
};

class ConcreteObserver : public Observer {
public:
    void update(int value) override {
        std::cout << "Observer received value: " << value << std::endl;
    }
};


int main(){
    Subject s1;
    ConcreteObserver o1;
    s1.attach(&o1);
    s1.setState(4);
}
