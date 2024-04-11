int f();
int g(int b, int t);
int main() {
    int z;
    z = 2;
    int q = 7;
    z = g(z * f(), g(q, f()) * q) / 1 + f() % 2;
    return z;
}
int f() { return 3; }
int g(int b, int t) {
    b = b + t * f();
    return b;
}
