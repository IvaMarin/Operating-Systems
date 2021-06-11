#include "implementation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Counting 'e' number
double e(int x) {
    printf("Counting 'e' number using Taylor series\n");
    double ans = 0;
    long long factorial = 1;
    for (int n = 0; n <= x; ++n) {
        if (n != 0) {
            factorial *= n;
        }
        ans += 1. / factorial;
    }
    return ans;
}

// Decimal to Ternary Conversion
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
        char ch = (x % 3) + '0';
        x /= 3;
        char *tmp = calloc(strlen(res) + 2, sizeof(char));
        strcpy(tmp+1, res);
        free(res);
        res = tmp;
        res[0] = ch;
    }
    return res;
}