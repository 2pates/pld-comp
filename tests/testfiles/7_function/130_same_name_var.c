#include <stdio.h>
int f(int z);
int main() {
    int z;
    z = 2;
    f(z);
    return z;
}
int f(int z) {
    z = 3;
    return z;
}
