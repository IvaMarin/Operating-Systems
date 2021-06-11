#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strlen(), strcmp(), strcpy()
#include <fcntl.h> // open() and O_XXX flags
#include <sys/mman.h> // mmap(), munmap()
#include <sys/stat.h> // S_IWRITE, S_IREAD flags

#define MAP_SIZE 4096

// Files for mapping
char* file1_name = "file1_mapped";
char* file2_name = "file2_mapped";

// Empty string as a signal
char isEmpty = 1;
char* empty_string = &isEmpty;

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

int main(int argc, char* argv[]) {
    char* file_name;
    if (argv[0][0] == '1') {
        file_name = file1_name;
    }
    else if (argv[0][0] == '2') {
        file_name = file2_name;
    }
    else {
        perror("Unknown file");
        exit(8);
    }
    // Opening a file for mapping
    int fd = open(file_name, O_RDWR | O_CREAT, S_IWRITE | S_IREAD);
    if (fd < 0) {
        perror("ERROR: unable to open file");
        exit(1);
    }

    // Mapping file
    /*MAP_SHARED - Share this mapping.  Updates to the mapping are visible to
                   other processes mapping the same region, and (in the case of
                   file-backed mappings) are carried through to the underlying
                   file.*/
    char* file = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // maps file into memory
    if (file == MAP_FAILED) {
        perror("ERROR: unable to map a file");
        exit(2);
    }

    while (1) { // To stop input send EOF (for example using 'Ctrl + D' in *nix terminal)
        // Wainting for a string
        while (strcmp(file, empty_string) == 0) {}
        
        // Terminating if 'Ctrl + D' was pressed
        if (file[0] == EOF) {
            if (munmap(file, MAP_SIZE) < 0) { // unmaps file from memory
                perror("ERROR: unable to unmap file");
                exit(4);
            }
            exit(0);
        }
        
        char* string = (char*)malloc(strlen(file) * sizeof(char)); // allocates memory for string
        strcpy(string, file);
        ReverseString(string);
        printf("%s\n", string);
        fflush(stdout); // Makes the OS flush any buffers to the underlying file
        strcpy(file, empty_string);
        free(string); // deallocates memory for string
    }
}