#include <stdio.h>
void g(char i) {
    i-=7;
    return i;
}
int main() {
    char ade = 'h';
    g(ade);
    int b = ade;
    return b;
}
