#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../lib/implementation.h"

void Menu() {
    puts("+-------------+------------------------------------------+");
    puts("|   command   |               description                |");
    puts("|=============+==========================================|");
    puts("|    1  x     |              count 'e' number            |");
    puts("|-------------+------------------------------------------|");
    puts("|    2  x     | translate number to binary number system |");
    puts("|             |        from decimal number system        |");
    puts("+-------------+------------------------------------------+\n");
}

int main() {
    Menu();
    int cmd;
    while (scanf("%d", &cmd) > 0) {
        if (cmd == 1) {
            int x;
            scanf("%d", &x);
            printf("%f\n", e(x));
        }
        else if (cmd == 2) {
            long x;
            if(scanf("%ld", &x)== EOF){
                break;
            }
            char* returned_str = translation(x);
            if (returned_str == NULL) {
                perror("Memory allocation error.");
            }
            puts("Decimal to binary conversion");
            printf("%s\n", returned_str);
            free(returned_str);
        }
        else {
            puts("Incorrect command");
        }
    }
}