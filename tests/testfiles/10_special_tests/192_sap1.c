#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


int main(){
    int hauteur = 15, tronc = 4, i, largeur = 6;

    i=0;
    while(i < 2) {
        int etage = 8, place = 7


        for(int i = 0; i<hauteur; i++){
            for(int j = 0; j<(hauteur-1-i); j++) putchar(' ');
            for(int j = 0; j<2*i+1; j++) {
                if ( i == hauteur-etage && j == place-1 ) {
                    // printf("\033[0;31m0");
                    putchar('*');
                }
                else {
                    // printf("\033[0;32m*");
                    putchar('o');
                }
            }
            // printf("\r\n");
            putchar(10); // newline
        }

        for(int i = 0; i<tronc ; i++){
            for(int j =0; j<(hauteur-largeur/2-1); j++) putchar(' ');
            for(int j = 0; j<largeur; j++)
                // printf("\033[0;34m|");
                putchar('U');
            // printf("\r\n");
            putchar(10); // newline
        }
        // // sleep
        // usleep(200000); // 200ms
        // // clear
        // system("clear");
        putchar(10); // newline
        putchar(10); // newline
        putchar(10); // newline

        i++;
        if (i == 2) i=0;

        i%=2;
    }

    return 0;
}

