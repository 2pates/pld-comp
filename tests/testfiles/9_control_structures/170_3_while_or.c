int main() {
    int a = 42;
    int b = 50;
    while (a < 50 || b < 60) {
        a = a + 1;
        b = b + 1;
    }
    return a + b;
}