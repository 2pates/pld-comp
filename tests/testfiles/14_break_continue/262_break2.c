int main() {
    int i = 0;
    int a;
    while (i < 10) {
        i++;
        if (i == 5) {
            break;
        } else if (i == 7)
            i *= 2;
        else
            break;
        {
            --i;
            i *= 2;
        }
        a += i;
    }
    return a;
}
