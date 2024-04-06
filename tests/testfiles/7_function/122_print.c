#include <stdio.h>
int f(int q);
int main() {
    int z;
    z = 17957;
    z = f(z);
    return z;
}
int f(int q) {
    putchar(q % 100 + 32);
    q = q / 100;
    putchar(q % 100 + 32);
    q = q / 100;
    putchar(q % 100 + 32);
    return 3;
}
