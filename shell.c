#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

void command_history();
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
        return -1;
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

int launch(char* cmd){
    // Unpiped command
    if (strchr(cmd, '|') == NULL){
        int status;
        status = create_process_and_run(cmd);
        return status;
    }
    // Piped command
    else{

    }
}
// Signal handler for SIGINT (Ctrl+C)
void sigint_handler(int signum){
    printf("Ctrl-c pressed\n");
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
        printf("cmd: %s\n", cmd);
        status = launch(cmd);
        free(cmd);
    } while(status);
}
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

int main(){
    set_signal_handler();
    shell_loop();
    return 0;
}