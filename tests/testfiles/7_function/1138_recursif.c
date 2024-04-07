#include <stdio.h>
int fibo(i);
int main() {
    int i=10;
    return fibo(i);
}
int fibo(int i) {
    if(i<=1){
    	return i;
    }
    return fibo(i-1)*i;
}
