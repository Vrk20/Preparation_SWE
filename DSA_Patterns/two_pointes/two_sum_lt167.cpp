#include<iostream>
#include<vector>

void printarray(std::vector<int> &arr){
    for(int i = 0;i<arr.size();i++){
        std::cout<<arr[i]<<"\t";
    }
    std::cout<<"\n";
}

std::vector<int> twoSum(std::vector<int>& numbers, int target){
        int left = 0;
    std::vector<int>res;
    int right = numbers.size()-1;
    while(left<right){
        int sum = numbers[left]+numbers[right];
        if(sum == target){
            res.push_back(left+1);
            res.push_back(right+1);
            return res;
        }
        else if(sum<target){
            left++;
        }
        else{
            right--;
        }
    }
    return res;
}
int main(){
    std::vector<int> arr = {1,2,3,4,5,6,7,8,9};
    return res;
}