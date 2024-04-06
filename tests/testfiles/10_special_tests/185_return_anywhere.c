int main() {
    int a = 12;
    int b;
    {
        b = 13;
        return a;
    }
    return b;
}
