#include <stdio.h>

int alphabet(int n);

int main() {
    int c;
    c = alphabet(15);
    return c;
}

int alphabet(int n) {
    char a;
    a='A';
    while (a<'A'+n) {
        putchar(a);
        a=a+1;
    }
    return a;
}
