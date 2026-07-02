#include<iostream>
#include<vector>
#include<algorithm>

double findMaxAverage(std::vector<int>& arr,int k){
double maxsum = 0;
    for(size_t i = 0; i<arr.size()-k+1;i++){
        double sum  = 0;
        for(size_t j = i;j<i+k;j++){
            std::cout<<arr[j]<<"\t";
            sum+=arr[j];
        }
        std::cout<<std::endl;
        std::cout<<sum<<std::endl;
        double avg = sum/k;
        maxsum = avg>maxsum ? avg:maxsum;
    }
    return maxsum;
    // std::cout<<maxsum<<std::endl;
}
double findmaxavg(std::vector<int>& arr,int k){
    
    int low = 0;
    int high = arr.size();
    while(low<high){
        while(low<k)
    }
}
int main(){
    std::vector<int> arr = {1,12,-5,-6,50,3};
    std::vector<int> arr2 = {5};//1,12,-5,-6,50,3};
    int k = 4;
    int k2 = 1;
    std::cout<<findMaxAverage(arr,k);
    
    return 0;
}