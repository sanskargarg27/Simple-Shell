#include <stdio.h>
#include <stdlib.h>

char read_user_input(){
    return "ls";
}
int global=0;
int create_process_and_run(char command) {
    int status = fork();
    if(status < 0) {
     printf("Something bad happened\n");
    } else if(status == 0) {
        printf("I am the child process\n");
        global++;
    } else {
        printf("I am the parent Shell\n");
    }
    printf("Global value = %d\n",global);
    return 0;
}

int launch (char command) {
    int status;
    status = create_process_and_run(command);
    return status;
}
void shell_loop() {
    int status;
    do {
    printf("iiitd@possum:~$ ");
    char command = read_user_input();
    status = launch(command);
    } while(status);
}


int main() {
    // char name[100];  // Allocating space for 100 characters
    // printf("Enter your name: ");
    // scanf("%s", name);  // Reads input until a space is encountered
    // printf("Hello, %s\n", name);
    shell_loop();
    return 0;
}
