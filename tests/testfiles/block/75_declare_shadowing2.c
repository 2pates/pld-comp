int main() {
    int a = 6;
    int b;
    {
        int a = 5;
        b = a;
    }
    b = b & a;
    return b;
}
