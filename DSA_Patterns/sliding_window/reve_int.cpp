#include<iostream>

int reverse(int x) {
    if ((x < (long long)INT_MIN) || (x > (long long)INT_MAX)) {return 0;}
    int a = 0;
    int res = x;
    int y =0;
    if(x < 0){res*=-1;}
    while(res>0){
        y = res%10;
        a = (a+y)*10;
        res = res/10;
    }
    int ans = a/10;
    return (x<0)? ans*-1:ans;
}
int main(){
    std::cout<<reverse(1534236469);
    return 0;
}