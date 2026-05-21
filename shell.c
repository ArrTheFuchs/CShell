#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

// splits the input string into individual command tokens
int split_into_commands(char *user_input, char **commands){

    int num_commands = 0;
    char *token = strtok(user_input, " ");

    while(token != NULL && num_commands < 63){
            commands[num_commands] = token;
        num_commands++;
        token = strtok(NULL, " ");
    }

    commands[num_commands] = NULL;

    return num_commands;
}

// changes the current working direcory
void change_directory(char **commands){

    const char *dir;

    if(commands[1] == NULL){
        dir = getenv("HOME");
            if(dir == NULL){
            fprintf(stderr, "cd: HOME not set\n");
            return;
        }
    }
    else{
        dir = commands[1];
    }

    if(chdir(dir) != 0)
        perror("cd");
}

// forks a child process and runs the command with execvp
void run_command(char **commands){

    pid_t child_pid = fork();

    if(child_pid < 0){
        perror("fork");
    }
    else if(child_pid == 0){
        execvp(commands[0], commands);
            perror(commands[0]);
        exit(EXIT_FAILURE);
    }
    else{
        int status;
        waitpid(child_pid, &status, 0);
    }
}

int shell(){

    char *user_input = malloc(1024);
    char **commands = malloc(64 * sizeof(char *));
    char **history = malloc(100 * sizeof(char *));
    int num_history = 0;

    while(1){

        printf("\n$ ");
        fflush(stdout);

        // read input
        if(!fgets(user_input, 1024, stdin)){
            printf("\n");
            break;
        }

        // strip newline
        user_input[strcspn(user_input, "\n")] = '\0';

        if(user_input[0] == '\0'){
            continue;}

        // run history entry by number
        if(atoi(user_input) > 0){
            int index = atoi(user_input) - 1;
                if(index < num_history){
                    
                strcpy(user_input, history[index]);
            }


            else{
                printf("no command at that number\n");
            }
        }

        // save to history
        if(num_history < 100){
            history[num_history] = strdup(user_input);
            num_history++;
        }

        int num_commands = split_into_commands(user_input, commands);

        if(num_commands == 0)
            continue;

        if(strcmp(commands[0], "exit") == 0)
            break;

        // print history
        if(strcmp(commands[0], "history") == 0){
            for(int i = 0; i < num_history; i++){
                printf("%d  %s\n", i + 1, history[i]);
            }

        }

        if(strcmp(commands[0], "cd") == 0){
            change_directory(commands);
        }

        run_command(commands);
    }

    // free history
    for(int i = 0; i < num_history; i++){
        free(history[i]);
    }

    free(history);
    free(user_input);
    free(commands);

    return 0;
}

int main(){
    shell();
}