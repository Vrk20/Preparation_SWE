#include<iostream>
#include<cstring>


/*
    Example : Shallow Copy vs Deep Copy in c++

    Shallow Copy:
    - When an object is created using another object (default copy constructor),
    pointer or dynamic member vairables share the same memory.
    - Any change in one object's pointer data will reflect in teh copied object.
    -Risk: double free or memory corruption

    Deep Copy:
    -To avoid shallow copy issues, define an explicit copy constructor.
    -Allocate new memory for pointer members and copy content explicitly.
    -Ensures each object manages its own memory safely.
 
*/

// Shallow Copy class
class shallow{
    private:
        char* Name; // dynamically allocated string
        int Age;
    public:
        //Constructor
        shallow(char* c, int n){
            Name = new char[strlen(c)+1];
            strcpy(Name,c);
            Age = n;
        }
        //setter for Name
        void setName(char* n){
            strcpy(Name,n);
        }
        //setter for Age
        void setAge(int a){
            Age = a;
        }
        //Display function
        void display(){
            std::cout<<"--------------\n";
            std::cout<<"Name :\t"<<Name<<"\n";
            std::cout<<"Age: \t"<<Age<<"\n";
        }
        //Destructor
        ~shallow(){
            delete[] Name;
            std::cout<<"Shallow destructor is called\n";
        }

};

// Deep Copy class
class deep{
    private:
        char* Name; //dynamically allocated string
        int Age;
    public:
        //Constructor
        deep(char* c,int a){
            Name = new char[strlen(c)+1]; 
            strcpy(Name,c);
            Age = a;
        }
        //Copy constructor
        deep(const deep& d){
            Name = new char[strlen(d.Name)+1]; // allocate new memory
            strcpy(Name,d.Name);               // copy content
            Age = d.Age;
        }
        //Setter for Name
        void setName(char *c){
            strcpy(Name,c);
        }
        // Setter for Age
        void setAge(int a){
            Age = a;
        }
        //Display function
        void display(){
            std::cout<<"---------------\n";
            std::cout<<"Name:\t"<<Name<<"\n";
            std::cout<<"Age:\t"<<Age<<"\n";
        }
        //Destructor
        ~deep(){
            delete[] Name;
            std::cout<<"Deep destructor is called\n";}
};

int main(){
    // shallow copy example 
    std::cout<<"Shallow Copy example \n";
    shallow s1("Alex",40);
    s1.display();

    shallow s2 = s1; // default copy contructor (shallow copy)
    s2.display();

    s1.setName("Bob"); //modifies the shared memory
    s1.setAge(20);

    std::cout<<"\nAfter modifying s1:\n";
    s1.display();
    s2.display();

    // deep copy example 
    std::cout<<"Deep Copy example \n";
    deep d1("John",20);
    d1.display();

    deep d2 = d1; //uses explicit deep copy constructor
    d2.display();

    d1.setName("Mark"); //modifies only d1's memory
    d1.setAge(25);

    std::cout<<"\nAfter modifying d1:\n";
    d1.display();
    d2.display();  //d2 remains unchanged

    return 0;
}