#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist) {
    if (!cmd_line || !clist) return ERR_MEMORY;

    clist->num = 0;
    char *token = strtok(cmd_line, "|");

    while (token && clist->num < CMD_MAX) {
        // Trim leading spaces
        while (*token == ' ') token++;

        clist->commands[clist->num].argc = 0;

        // Tokenize command arguments
        char *arg = strtok(token, " \t");
        while (arg && clist->commands[clist->num].argc < CMD_ARGV_MAX - 1) {
            // If the argument contains a quote, treat it as a single argument
            if (arg[0] == '"' || arg[0] == '\'') {
                char *end = strchr(arg + 1, arg[0]);
                if (end) {
                    *end = '\0';
                    arg++;
                }
            }
            clist->commands[clist->num].argv[clist->commands[clist->num].argc] = strdup(arg);
            clist->commands[clist->num].argc++;
            arg = strtok(NULL, " \t");
        }

        // Ensure NULL termination for execvp()
        clist->commands[clist->num].argv[clist->commands[clist->num].argc] = NULL;

        // Debug print
        printf("Command %d: ", clist->num);
        for (int i = 0; i < clist->commands[clist->num].argc; i++) {
            printf("[%s] ", clist->commands[clist->num].argv[i]);
        }
        printf("\n");

        clist->num++;
        token = strtok(NULL, "|");
    }

    return OK;
}

int free_cmd_list(command_list_t *clist) {
    if (!clist) return ERR_MEMORY;

    for (int i = 0; i < clist->num; i++) {
        for (int j = 0; j < clist->commands[i].argc; j++) {
            free(clist->commands[i].argv[j]);
        }
    }
    clist->num = 0;

    return OK;
}

int exec_local_cmd_loop() {
    char input[SH_CMD_MAX];
    command_list_t clist;

    while (1) {
        printf("%s", SH_PROMPT);
        fflush(stdout);

        if (fgets(input, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, EXIT_CMD) == 0) {
            printf("exiting...\n");
            break;
        }

        if (build_cmd_list(input, &clist) != OK) {
            continue;
        }

        execute_pipeline(&clist);
        free_cmd_list(&clist);
    }

    return OK;
}

int execute_pipeline(command_list_t *clist) {
    int num_cmds = clist->num;
    int pipes[2][2];
    pid_t pid;
    int in_fd = STDIN_FILENO;

    for (int i = 0; i < num_cmds; i++) {
        if (i < num_cmds - 1) {
            if (pipe(pipes[i % 2]) < 0) {
                perror("pipe");
                return ERR_EXEC_CMD;
            }
        }

        pid = fork();
        if (pid == 0) {
            // Child process
            if (in_fd != STDIN_FILENO) {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }

            if (i < num_cmds - 1) {
                close(pipes[i % 2][0]);
                dup2(pipes[i % 2][1], STDOUT_FILENO);
                close(pipes[i % 2][1]);
            }

            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("fork");
            return ERR_EXEC_CMD;
        }

        // Parent process
        if (in_fd != STDIN_FILENO)
            close(in_fd);
        if (i < num_cmds - 1) {
            close(pipes[i % 2][1]);
            in_fd = pipes[i % 2][0];
        }
    }

    // If there was more than one command, read from the last pipe
    if (num_cmds > 1) {
        char buffer[1024];
        ssize_t bytes_read;
        close(pipes[(num_cmds - 2) % 2][1]);  // Close write end of last pipe
        while ((bytes_read = read(pipes[(num_cmds - 2) % 2][0], buffer, sizeof(buffer))) > 0) {
            write(STDOUT_FILENO, buffer, bytes_read);
        }
        close(pipes[(num_cmds - 2) % 2][0]);
    }

    // Wait for all child processes
    while (wait(NULL) > 0);

    return OK;
}
