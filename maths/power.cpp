#include<bits/stdc++.h>
using namespace std;
int power1(int a, int b){
    if(b == 0) return 1;
    int tmp = power1(a, b / 2);
    int result = tmp * tmp;
    if(b % 2 == 1) result *= a;
    return result;
}

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
