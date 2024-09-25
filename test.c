#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int create_process_and_run() {
    int status = fork();
    if(status < 0) {
        printf("Something bad happened\n");
        exit(0);
    } else if(status == 0) {
        printf("I am the child process\n");
        char* args[3] = {"echo", "hello", NULL};
        execvp(args[0], args);
        printf("I should never print\n");
    } else {
        printf("I am the parent Shell\n");
    }
    return 0;
}

int main() {
    // int error_code = 404;
    
    // // Write an error message to stderr
    // fprintf(stderr, "Error: Could not find the resource (code %d).\n", error_code);

    // char inp[20];
    // fgets(inp, 20, stdin);
    // printf("%ld: %s", sizeof(inp), inp);
    // char* result = strchr(inp, '\n');
    // printf("Character ' ' found at position: %ld\n", result - inp);
    // inp[3] = '\0';
    // char* resu = strchr(inp, '\n');
    // printf("Character ' ' found at position: %ld\n", resu - inp);

    // char inp[5] = "sansk";
    // printf("%s", inp[2:4]);

    // char* inp;
    // fgets(inp, sizeof(inp), stdin);
    // printf("%s", inp);
    create_process_and_run();
    
    
    return 0;
}
