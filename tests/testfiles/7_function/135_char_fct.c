#include <stdio.h>
char f(int i);
int main() {
    char a = f(12);
    int b = a;
    return b;
}
char f(int i) { return 'a'+i; }
