/* 
Idea behind:
b ^ a = (b^(a/2))^2
7^4 = 7^2 * 7^2, instead of 2*2*2*2
3^8 = 3^4 * 3^4, instead of 2*... 8 times
5^11 = 5^5 * 5^5 * 5, instead of 5*... 11 times 
*/

/* 
3^11
3^11 = (3^5)^2 * 3 
3^5 = (3^2)^2 * 3
3^2 = (3^1)^2
3^1 = (3^0)^2 * 3 
*/
#include<bits/stdc++.h>
using namespace std;
int power1(int a, int b){
    if(b == 0) return 1;
    int tmp = power1(a, b / 2);
    int result = tmp * tmp;
    if(b % 2 == 1) result *= a;
    return result;
}
/*
3^11, where 11 = 1011
initial values:
r = 1, a = 3, b = 11
1) r = 3, a = 3^2, b = 5
2) r = 3^3, a = 3^4, b = 2
3) r = 3^3, a = 3^8, b = 1
4) r = 3^11, a = 3^16, b = 0
*/
int power2(int a, int b){
    int result = 1;
    while(b > 0){
        if(b % 2 == 1) result *= a;
        a *= a;
        b /= 2;
    }
    return result;
}

int main(){
    cout << power1(2,5) << endl;
    cout << power2(2,5) << endl;
    return 0;
}
