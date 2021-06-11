#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "implementation.h"

// Сounting 'e' number
double e(int x) {
    printf("Counting 'e' number using second remarkable limit\n");
    return pow(1 + 1. / x, x);
}

// Decimal to Binary Conversion
char* translation(long x) {
    char *res = calloc(1, sizeof(char));
    res[0] = '\0';
    if(x == 0) {
        char *tmp = calloc(strlen(res) + 2, sizeof(char));
        strcpy(tmp+1, res);
        free(res);
        res = tmp;
        res[0] = '0';
    }
    while(x > 0) {
        char ch = (x & 1u) + '0';
        x >>= 1u;
        char *tmp = calloc(strlen(res) + 2, sizeof(char));
        strcpy(tmp+1, res);
        free(res);
        res = tmp;
        res[0] = ch;
    }
    return res;
}