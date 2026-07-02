#include<iostream>
#include<vector>

void move_zeros(std::vector<int>& nums){
    for(size_t i = 0;i<nums.size();i++){
        if(nums[i]!=0){
            continue;
        }
        for(size_t j = i+1;j<nums.size();j++){
            if(nums[j]!=0){
                std::swap(nums[i],nums[j]);
                break;
            }
        }
    }
}

void move_zeors_opt(std::vector<int>& nums){
    size_t j = 0;
    for(size_t i = 0; i<nums.size();i++){
        if(nums[i]!=0){
            std::swap(nums[j],nums[i]);
            j++;
        }
    }
}

int main(){
    std::vector<int> a = {0,1,0,3,12};
    // move_zeros(a);
    move_zeors_opt(a);
    for(auto ele:a){
        std::cout<<ele<<"\t";
    }
    std::cout<<std::endl;
    return 0;
}