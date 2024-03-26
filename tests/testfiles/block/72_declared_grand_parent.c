int main() {
    int a;
    {
        a = 6;
        {
            a = 5;
            int c = 12;
        }
    }
    return a;
}
