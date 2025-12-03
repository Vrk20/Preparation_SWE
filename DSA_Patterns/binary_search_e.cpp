#include<iostream>
#include<vector>
using namespace std;

int binsearch(vector<int>& arr, int element){
    int low = 0;
    int n = arr.size();
    int high = n-1;
    while (low<=high)
    {
        int mid = low + (high-low)/2;
        if(arr[mid] == element){
            return mid;
        }
        else if(arr[mid] > element){
            high = mid-1;
        }
        else{
            low = mid+1;
        }
    }
    return -1;
}
int main(){
    vector<int> arr = {2,4,6,8,10,12,14};
    int ele = 12;
    int res = binsearch(arr,ele);
    if(res!=-1){
        cout<<"Element found at index "<<res+1<<endl;
    }
    else{
        cout<<"Element not found\n";
    }
    return 0;
}
