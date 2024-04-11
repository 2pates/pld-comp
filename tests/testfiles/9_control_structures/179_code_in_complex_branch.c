int main() {
    int a = 42;
    int b = 50;
    while (b > 40) {
        a = a - 1;
        if (a < 1) {
            a = -a;
        } else {
            a = 43;
            while (a > 24) {
                a = a - 1;
            }
            a = -12;
        }
        a -= 13;
        // C
        if (a < 40) {
            a = 13;
        } else {
            a = 14;
        }
        a = 3;
        b = b - 1;
    }
    // A
    return a;
}
