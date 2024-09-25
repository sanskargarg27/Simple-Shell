#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

typedef struct{
    char command[1024]; 
    time_t start_time;
    double execution_time;
    pid_t pid;
}history_t;

history_t HistoryList[100];
int historyCnt = 0;

void command_history(){
    if(historyCnt==0){
        printf("No commands in history\n");
        return;
    }
    // printf("Command History: \n");
    int i=0;
    while(i<historyCnt){
        printf("Command [%d]:\n",i+1);
        printf("PID: %d\n", HistoryList[i].pid);
        printf("Command: %s\n", HistoryList[i].command);
        printf("Start time: %s", ctime(&HistoryList[i].start_time));
        printf("Execution Duration: %.2f seconds\n", HistoryList[i].execution_time);
        printf("-----------------------------\n");
        i++;
    }

}

 
int create_process_and_run(char* cmd){
    if (strcmp(cmd, "exit") == 0){
        return 1;
    }
    else if (strcmp(cmd, "history") == 0){
        command_history();
        return 1;
    }
    char* args[100];
    int cnt = 0;
    char *token;

    token = strtok(cmd, " ");

    while(token != NULL && cnt < 100){
        args[cnt] = token;
        cnt++;
        token = strtok(NULL, " ");
    }

    if (cnt < 100){
        args[cnt] = NULL;
    }

    int ret = fork();
    if (ret < 0){
        perror("fork error");
    }
    else if(ret == 0){
        execvp(args[0], args);
        perror("exec failed");
    }
    else{
        if (historyCnt>= 100){
            for (int i = 1; i < 100; i++){
                HistoryList[i-1] = HistoryList[i];
            }
            historyCnt--;
        }

        time(&HistoryList[historyCnt].start_time);
        HistoryList[historyCnt].pid = ret;
        strncpy(HistoryList[historyCnt].command, cmd, 1024);
        wait(NULL);
    }
    HistoryList[historyCnt].execution_time = difftime(time(NULL), HistoryList[historyCnt].start_time);
    historyCnt++;
    return 1;
}

void remove_space(char* str){
    int start = 0;
    int end = strlen(str) - 1;
    // printf("%d\n", end);

    while (isspace(str[start])){
        start++;
    }
    while (end >= 0 && isspace(str[end])){
        end--;
    }
    for (int i = 0; i <= end - start; i++){
        str[i] = str[i + start];
    }
    str[end - start + 1] = '\0';
}


char* read_user_input(){
    char* inp = malloc(1024 * sizeof(char));
    fgets(inp, 1024, stdin);

    remove_space(inp);
    
    return inp;
}

int run_piped_process(char* cmd1, char* cmd2, char* cmd){
    int pipe_fd[2]; // Pipe - read & write end
    pid_t pid1, pid2; // This is a child 1 & child 2 - forked twice xD

    if (pipe(pipe_fd) == -1) { // Create a pipe
        perror("Pipe error");
        exit(1);
    }

    pid1 = fork();

    if (pid1 < 0) {
        perror("Fork error");
        exit(1);
    }
    else if (pid1 == 0) { // Child process 1 (writes to the pipe)
        close(pipe_fd[0]); // Close the read end of the pipe
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
            perror("dup2 error");
            exit(1);
        }
        close(pipe_fd[1]);

        // Execute the first command
        if (system(cmd) == -1) { // Systuummmmm - To execute command. IYKYK ; )
            perror("System error");
            exit(1);
        }
        exit(0);
    }
    else { // Parent process
        pid2 = fork(); // Child process 2

        if (pid2 < 0) {
            perror("Fork error");
            exit(1);
        }
        else if (pid2 == 0) { // Child process 2 (reads from the pipe)
            close(pipe_fd[1]); // Close the write end of the pipe
            if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
                perror("dup2 error");
                exit(1);
            }
            close(pipe_fd[0]);

            // Execute the second command
            if (system(cmd) == -1) {
                perror("System error");
                exit(1);
            }
            exit(0);
        }
        else { // Parent process
            close(pipe_fd[0]);
            close(pipe_fd[1]);

            // Good Parenting Practise - Prevent Orphan Children
            if (waitpid(pid1, NULL, 0) == -1) {
                perror("waitpid error");
                exit(1);
            }
            if (waitpid(pid2, NULL, 0) == -1) {
                perror("waitpid error");
                exit(1);
            }
        }
    }
    return 1;
}

int launch(char* cmd){
    int status;
    // Unpiped command
    if (strchr(cmd, '|') == NULL){
        status = create_process_and_run(cmd);
    }
    // Piped command
    else{
        char* token = strtok(cmd, "|");

        char* cmd1;
        char* cmd2;

        while (token != NULL){
            cmd1 = token;
            token = strtok(NULL, "|");
        }
        remove_space(cmd1);
        remove_space(cmd2);
        // printf("C1: %s, C2: %s", cmd1, cmd2);
        status = run_piped_process(cmd1, cmd2, cmd);
    }
    return status;
}
// Signal handler for SIGINT (Ctrl+C)
void sigint_handler(int signum){
    printf("\nCtrl-c pressed\n");
    printf("\nExiting the shell...........\n");
    printf("Command History:\n");
    command_history();  // Calling the function to display the command history 
    exit(0);  
}

// Setting the signal handler for Ctrl+C using sigaction
void set_signal_handler(){
    struct sigaction sa;  
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1){
        perror("sigaction error");  
        exit(EXIT_FAILURE);  
    }
}

void shell_loop(){
    int status;
    do {
        printf("simpleShell:~$ ");
        char* cmd = read_user_input();
        status = launch(cmd);
        free(cmd);
    } while(status);
}

int main(){
    set_signal_handler();
    shell_loop();
    return 0;
}