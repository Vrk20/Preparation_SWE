#include<iostream>
#include<string>

bool validPalindrome(std::string s){
    int left = 0;
    int right = s.size()-1;
    int count = 0;
    while (left<right)
    {
        if(s[left]!=s[right]){
            count++;
            right++;
        }
        if(count>1){
            return false;
        }
        left++;
        right--;
    }
    return true;
}


 bool isPalindrome(std::string s) {
        std::string result="";
        for (unsigned char c : s) {
        if (std::isalnum(c)) {
            result += std::tolower(c);
        }
    }
            std::cout<<"result is\t"<<result<<std::endl;

        size_t left = 0;
        size_t right = result.size()-1;

        while(left<right){
            if(result[left]!=result[right])
                return false;
            left++;
            right--;
        }
        return true;
    }
int main(){
    std::string s = "cbcc";
    std::string a = "A man, a plan, a canal: Panama";
    // std::cout<<"for abc";
    std::cout<<isPalindrome(a);
    // std::cout<<validPalindrome(a)<<std::endl;
    // std::cout<<validPalindrome(s);
    return 0;
}