int main() {
    int a = 5;
    {
        int b = 12;
    }
    a = b;
    return a;
}