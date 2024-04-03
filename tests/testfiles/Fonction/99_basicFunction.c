#include <stdio.h>

int main() {
    int z = 5;
    z = f(z);
    return z;
}

int f(int z) { return z + 1; }
