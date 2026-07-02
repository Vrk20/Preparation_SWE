#include<iostream>
#include<string>

bool isvowel(char c){
    if((c== 'a')|| (c=='e') || (c=='i')|| (c=='o')|| (c=='u'))
        return true;
    return false;
}
int maxvowel(std::string s,int k){
    int count=0;
    int maxcount;
    for(size_t i = 0;i<k;i++){
        std::cout<<s[i]<<"\t";
        if(isvowel(s[i])){
            count++;
        }
    }
    std::cout<<std::endl;

    maxcount = count;
    for(size_t i= k;i<s.length();i++){
        std::cout<<s[i-k]<<"\t"<<s[i]<<"\n";
        if(isvowel(s[i-k])){
            count--;
        }
        if(isvowel(s[i])){
            count++;
        }
        maxcount = std::max(maxcount,count);
    }
    std::cout<<std::endl;
    return maxcount;
}

int main(){
    std::string s= "abciiidef";
    std::cout<<maxvowel(s,3);
    return 0;
}