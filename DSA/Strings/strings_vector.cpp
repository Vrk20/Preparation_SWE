#include<iostream>
#include<vector>
#include<string>

void print_vec_str(std::vector<std::string>& s){
    for(auto it : s){
        std::cout<<it<<"\t";
    }
}
std::vector<std::string> splitwords(const std::string& s){
    std::vector<std::string>result;
    std::string word;
    for(char c : s){
        if(c!=' '){
            word.push_back(c);
        }
        else if(!word.empty()){
            result.push_back(word);
            word.clear();
        }
    }

    if(!word.empty())
        result.push_back(word);
    
    return result;
}

std::vector<std::string> u_splitwords(const std::string& s){
        std::vector<std::string> result;
        std::string word;
        for( char c:s){
            if(c!=' '){
                word.push_back(c);
            }
            else if(!word.empty()){
                result.push_back(word);
                word.clear();
            }
        }
        if(!word.empty()){
            result.push_back(word);
        }
    return result;
}

std::string longestword(const std::vector<std::string>& v){
    int max_size = 0;
    std::string ans;
    for(auto it:v){
        if(it.size()>max_size){
            ans = it;
        }
    }
    return  ans;
}

int main(){
    std::vector<std::string> v = {"area","house","money","billionare"};
    // print_vec_str(v);
    std::string s = "Im will be a billionare in 2026";
    std::vector<std::string> re = u_splitwords(s);
    std::cout<<re.size()<<"\n";
    print_vec_str(re);
    std::cout<<"\n";
    std::cout<<longestword(v);
    
}