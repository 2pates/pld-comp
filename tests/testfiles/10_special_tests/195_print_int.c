#include <stdio.h>

void print_int(int x);

int main() {
    print_int(-273);
    putchar(10);// newline
}

void print_int(int x) {
    if(x<0) {
        putchar('-');
        x = -x;
    }
    if(x/10 != 0)
        print_int(x/10);
    putchar(x%10 + '0');
}
