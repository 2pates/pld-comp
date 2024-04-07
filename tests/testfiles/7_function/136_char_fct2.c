#include <stdio.h>
int g(char i) {
    int x = i-'a';
    return x;
}
int main() {
    return f('g');
}
