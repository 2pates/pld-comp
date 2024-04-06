int main() { 
    int i = 0;
    int b = 1;
    while (i < 10) {
        i++;
        b+=1;
        if (i == 5) {
            b+=30;
            continue;
        }
    }

    return b;
}
