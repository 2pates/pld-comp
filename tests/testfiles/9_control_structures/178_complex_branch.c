int main() {
    int a = 42;
    while (a > 40) {
        a = a - 1;
        if (a < 30)
            a = 12;
        else {
            while (a > 24)
                a = a - 1;
        }
        // C
        if (a < 40)
            a = 13;
        else
            a = 14;
        // B
    }
    // A
    return a;
}
