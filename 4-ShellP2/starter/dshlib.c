#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"
#include <errno.h>

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

int last_return_code = 0;

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (!cmd_buff->_cmd_buffer) {
        return ERR_MEMORY;
    }
    return OK;
}
 
int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    for (int i = 0; i < cmd_buff->argc; i++) {
        free(cmd_buff->argv[i]);  // Free dynamically allocated argument strings
    }
    return OK;
}
 
int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    memset(cmd_buff, 0, sizeof(cmd_buff_t));
    return OK;
}
 
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    cmd_buff->argc = 0;
    int i = 0;
    int in_quotes = 0;
    char *token = malloc(SH_CMD_MAX);
    int token_index = 0;
 
    if (!token) {
        return ERR_MEMORY; 
    }
 
    while (cmd_line[i] != '\0') {
        if (cmd_line[i] == '"' && !in_quotes) {
            in_quotes = 1;
            i++;
            continue;
        } else if (cmd_line[i] == '"' && in_quotes) {
            in_quotes = 0;
            i++;
            continue;
        }
 
        if (in_quotes || cmd_line[i] != ' ') {
            token[token_index++] = cmd_line[i];
        }
 
        if ((cmd_line[i] == ' ' && !in_quotes) || cmd_line[i+1] == '\0') {
            if (token_index > 0) {
                token[token_index] = '\0'; 
                cmd_buff->argv[cmd_buff->argc++] = strdup(token);
                token_index = 0;
            }
        }
        i++;
    }
    cmd_buff->argv[cmd_buff->argc] = NULL;
 
    free(token);
    return OK;
}
 
Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, "exit") == 0) return BI_CMD_EXIT;
    if (strcmp(input, "dragon") == 0) return BI_CMD_DRAGON;
    if (strcmp(input, "cd") == 0) return BI_CMD_CD;
    if (strcmp(input, "rc") == 0) return BI_RC;
    return BI_NOT_BI;
}
 
Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (cmd->argc == 0) return BI_RC;
 
    if (strcmp(cmd->argv[0], "exit") == 0) {
        exit(0);
    }
    else if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc > 1) {
            if (chdir(cmd->argv[1]) != 0) {
                perror("cd");
            }
        }
        return BI_EXECUTED;
    }
    else if (strcmp(cmd->argv[0], "rc") == 0) {
        printf("%d\n", last_return_code);
        fflush(stdout);
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
}
 
int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        last_return_code = ERR_EXEC_CMD;
        return ERR_EXEC_CMD;
    } 
    else if (pid == 0) {  // Child process
        execvp(cmd->argv[0], cmd->argv);

        // Capture the exact error code
        int error_code = errno;
        switch (error_code) {
            case ENOENT:
                fprintf(stderr, "Command not found in PATH\n");
                _exit(127);  // Restore 127 for "command not found"
            case EACCES:
                fprintf(stderr, "Error: Permission denied for '%s'\n", cmd->argv[0]);
                _exit(126);  // Permission denied
            default:
                fprintf(stderr, "Error: Failed to execute '%s' (errno: %d)\n", cmd->argv[0], error_code);
                _exit(error_code);
        }
    } 
    else {  // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            last_return_code = WEXITSTATUS(status);
        } else {
            last_return_code = ERR_EXEC_CMD;
        }

        fflush(stdout);
        return last_return_code;
    }
}


int exec_local_cmd_loop() {
    char cmd_line[SH_CMD_MAX];
    cmd_buff_t cmd_buff;
     
    while (1) {
        printf(SH_PROMPT);
        if (fgets(cmd_line, SH_CMD_MAX, stdin) == NULL) {
            break;
        }
        cmd_line[strcspn(cmd_line, "\n")] = 0; // Remove newline
         
        if (build_cmd_buff(cmd_line, &cmd_buff) != OK) {
            fprintf(stderr, "Error processing command\n");
            continue;
        }
         
        Built_In_Cmds result = match_command(cmd_buff.argv[0]);
        if (result != BI_NOT_BI) {
            exec_built_in_cmd(&cmd_buff);
        } else {
            exec_cmd(&cmd_buff);
        }
    }
    return OK;
}