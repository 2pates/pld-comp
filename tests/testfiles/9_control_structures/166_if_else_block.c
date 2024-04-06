int main() {
    int a;
    a = 42;
    int b = 666;
    if (a == 42) {
        a = 12;
        b = b - 1;
    } else {
        a = 1;
        b = b + 1;
    }
    return a + b;
}
