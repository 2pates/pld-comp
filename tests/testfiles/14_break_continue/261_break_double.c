int main() {
    int i = 0;
    int a = 30;
    while (i < 10) {
        i++;
        a = 30;
        while (a < 40) {
            a++;
            if (a == 35) {
                break;
            }
        }
    }

    return 0;
}
