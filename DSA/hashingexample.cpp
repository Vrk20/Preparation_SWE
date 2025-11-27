#include<iostream>
#include<memory>
#include<vector>
using namespace std;

struct Node{
    string key;
    int value;
    unique_ptr<Node> next;
    Node(const string& k, int v) : key(k), value(v), next(nullptr) {}

};

class HashTable{
    public:
    int bucket_count;
    vector<unique_ptr<Node>> buckets;
    HashTable(int n):bucket_count(n),buckets(n){}
    int HashFunc(const string& key){
        int sum =0;
        for(char c: key)sum+=c;
        return sum%bucket_count;
    }
    void insert(const string& key,int value){
        int index = HashFunc(key);
        if(!buckets[index]){
            buckets[index] = make_unique<Node>(key,value);
            return;
        }
        Node* curr = buckets[index].get();
        while(true){
            if(curr->key == key){
                curr->value = value;
                return;
            }
            if(!curr->next)break;
            curr = curr->next.get();
        }
        curr->next = make_unique<Node>(key,value);
    }
    int* search(const string& key){
        int index = HashFunc(key);
        Node* curr = buckets[index].get();
        while(curr){
            if(curr->key == key){
                return &curr->value;
            }
            curr = curr->next.get();
        }
        return nullptr;
    }

};

int main() {
    HashTable ht(5);

    ht.insert("a", 1);
    ht.insert("b", 2);
    ht.insert("c", 3);
    ht.insert("aa", 4);

    if (int* v = ht.search("c"))
        cout << "c = " << *v << endl;
    else
        cout << "c not found" << endl;

    if (int* v = ht.search("aa"))
        cout << "aa = " << *v << endl;

    if (!ht.search("z"))
        cout << "z not found" << endl;

    return 0;
}
