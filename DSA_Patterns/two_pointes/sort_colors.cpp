#include<iostream>
#include<vector>

void sortColors(std::vector<int>& nums) {
        int low = 0;
        int high = nums.size()-1;
        int mid = 0;
        while(mid < high){
            int res = nums[mid];
            
            std::cout<<"nums[mid]\t"<<res<<std::endl;
            if(res ==0)
            {
                std::swap(nums[low],nums[mid]);
                mid++;
                low++;
                std::cout<<"\tinside if[0]\t"<<nums[mid]<<"\tlow\t"<<nums[low]<<"\tmid\t "<<nums[mid]<<"\thigh\t "<<nums[high]<<std::endl;

            }
            else if(res == 1)
            {
                mid++;
                std::cout<<"\tinside if[1]\t"<<nums[mid]<<"\tlow\t"<<nums[low]<<"\tmid\t "<<nums[mid]<<"\thigh\t "<<nums[high]<<std::endl;

            }
            else
            {
                std::swap(nums[high],nums[mid]);
                high--;
                mid++;
                std::cout<<"\tinside if[2]\t"<<nums[mid]<<"\tlow\t"<<nums[low]<<"\tmid\t "<<nums[mid]<<"\thigh\t "<<nums[high]<<std::endl;

            }
        }
}

int main()
{
    std::vector<int> flag ={2,0,2,1,1,0};
    sortColors(flag);
    for(auto it:flag){
        std::cout<<it<<"\t";
    }
    return 0;

}