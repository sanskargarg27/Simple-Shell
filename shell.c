#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
        wait(NULL);
    }
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
    shell_loop();
    return 0;
}