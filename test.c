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
void func(){
    int a = 1;
    char cmd[] = "ls";
    if (a == 1){
        system(cmd);
    }
}
int execute_piped_commands(char *cmd) {
    int pipe_fd[2]; // Pipe - read & write end
    pid_t pid1, pid2; // This is a child 1 & child 2 - forked twice xD

    if (pipe(pipe_fd) == -1) { // Create a pipe
        perror("Pipe error");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();

    if (pid1 < 0) {
        perror("Fork error");
        exit(EXIT_FAILURE);
    }
    else if (pid1 == 0) { // Child process 1 (writes to the pipe)
        close(pipe_fd[0]); // Close the read end of the pipe
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
            perror("dup2 error");
            exit(EXIT_FAILURE);
        }
        close(pipe_fd[1]);

        // Execute the first command
        if (system(cmd) == -1) { // Systuummmmm - To execute command. IYKYK ; )
            perror("System error");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    else { // Parent process
        printf("here");
        if (waitpid(pid1, NULL, 0) == -1) {
            perror("waitpid error");
            exit(EXIT_FAILURE);
        }
    }
    return 1;
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
    // create_process_and_run();

    char cmd[] = "cat t.txt | ./a.out";
    // system(cmd);
    execute_piped_commands(cmd);
    printf("Here\n");
    
    
    return 0;
}
