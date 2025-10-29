#include<iostream>
#include<cstring>

class Mystring{
private:
    char* m_char;
public:
    Mystring(const char* temp){
        int len = length(temp);
        m_char = new char[len+1];
        copy(temp,m_char);
        std::cout<<"This is default constructor\n";
    }
    int length(const char* str){
        int count = 0;
        while(str[count]!='\0'){
            count++;
        }
        return count;
    }
    void copy(const char* src,char* dest){
        int index=0;
        while(src[index]!='\0'){
            dest[index] = src[index];
            index++;
        }
        dest[index]='\0';
    }
    void concat(const char* a, const char* b){
        int lena= length(a);
        int lenb = length(b);
        char* result = new char[lena+lenb+1];
        int i=0;
        for(;i<lena;i++)
            result[i] = a[i];
        for(int j=0;j<lenb;++j,++i)
            result[i]=b[j];
        result[i]='\0';

    }
    bool compare(const char* a,const char* b){
        int index= 0;
        while (a[index]!='\0' && b[index]!='\0')
        {
            if(a[index]!=b[index]){
                return false;
            }
            index++;
        }
        return(a[index]=='\0' && b[index]=='\0');
        
    }
    void reverse(char* str){
        int start = 0;
        int end = length(str)-1;
        while(start<end){
            char temp = str[start];
            str[start] = str[end];
            str[end] = temp;
            start++;
            end--;
        }
    }
    void print(){
        std::cout<<m_char;
    }
    ~Mystring(){
        delete[] m_char;
    }

};


int main(){
    // Mystring str;
    Mystring s1("Hello");
    s1.print();

    return 0;
}