#include <stdio.h>
int isPrime(int num);
void computePrimes(int m);
void printNumber(int number);
// simple programm for computing the n first prime numbers
int main() {
    int n = 10;
    computePrimes(n);
    return 0;
}

int isPrime(int num) {
    if (num <= 1) {
        return 0;
    }
    int i = 2;
    while (i * i <= num) {
        if (num % i == 0) {
            return 0;
        }
        i = i + 1;
    }

    return 1;
}

void computePrimes(int m) {
    int count = 0;
    int num = 2;

    while (count < m) {
        if (isPrime(num)) {
            printNumber(num);
            putchar(10);
            count = count + 1;
        }
        num = num + 1;
    }
}

void printNumber(int x) {
    if (x < 0) {
        putchar('-');
        x = -x;
    }
    if (x / 10 != 0)
        printNumber(x / 10);
    putchar(x % 10 + '0');
}