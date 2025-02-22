#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "dshlib.h"

#define MAX_INPUT 1024

// Global variable to store the last return code for "rc" builtin.
int last_rc = 0;

/*
 * trim_whitespace: Remove leading and trailing whitespace from a string.
 */
static char *trim_whitespace(char *str) {
    while (*str == ' ' || *str == '\t' || *str == '\n')
        str++;
    char *end = str + strlen(str) - 1;
    while (end > str && (*end==' ' || *end=='\t' || *end=='\n'))
        end--;
    *(end+1) = '\0';
    return str;
}

/*
 * parse_command: Parse a line of input into the provided cmd_buff_t.
 * 
 * This version handles quoted strings by:
 *   - If a token starts with a double quote, it scans until the closing quote.
 *   - The quotes are removed, but all inner spaces are preserved.
 *   - Nonquoted tokens are split on whitespace.
 *
 * The parsed tokens are stored as pointers inside a mutable copy of the input.
 */
void parse_command(char *input, cmd_buff_t *cmd) {
    cmd->argc = 0;
    // Make a mutable copy of input.
    cmd->_cmd_buffer = strdup(input);
    char *str = cmd->_cmd_buffer;
    char *token;
    while (*str) {
        // Skip any leading whitespace.
        while (*str == ' ' || *str == '\t')
            str++;
        if (*str == '\0')
            break;
        if (*str == '"') {
            // Quoted token: skip the opening quote.
            str++;
            token = str;
            char *end_quote = strchr(str, '"');
            if (end_quote) {
                // Terminate token at closing quote.
                *end_quote = '\0';
                str = end_quote + 1;
            } else {
                // No closing quote: take rest of string.
                str += strlen(str);
            }
        } else {
            // Unquoted token.
            token = str;
            while (*str && *str != ' ' && *str != '\t')
                str++;
            if (*str) {
                *str = '\0';
                str++;
            }
        }
        cmd->argv[cmd->argc++] = token;
    }
    cmd->argv[cmd->argc] = NULL;
}

/*
 * free_command: Free the memory allocated by parse_command.
 *
 * Since the tokens are pointers into _cmd_buffer, we only need to free that.
 */
void free_command(cmd_buff_t *cmd) {
    free(cmd->_cmd_buffer);
}

extern void print_dragon(); // Declare the function from dragon.c
/*
 * handle_builtin: Execute built-in commands (cd, rc, exit).
 *
 * cd: with no arguments does nothing; with one argument, uses chdir().
 * rc: prints the last return code.
 * exit: exits the shell.
 *
 * Returns 1 if a built-in was executed, 0 otherwise.
 */
int handle_builtin(cmd_buff_t *cmd) {
    if (cmd->argc == 0)
        return 1;
    
    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc == 1) {
            return 1;
        } else if (cmd->argc == 2) {
            if (chdir(cmd->argv[1]) != 0) {
                perror("cd error");
            }
            return 1;
        } else {
            fprintf(stderr, "cd: too many arguments\n");
            return 1;
        }
    }
    // Add dragon command handling
    if (strcmp(cmd->argv[0], "dragon") == 0) {
        print_dragon();
        return 1;
    }
    if (strcmp(cmd->argv[0], "rc") == 0) {
        printf("%d\n", last_rc);
        return 1;
    }
    if (strcmp(cmd->argv[0], "exit") == 0) {
        exit(0);
    }
    return 0;
}

/*
 * exec_local_cmd_loop: The main shell loop.
 *
 * This function prints the prompt (to stderr), reads a line from stdin,
 * parses it into a cmd_buff_t, and then either executes a built-in command or
 * forks and execs an external command using execvp(). The parent's waitpid()
 * call extracts the exit status for use by the "rc" builtin.
 */
int exec_local_cmd_loop() {
    setlinebuf(stdout);  // Force line buffering for stdout.
    char input[MAX_INPUT];
    int interactive = isatty(STDIN_FILENO);
    while (1) {
        if (interactive) {
            printf("dsh2> ");
            fflush(stdout);
        }
        if (fgets(input, sizeof(input), stdin) == NULL)
            break;
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n')
            input[len-1] = '\0';
        char *trimmed = trim_whitespace(input);
        if (strlen(trimmed) == 0)
            continue;
        cmd_buff_t cmd;
        parse_command(trimmed, &cmd);
        if (handle_builtin(&cmd)) {
            free_command(&cmd);
            continue;
        }
        pid_t pid = fork();
        if (pid < 0) {
            printf("fork error: %s\n", strerror(errno));
            free_command(&cmd);
            if (!interactive) {
                printf("\ndsh2> ");
                fflush(stdout);
            }
            continue;
        }
        if (pid == 0) {  // Child process.
            execvp(cmd.argv[0], cmd.argv);
            // If execvp returns, an error occurred.
            int err = errno;
            switch (err) {
                case ENOENT:
                    printf("Command not found in PATH\n");
                    break;
                case EACCES:
                    printf("Permission denied\n");
                    break;
                default:
                    printf("Error executing command: %s\n", strerror(err));
            }
            exit(err);
        } else {  // Parent process.
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                last_rc = WEXITSTATUS(status);
            } else {
                last_rc = -1;
            }
        }
        free_command(&cmd);
        if (!interactive) {
            printf("\ndsh2> ");
            fflush(stdout);
        }
    }
    if (!interactive) {
        printf("\ndsh2> ");
        fflush(stdout);
    }
    return last_rc;
}