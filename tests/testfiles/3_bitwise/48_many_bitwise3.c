int main() {
    int a = 31, b = 12, c = 7, d = ~b, e = 0, f = ~b | ~c, g = 27;
    return a | (~c | e) & b | ~(g | f & d);
}
