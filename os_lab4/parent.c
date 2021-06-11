#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strlen(), strcmp(), strcpy()
#include <fcntl.h> // open() and O_XXX flags
#include <unistd.h> // close(), dup2(), execl()
#include <sys/stat.h> // S_IXXX flags
#include <sys/mman.h> // mmap(), munmap()

#define MAP_SIZE 4096 //in bytes

// Files for mapping
char* file1_name = "file1_mapped";
char* file2_name = "file2_mapped";

// Empty string as a signal
char isEmpty = 1;
char* empty_string = &isEmpty;

// Function to scan a string with unknown length
char* GetString() {
    int length = 0, capacity = 10;
    char* s = (char*)malloc(10 * sizeof(char));
    if (s == NULL) {
        perror("ERROR: unable to read string.");
        exit(6);
    }

    char ch;
    while ((ch = getchar()) != '\n') {
        s[length++] = ch;
        if (ch == EOF) {
            break;
        }
        if (length == capacity) {
            capacity *= 2;
            s = (char*)realloc(s, capacity * sizeof(char));
            if (s == NULL) {
                perror("ERROR: unable to read string.");
                exit(6);
            }
        }
    };
    s[length] = '\0'; // Null-terminate the completed string
    return s;
}

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

int main() {

    // Creating files for output of child processes
    printf("Enter file's name for child process 1 (gets odd lines): ");
    char* output_file1_name = GetString();

    printf("Enter file's name for child process 2 (gets even lines): ");
    char* output_file2_name = GetString();

    // Flags for open(): 
    //O_WRONLY - write only, O_CREAT - create file if it doesn't exist, 
    //S_IWRITE, S_IREAD - give user rights to write and read accordingly, 
    int output_file1 = open(output_file1_name, O_WRONLY | O_CREAT, S_IWRITE | S_IREAD); // System call to open file
    int output_file2 = open(output_file2_name, O_WRONLY | O_CREAT, S_IWRITE | S_IREAD);
    if (output_file1 < 0 || output_file2 < 0) {
        perror("ERROR: unable to open file.");
        exit(1);
    }

    // Сreating files for mapping
    int fd1 = open(file1_name, O_RDWR | O_CREAT, S_IWRITE | S_IREAD);
    int fd2 = open(file2_name, O_RDWR | O_CREAT, S_IWRITE | S_IREAD);
    if (fd1 < 0 || fd2 < 0) {
        perror("ERROR: unable to open file."); // open() returns '-1'
        exit(1);
    }

    // Empty files can't be mapped, so we'll put our empty_stirng there
    if (write(fd1, empty_string, sizeof(empty_string)) < 0) {
        perror("ERROR: unable to write to file");
        exit(1);
    }
    if (write(fd2, empty_string, sizeof(empty_string)) < 0) {
        perror("ERROR: unable to write to file");
        exit(1);
    }

    // Mapping files
    char* file1 = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0); // maps file into memory
    char* file2 = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
    if (file1 == MAP_FAILED || file2 == MAP_FAILED) {
        perror("ERROR: unable to map a file");
        exit(2);
    }

    // Creating child processes
    pid_t ID_1 = fork(); // System call for creating child processes
    if (ID_1 < 0) { // fork() returns negative value if error occured
        perror("ERROR: unable to create child process.");
        exit(3);
    }

    if (ID_1 > 0) { // fork() returns default value for parent process
        pid_t ID_2 = fork();
        if (ID_2 < 0) {
            perror("ERROR: unable to create child process");
            exit(3);
        }

        if (ID_2 > 0) { // Does everything parent process related

            int counter = 1; // Counter to differ odd and even lines
            
            while (1) { // To stop input send EOF (for example using 'Ctrl + D' in *nix terminal)
                char* s = GetString();
                
                if (counter % 2 == 0) { // Writes to file1 odd lines
                    counter += 1;
                    strcpy(file2, s);
                    if (s[0] == EOF) {
                        strcpy(file2, s);
                        break;
                    }
                }
                else { // Writes to file2 even lines
                    counter += 1;
                    strcpy(file1, s);
                    if (s[0] == EOF) {
                        strcpy(file1, s);
                        break;
                    }
                }
            }

            // Unmaps files from memory, closes  and removes them. Also checks for errors
            if (munmap(file1, MAP_SIZE) < 0 || munmap(file2, MAP_SIZE) < 0) {
                perror("ERROR: unable to unmap files");
                exit(4);
            }
            if (close(fd1) < 0 || close(fd2) < 0) {
                perror("ERROR: unable to close files");
                exit(5);
            }
            if (remove(file1_name) < 0 || remove(file2_name) < 0) {
                perror("ERROR: unable to delete files");
                exit(6);
            }
        }
        else { // Does everything child2 process related
            // dup2 () redirects standart input and output for child processes
            if (dup2(output_file2, STDOUT_FILENO) < 0) {
                perror("ERROR: unable to redirect stdout for child process.");
                exit(7);
            }

            char* myargs [] = {"2", NULL};
            execv("child", myargs); // System call to execute  file 'child'

            // It won't go here if 'child' executes
            perror("ERROR: unable to execute child process.");
            exit(8);
        }
    }
    else { // Does everything child1 process related
        // dup2 () redirects standart input and output for child processes
        if (dup2(output_file1, STDOUT_FILENO) < 0) {
            perror("ERROR: unable to redirect stdout for child process.");
            exit(7);
        }
   
        char* myargs [] = {"1", NULL};
        execv("child", myargs); // System call to execute  file 'child'

        // It won't go here if 'child' executes
        perror("ERROR: unable to execute child process.");
        exit(8);
    }
}