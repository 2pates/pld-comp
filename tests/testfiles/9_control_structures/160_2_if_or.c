int main() {
    int a = 42;
    if (a || 0)
        a = 12;
    return a;
}
