#include<iostream>
#include<vector>

using namespace std;

void printarray(std::vector<int> &arr){
    for(int i = 0;i<arr.size();i++){
        std::cout<<arr[i]<<"\t";
    }
    std::cout<<"\n";
}

int main(){
    std::vector<int> arr = {0,0,1,1,1,2,2,3,3,4};
    int kval = 0;
    for(int i=0;i<arr.size();i++){
        if(arr[i]!= arr[i+1]){
            arr[kval]= arr[i];
            kval++;
        }
    }
    cout<<"kval is\t"<<kval<<endl;
    printarray(arr);

}


