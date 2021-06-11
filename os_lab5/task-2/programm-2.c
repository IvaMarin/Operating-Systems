#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>


typedef enum {
    FIRST,
    SECOND,
} Context;

// default context for implementation
Context r = FIRST;

const char* lib_name_1 = "libimp.so";
const char* lib_name_2 = "libimp_2.so";

void *handler = NULL;

void loadDLibs(Context implementation){
    const char *name;
    if(implementation == FIRST){
        name = lib_name_1;
    } else{
        name = lib_name_2;
    }
    // loading library
    // RTLD_LAZY - perform lazy binding
    handler = dlopen(name, RTLD_LAZY);
    if(!handler){
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
}

// resolving functions from library
double (*e)(int) = NULL;
char* (*translation)(long x) = NULL;
char *err;


void loadContext(){
    loadDLibs(r);
    // dslym() takes a "handle" of a dynamic library returned by dlopen()
    // and the null-terminated symbol name, returning the address where that symbol is loaded into memory
    e = dlsym(handler, "e");
    translation = dlsym(handler, "translation");
    if((err = dlerror())) {
        fprintf(stderr, "%s\n", err);
        exit(EXIT_FAILURE);
    }
}

void changeContext(){
    dlclose(handler);
    if(r == FIRST){
        r = SECOND;
    } else {
        r = FIRST;
    }

    loadContext();
}

void Menu() {
    puts("+-------------+-------------------------------------------+");
    puts("|   command   |               description                 |");
    puts("|=============+===========================================|");
    puts("|      0      |           switch implementation           |");
    puts("|=========================================================|");
    puts("|              FISRT IMPLEMENTATION(default)              |");
    puts("|=========================================================|");
    puts("|     1  x    |               count 'e' number            |");
    puts("|             |        using second remarkable limit      |");
    puts("|-------------+-------------------------------------------|");
    puts("|    2 num    | translate number to binary number system  |");
    puts("|             |        from decimal number system         |");
    puts("|=========================================================|");
    puts("|                  SECOND IMPLEMENTATION                  |");
    puts("|=========================================================|");
    puts("|     1  x    |              count 'e' number             |");
    puts("|             |           using Taylor series             |");
    puts("|-------------+-------------------------------------------|");
    puts("|    2 num    | translate number to ternary number system |");
    puts("|             |        from decimal number system         |");
    puts("+-------------+-------------------------------------------+\n");
}

int main(){
    Menu();
    r = FIRST;
    loadContext();

    int cmd = 0;

    while (scanf("%d", &cmd) != EOF){

        if(cmd == 0){
            changeContext();
            puts("Changing context...");
            sleep(1);
            if(r == FIRST){
                puts("Now context is first");
            } else{
                puts("Now context is second");
            }
            continue;
        }
        if(cmd == 1) {
            int x;
            if (scanf("%d", &x) == EOF) {
                break;
            }
            printf("%f\n", e(x));
        }
        else if (cmd == 2) {
            long x;
            if(scanf("%ld", &x)== EOF){
                break;
            }

            char *string;
            printf("Translate number %ld to ", x);
            if(r == FIRST) {
                printf("binary ");
            }else{
                printf("ternary ");
            }
            puts("number system");
            string = translation(x);
            printf("%s\n", string);
            free(string);
        }
        else {
            puts("Incorrect command");
        }
    }
    dlclose(handler);
}