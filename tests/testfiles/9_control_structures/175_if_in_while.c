int main() {
    int a = 42;
    while (a > 40) {
        a = a - 1;
        if (a < 30)
            a = 12;
        else
            a = a - 2;
    }
    return a;
}
