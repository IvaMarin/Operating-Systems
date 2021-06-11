#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // close()

// Function that reverses user string
void ReverseString(char *str) {
    int length = strlen(str);
    char *front = str;
    char *back = str + length - 1;

    while (front < back) {
        char tmp = *front;
        *front = *back;
        *back = tmp;
        ++front;
        --back;
    }
}

// Function to scan a string with unknown length
char* GetString() {
    int len = 0, capacity = 10;
    char* s = (char*)malloc(10 * sizeof(char));
    if (s == NULL) {
        perror("ERROR: unable to read string.");
        exit(6);
    }

    char c;
    do {
        c = getchar();
        if (c == EOF) {
            close(0);
            exit(0);
        }
        s[len++] = c;
        if (len == capacity) {
            capacity *= 2;
            s = (char*)realloc(s, capacity * sizeof(char));
            if (s == NULL) {
                perror("ERROR: unable to read string.");
                exit(6);
            }
        }
    } while  (c != '\0'); // Null-terminate the completed string
    s[len] = 0;
    return s;
}

int main(int argc, char* argv[]) {
    while (1) { // To stop input send EOF (for example using 'Ctrl + D' in *nix terminal)
        char* str = GetString();
        ReverseString(str);
        printf("%s\n", str);
        fflush(stdout); // makes the OS flush any buffers to the underlying file
    }
}