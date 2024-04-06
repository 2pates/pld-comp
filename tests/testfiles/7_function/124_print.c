#include <stdio.h>
int main() {
    int z;
    z=585453;
    z=f(z);
    return z;
}
int f(int q){
    putchar(q%100);
    q=q/100;
    putchar(q%100);
    q=q/100;
    putchar(q%100);
    return 3;
}
