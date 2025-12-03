#include<iostream>
#include<vector>
using namespace std;

void sumpair(vector<int>& arr,int target){
    int left = 0;
    int right = arr.size() -1;
    while (left<right)
    {
        int sum = arr[left] + arr[right];

        if(sum == target){
            cout<<"Found pair : "<<arr[left]<<" & "<<arr[right]<< "\n";
            return;
        }
        else if(sum<target){
            left++;
        }
        else{
            right--;
        }
    }
    cout<<"No pair found\n";
    
}

int main(){
    vector<int> arr = {1,2,4,7,11,15};
    int target = 15;
    sumpair(arr,target);
    return 0;
}