#include <iostream>
#include <memory>
#include <vector>

class Person{
    public:
        std::string name;
        //weak_ptr deletes the references and correctly deletes the object
        std::weak_ptr<Person> friendPtr;

        //not use the shared_ptr<Person> friendPtr; it will create another reference count

        Person(std::string name):name(name){
            std::cout<<name<<" created\n";
        }

        ~Person(){
            std::cout<<name<<" destroyed\n";
        }
};


class Session;

class Server{
    public:
        std::vector<std::shared_ptr<Session>> sessions;

        void addSessions(std::shared_ptr<Session> s){
            sessions.push_back(s);
        }
        
        ~Server(){
            std::cout<<"Server destroyed\n";
        }
};

class WrongSession{
    public:
        std::shared_ptr<Server> server;
        WrongSession(std::shared_ptr<Server> srv):server(srv){}

        ~WrongSession(){
            std::cout<<"Session Destroyed\n";
        }
};  

class Session{
    public:
        std::weak_ptr<Server>server;
        Session(std::shared_ptr<Server>srv):server(srv){}
        void sendSomething(){
            if(auto s = server.lock()){
                std::cout<<"Server still alive\n";
            }
        }
        ~Session(){
            std::cout<<"Session Destroyed\n";
        }
};
int main(){

    // std::shared_ptr<Person> p1 = std::make_shared<Person>("Alice");
    // std::shared_ptr<Person> p2 = std::make_shared<Person>("Bob");
    // p1->friendPtr = p2;
    // p2->friendPtr = p1;

    auto server = std::make_shared<Server>();

    auto s1 = std::make_shared<Session>(server);
    auto s2 = std::make_shared<Session>(server);

    server->addSessions(s1);
    server->addSessions(s2);
    std::cout<<"End of Main\n";

    return 0;
}