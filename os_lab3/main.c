#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct ThreadArg {
    char *str;
    char *pattern;
    int strLen;
    int patternLen;
    int startIndex;
    int endIndex;
    int res;
} ThreadArg;

// Thread function - Naive algorithm for Pattern Searching
void* Search(void* argument) {
    ThreadArg* arg = (ThreadArg*) argument;
    char *pattern = arg->pattern;
    char *str = arg->str;
    int found = 1;

    if(arg->strLen < arg->patternLen) {
        printf("ERROR: pattern is larger than string.\n");
        exit(4);
    }

    // A loop to slide patttern[] one by one 
    for (int i = arg->startIndex; i < arg->endIndex; ++i) {
        for (int j = 0; j < arg->patternLen; ++j) {
            if(i + j - arg->startIndex >= arg->strLen) {
                found = 0;
                break;
            }
            if (str[i+j] != pattern[j]) { //One character doesn't match
                found = 0;
                break;
            }
        }
        if(found) { // if pattern[0...(arg->patternLen)-1] = text[i, i+1, ...i+(arg->patternLen)-1]
            arg->res = i;
            pthread_exit(NULL);
        }
        else {
            found = 1;
        }
    }
    arg->res = -1;
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    struct timespec start, finish;

    if (argc != 2) {
        printf("Syntax: ./main Number_of_threads\n");
        exit(1);
    }

    // Threads initialization
    int threadNumber = atoi(argv[1]);
    
    printf("Thread number: %d\n", threadNumber);

    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * threadNumber);
    if (threads == NULL) {
        printf("ERROR: unable to allocate space for threads.\n");
        exit(3);
    }

    // Iput text and pattern initialization
    int input_string_len, input_pattern_len;

    printf("Enter string length: ");
    scanf(" %d", &input_string_len);
    
    printf("Enter pattern length: ");
    scanf(" %d", &input_pattern_len);

    char *text = malloc(sizeof(char) * (input_string_len+1));
    if (text == NULL) {
        printf("ERROR: unable to create an array for text.\n");
        exit(3);
    }

    char *pattern = malloc(sizeof(char) * (input_pattern_len+1));
    if (pattern == NULL) {
        printf("ERROR: unable to create an array for pattern.\n");
        exit(3);
    }
    
    printf("Enter string: ");
    scanf(" %s", text);
    
    printf("Enter pattern: ");
    scanf(" %s", pattern);

    // Array for arguments which will be passed into thread function
    ThreadArg* args = (ThreadArg*)malloc(sizeof(ThreadArg) * threadNumber);
    if (args == NULL) {
        printf("ERROR: unable to create an array of arguments for threads\n");
        exit(3);
    }

    // Gets time of the start
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Separating text for threads
    int step = input_string_len / threadNumber;
    for (int i = 0; i < threadNumber; ++i) {
        int curLen = step;
        if (i < threadNumber - 1) {
            curLen += input_pattern_len;
        }
        
        args[i].strLen = curLen;
        args[i].str = text;
        args[i].startIndex = step * i;
        args[i].endIndex = args[i].startIndex + curLen;
        args[i].pattern = pattern;
        args[i].patternLen = input_pattern_len;   
    }
    
    // Starting threads
    for (int i = 0; i < threadNumber; ++i) {
        if (pthread_create(&threads[i], NULL, Search, &args[i]) != 0) {
            perror("ERROR: unable to create thread.\n");
            exit(2);
        }
    }
    
    // Waiting for all threads
    for (int i = 0; i < threadNumber; ++i){
        if (pthread_join(threads[i], NULL) != 0) {
            perror("ERROR: unable to join threads.");
            exit(2);
        }
    }

    // Gets minimal index of the patern occurence if it exists
    int min_index = -1;
    for(int i = 0; i < threadNumber; ++i) {
        if((args[i].res != -1 && args[i].res < min_index) || (min_index == -1 && args[i].res != -1)) {
            min_index = args[i].res;
        }
    }
    if (min_index == -1) {
        printf("Pattern wasn't found!\n");
    }
    else {
        printf("Minimal index of the pattern occurence in the text: %d\n", min_index);
    }

    // Gets finish time and prints overall time
    clock_gettime(CLOCK_MONOTONIC, &finish);
    double elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Time: %.4f seconds\n", elapsed);

    free(text);
    free(pattern);
    free(threads);
    free(args);
    return 0;
}