#include<iostream>
#include<vector>
#include<string>
int removeDuplicates(std::vector<int>& nums){
    if(nums.empty())
        return 0;
    std::string s = "Je;;sa32";
    size_t write = 1;
    for(size_t read = 1; read<nums.size();read++){
        if(nums[read]!=nums[read-1]){
            nums[write] = nums[read];
            write++;
       }
    }
    return write;
   
}
int main(){
    std::vector<int> n = {0,0,1,1,1,2,2,3,3,4};
    std::cout<<removeDuplicates(n);
    std::cout<<std::endl;
    for (auto it:n){
        std::cout<<it<<"\t";
    }
    std::cout<<std::endl;
    return 0;
}