int main() {
    int a, b, c, d, e;
    a = 11;
    b = -8;
    c = 13;
    d = ~b;
    e = 0;
    return a | ((b & c) & (d | ~e));
}
