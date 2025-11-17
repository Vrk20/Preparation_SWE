/*
    The below is the basics of the arrays in cpp
    later array operations will be added 
    then we will look about vectors once this is clear
*/
#include<iostream>

// added grow function to have understanding of vector internal working
int* grow(int* oldarr,int oldsize,int newsize){
    int* newarr = new int[newsize];
    for(int i=0;i<oldsize;i++){
        newarr[i] = oldarr[i];
    }
    delete oldarr[];
    return newarr;
}

// added sample push_back function from vector 
void push_back(int*& arr,int& size,int& capacity,int x){
    if(size == capacity){
        arr = grow(arr,capacity,capacity*2);
        capacity *=2;
    }
    arr[size] = x;
    size++;
}
void printarray(int* arr, int size) // passing array as a pointer int* or int arr[] in this case size needs to explicitly passed
{  
    for(int i=0;i<size;i++){
        std::cout<<arr[i]<<"\t";
    }
    std::cout<<"\n";
}
int main(){
    int a[5] = {1,2,3,4,5}; // basic array implementation
    printarray(a,5);
    int a2[3];  // array creation if no elements are assigned 
    printarray(a2,3);  //Uninitialized arrays contain garbage values (whatever was in memory previously)
    int size_arr;
    std::cout<<"Enter the size of the array\n";  
    std::cin>>size_arr;
    int arr[size_arr]; // NOTE: Variable Length Arrays (VLAs) are compiler extensions, not standard C++
    std::cout<<"Enter "<<size_arr<<" elements:\n";
    for (int i = 0; i < size_arr; i++) // need to cover modern for techniques
    {
        std::cin>>arr[i];
    }
    printarray(arr,size_arr);
    // to know the memory allocated for an array in bytes
    std::cout<<"size of one element of an array is\t"<<sizeof(arr[0])<<"\n";//size of an individual elements
    std::cout<<"memory of total array is\t"<<sizeof(arr)<<"\n"; //size of an entire array
    return 0;
}