int main() {
    int a;

    {
        a = 5;
        int d = b;
        int c = 12;
    }

    { d = 5; }
    return a;
}
