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
 
int create_process_and_run(char* cmd){
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
int run_piped_process(char* cmd1, char* cmd2){
    pid_t pid1, pid2;
    int fd[2];

    pipe(fd);

    pid1 = fork();
    if (pid1 < 0){
        perror("fork error");
    }
    else if (pid1 == 0){
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        system(cmd1);
        exit(1);
    }
    else{
        pid2 = fork();

        if (pid2 < 0){
            perror("fork error");
        }
        else if (pid2 == 0){
            close(fd[1]);
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            system(cmd2);
            exit(1);
        }
        else{
            close(fd[0]);
            close(fd[1]);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
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
        status = run_piped_process(cmd1, cmd2);
    }
    return status;
}
// Signal handler for SIGINT (Ctrl+C)
void sigint_handler(int signum){
    printf("\nCtrl-c pressed\n");
    printf("\nExiting the shell...........\n");
    printf("Command History:\n");
    // display_history();  // Calling the function to display the command history 
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
        printf("cmd: %s\n", cmd);
        status = launch(cmd);
        free(cmd);
    } while(status);
}

int main(){
    set_signal_handler();
    shell_loop();
    return 0;
}