#include<iostream>
#include<string>
void convlower(std::string& s);

int vowelcount(std::string& s){
    std::string vowels = "aeiou";
    int count = 0;
    convlower(s);
    for(char c: s){
        for(char ac:vowels){
            if(ac == c){
                count++;
            }
        }
    }
    return count;
}

int u_vowelcount(std::string& s){
    std::string vowels = "aeiou";
    int count = 0;
    convlower(s);
    for(char c: s){
        for(char ac:vowels){
            if (vowels.find(c) != std::string::npos)
            count++;
        }
    }
    return count;
}

void convlower(std::string& s){
    for(char& c : s){
        c = std::tolower(static_cast<unsigned char>(c));
    }
}

void reversestring(std::string& s){
    int l = 0, r = s.size() - 1;
    while(l < r){
        std::swap(s[l],s[r]);
        l++,r--;
    }
}

void removespaces(std::string& s){
    std::string ns;
    for(char c : s){
        if(isalnum(c)){
            ns.push_back(c);
        }
    }
    s = ns;
}

void u_removespaces(std::string& s){
    std::string ns;
    for(char c : s){
        if(!std::isspace(static_cast<unsigned char>(c))){
            ns.push_back(c);
        }
    }
    s = ns;
}

int countwords(std::string& s){
    int count = 0;
    for(char c : s){
        if(!isalnum(c)){
            count++;
        }
    }
    return count+1;
}
int u_countwords(std::string& s){
    int count = 0;
    bool inword = false;
    for(char c : s){
        if(!std::isspace(static_cast<unsigned char>(c))){
            if(!inword){
                count++;
                inword = true;
            }
        }
            else{
                inword = false;
            }
    }
    return count;
}

int main(){
    std::string s = "EMBEDDED";
    std::string lw = "real time operating system";
    std::string lws = "low level code";
    std::string ns;
    std::cout<<"count words "<<countwords(lw)<<"\n";
    std::cout<<"count words mod "<<u_countwords(lw)<<"\n";
    int count = 0;
    for( char c : lws){
        if(isalnum(c)){
            count++;
            ns.push_back(c);
        }
    }
    std::cout<<"unmod "<<vowelcount(s)<<"\n";
    std::cout<<"mod "<<vowelcount(s)<<"\n";

    u_removespaces(lws);
    std::cout<<"lws is\t"<<lws<<"\n";

    std::string v = "ver9sion";

    for(char a :v){
        if(!isalpha(a))
            std::cout<<a;
    }
}