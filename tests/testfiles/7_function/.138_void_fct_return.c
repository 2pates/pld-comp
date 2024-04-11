#include <stdio.h>
/*Un test pour une feature non implémenté*/
void g(char i);

int main() {
    char ade = 'h';
    g(ade);
    int b = ade;
    return b;
}

void g(char i) {
    i -= 7;
    return i;
}
