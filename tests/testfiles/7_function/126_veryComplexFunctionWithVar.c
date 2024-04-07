#include <stdio.h>
int f();
int g(int b, int t);
int h(int e, int h, int v, int y, int w, int x);
int main() {
    int z;
    z = 2;
    int q = 7;
    // z = g(z++, q) + f();
    z = g(z, q) + f();
    int pop = 3;
    z = z + h(1, 3, 5, z, q, pop);
    return z + 1;
}
int f() { return 3; }
int g(int b, int t) {
    b = b + t;
    return b;
}
int h(int e, int h, int v, int y, int w, int x) {
    putchar(e);
    int res = e - h + v * y + w / x;
    return res;
}
