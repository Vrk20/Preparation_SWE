#include<iostream>
#include<vector>
using namespace std;

int longsubarray(vector<int>& arr,int target){
    int n = (int)arr.size();
    int left = 0;
    int sum =0;
    int best =0;
    for(int right =0;right<n;right++){
        sum+=arr[right];
        while(sum>target && left<=right){
            sum-=arr[left];
            ++left;
        }
        best = max(best,right -left+1);
    }
    return best;
}
int main(){
    vector<int> arr = {2,1,5,1,3,2};
    int tar = 7;
    int res = longsubarray(arr,tar);
    cout<<"longest subarray with <= 7 is "<<res;
    return 0;
}