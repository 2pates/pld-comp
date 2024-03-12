#include <stdio.h>

void putdigit(int digit)
{
        char c=digit+'0';
        putchar(c);
}
    
int main(void)
{
    int x;
    for(x=0; x<10; x++)
    {
        putdigit(x);
    }
    putchar('\n');
    
    return 42;
}
