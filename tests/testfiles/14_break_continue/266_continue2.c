int main() { 
    int i = 0;
    int a = 0;
    while (i < 10) {
        i=i+1;
        {
            i+=1;
            continue;
            ++i;
        }
        i++;
        {
            i=i;
        }
        a++;
    }
    return a;
}
