#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> // open() and O_XXX flags
#include <unistd.h> // close(), dup2(), execl()
#include <sys/stat.h> // S_IXXX flags 

// Function that writes string with its length to pipe file descriptor(fd) 
void WriteToPipe(int* fd, char* str) {
    int i = 0;
    char ch;
    do {
        ch = str[i++];
        if (write(fd[1], &ch, sizeof(char)) < 0) {
            perror("ERROR: failed to write to the pipe");
            exit(4);
        }
    } while (ch != '\0');
}

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

int main() {

    // Creating files for child processes
    printf("Enter file's name for child process 1 (gets odd lines): ");
    char* file1_name = GetString();

    printf("Enter file's name for child process 2 (gets even lines): ");
    char* file2_name = GetString();

    // Flags for open(): 
    //O_WRONLY - write only, O_CREAT - create file if it doesn't exist, 
    //S_IWRITE, S_IREAD - give user rights to write and read accordingly, 
    int file1 = open(file1_name, O_WRONLY | O_CREAT, S_IWRITE | S_IREAD); // System call to open file
    int file2 = open(file2_name, O_WRONLY | O_CREAT, S_IWRITE | S_IREAD);
    if (file1 < 0 || file2 < 0) {
        perror("ERROR: unable to open file.");
        exit(1);
    }

    // Creating pipes for child processes
    int fd1[2];
    int fd2[2];
    // fd[0] - read, fd[1] - write, fd[2] - error
    if (pipe(fd1) < 0 || pipe(fd2) < 0) {
        perror("ERROR: unable to create pipe."); // pipe() returns '-1'
        exit(2);
    }

    // Creating child processes
    int ID_1 = fork(); // System call for creating child processes
    if (ID_1 < 0) { // fork() returns negative value if error occured
        perror("ERROR: unable to create child process.");
        exit(3);
    }

    if (ID_1 > 0) { // fork() returns default value for parent process
        int ID_2 = fork();
        if (ID_2 < 0) {
            perror("ERROR: unable to create child process");
            exit(3);
        }

        if (ID_2 > 0) { // Does everything parent process related
            // Closes a file descriptor, so that it no longer refers to any file and may be reused
            close(fd1[0]); 
            close(fd2[0]);
            int counter = 1; // Counter to differ odd and even lines
            
            while (1) { // To stop input send EOF (for example using 'Ctrl + D' in *nix terminal)
                char* s = GetString();
                
                if (counter % 2 == 0) { // Writes to pipe1 odd lines
                    counter += 1;
                    WriteToPipe(fd2, s);
                    if (s[0] == EOF) {
                        WriteToPipe(fd2, s);
                        break;
                    }
                }
                else { // Writes to pipe2 even lines
                    counter += 1;
                    WriteToPipe(fd1, s);
                    if (s[0] == EOF) {
                        WriteToPipe(fd1, s);
                        break;
                    }
                }
            }

            close(fd1[1]);
            close(fd2[1]);
        }
        else { // Does everything child2 process related
            // Closes a file descriptor, so that it no longer refers to any file and may be reused
            close(fd1[0]);
            close(fd1[1]);
            close(fd2[1]);

            // dup2 () redirects standart input and output for child processes
            if (dup2(fd2[0], STDIN_FILENO) < 0) {
                perror("ERROR: unable to redirect stdin for child process.");
                exit(5);
            }
            if (dup2(file2, STDOUT_FILENO) < 0) {
                perror("ERROR: unable to redirect stdout for child process.");
                exit(5);
            }
            execlp("/mnt/c/Users/Иван/projects/os_labs/os lab2/child", "child", NULL); // System call to execute  file 'child'

            // It won't go here if 'child' executes
            perror("ERROR: unable to execute child process.");
            exit(6);
        }
    }
    else { // Does everything child1 process related
        // Closes a file descriptor, so that it no longer refers to any file and may be reused
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);

        // dup2 () redirects standart input and output for child processes
        if (dup2(fd1[0], STDIN_FILENO) < 0) {
            perror("ERROR: unable to redirect stdin for child process.");
            exit(5);
        }
        if (dup2(file1, STDOUT_FILENO) < 0) {
            perror("ERROR: unable to redirect stdout for child process.");
            exit(5);
        }
        execlp("/mnt/c/Users/Иван/projects/os_labs/os lab2/child", "child", NULL); // System call to execute  file 'child'

        // It won't go here if 'child' executes
        perror("ERROR: unable to execute child process.");
        exit(6);
    }
}