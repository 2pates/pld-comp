#include <stdio.h>

int g(char i);

int main() { return g('g'); }

int g(char i) {
    int x = i - 'a';
    return x;
}
