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
    if (!cmd_line || strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }

    char *token;
    char *rest = cmd_line;
    clist->num = 0;

    while ((token = strtok_r(rest, PIPE_STRING, &rest))) {
        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        while (*token == SPACE_CHAR) token++;
        char *end = token + strlen(token) - 1;
        while (end > token && *end == SPACE_CHAR) *end-- = '\0';

        clist->commands[clist->num].argc = 0;

        // Handle redirection within existing structure
        char *redirect = strstr(token, ">>");
        char *output_file = NULL;
        int is_append = 0;
        
        if (redirect) {
            *redirect = '\0';
            output_file = redirect + 2;
            while (*output_file == ' ') output_file++;
            is_append = 1;
        } else if ((redirect = strchr(token, '>'))) {
            *redirect = '\0';
            output_file = redirect + 1;
            while (*output_file == ' ') output_file++;
        }

        char *arg = strtok(token, " ");
        while (arg && clist->commands[clist->num].argc < CMD_ARGV_MAX - 2) {  // Leave space for redirection
            if (strcmp(arg, "grep") == 0) {
                clist->commands[clist->num].argv[clist->commands[clist->num].argc++] = strdup(arg);
                arg = strtok(NULL, " ");
                if (arg && arg[0] == '"' && arg[strlen(arg) - 1] == '"') {
                    arg[strlen(arg) - 1] = '\0';
                    arg++;
                }
            }
            clist->commands[clist->num].argv[clist->commands[clist->num].argc++] = strdup(arg);
            arg = strtok(NULL, " ");
        }

        // Add redirection information to argv if present
        if (output_file) {
            clist->commands[clist->num].argv[clist->commands[clist->num].argc++] = strdup(is_append ? ">>" : ">");
            clist->commands[clist->num].argv[clist->commands[clist->num].argc++] = strdup(output_file);
        }
        
        clist->commands[clist->num].argv[clist->commands[clist->num].argc] = NULL;
        clist->num++;
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

int execute_pipeline(command_list_t *clist) {
    int num_cmds = clist->num;
    if (num_cmds < 1) return ERR_CMD_ARGS_BAD;
    if (num_cmds > CMD_MAX) return ERR_TOO_MANY_COMMANDS;

    int pipes[num_cmds - 1][2];
    pid_t pids[num_cmds];

    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe error");
            return ERR_EXEC_CMD;
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork error");
            return ERR_EXEC_CMD;
        }

        if (pids[i] == 0) {
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i < num_cmds - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Handle redirection using existing argv
            for (int j = 0; j < clist->commands[i].argc - 1; j++) {
                if (strcmp(clist->commands[i].argv[j], ">") == 0 ||
                    strcmp(clist->commands[i].argv[j], ">>") == 0) {
                    int flags = O_WRONLY | O_CREAT;
                    flags |= (strcmp(clist->commands[i].argv[j], ">>") == 0) ? O_APPEND : O_TRUNC;
                    
                    int fd = open(clist->commands[i].argv[j + 1], flags, 0644);
                    if (fd < 0) {
                        perror("open");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    
                    // Remove redirection arguments
                    clist->commands[i].argv[j] = NULL;
                    break;
                }
            }

            for (int j = 0; j < num_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp error");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < num_cmds; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return OK;
}
int exec_local_cmd_loop() {
    char input[SH_CMD_MAX];
    command_list_t clist;

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(input, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            printf("exiting...\n");
            break;
        } else if (strncmp(input, "cd", 2) == 0) {
            char *dir = strtok(input + 3, " ");
            if (dir) {
                if (chdir(dir) != 0) {
                    perror("cd error");
                }
            }
            continue;
        }

        if (build_cmd_list(input, &clist) != OK) {
            continue;
        }

        execute_pipeline(&clist);
        free_cmd_list(&clist);
    }

    return OK;
}