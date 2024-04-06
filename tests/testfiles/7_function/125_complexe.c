int main() {
    int z;
    z = 2;
    int q = 7;
    z = g(z, q) + f();
    return z;
}
int f() { return 3; }
int g(int b, int t) {
    b = b + t;
    return b;
}
