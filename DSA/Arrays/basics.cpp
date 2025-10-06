/*
    The below is the basics of the arrays in cpp
    later array operations will be added 
    then we will look about vectors once this is clear
*/
#include<iostream>

void printarray(int* arr, int size){
    for(int i=0;i<size;i++){
        std::cout<<arr[i]<<"\t";
    }
    std::cout<<"\n";
}
int main(){
    int a[5] = {1,2,3,4,5}; // basic array implementation
    printarray(a,5);
    int size_arr;
    std::cout<<"Enter the size of the array\n";  
    std::cin>>size_arr;
    int arr[size_arr];
    for (int i = 0; i < size_arr; i++) // need to cover modern for techniques
    {
        std::cin>>arr[i];
    }
    printarray(arr,size_arr);
    
    return 0;
}