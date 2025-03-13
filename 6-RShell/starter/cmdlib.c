#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dshlib.h"
#include "rshlib.h"

/**
 * build_cmd_list - Parse a command line string into a list of commands
 * 
 * @param cmd_line: The input command line string to parse
 * @param clist: Pointer to command_list_t structure to store parsed commands
 * 
 * @return OK on success, error code on failure
 */
int build_cmd_list(char *cmd_line, command_list_t *clist) {
    char *token;
    char *saveptr;
    char *cmd_copy;

    // Clear the existing command list
    memset(clist, 0, sizeof(command_list_t));
    
    // Check if cmd_line is empty
    if (!cmd_line || strlen(cmd_line) == 0) {
        fprintf(stderr, "%s", CMD_WARN_NO_CMD);
        return WARN_NO_CMDS;
    }

    // Make a copy of the command line for tokenization
    cmd_copy = strdup(cmd_line);
    if (!cmd_copy) {
        return ERR_MEMORY;
    }
    
    // Split the command by pipe character
    token = strtok_r(cmd_copy, "|", &saveptr);
    
    // Process each command segment
    while (token != NULL && clist->num < CMD_MAX) {
        // Skip leading whitespace
        while (*token == ' ' || *token == '\t') {
            token++;
        }
        
        // Allocate and copy the command
        clist->commands[clist->num]._cmd_buffer = strdup(token);
        if (!clist->commands[clist->num]._cmd_buffer) {
            free(cmd_copy);
            free_cmd_list(clist);
            return ERR_MEMORY;
        }
        
        // Parse arguments for this command
        char *arg;
        char *arg_saveptr;
        int arg_idx = 0;
        
        // Make a working copy for tokenization of arguments
        char *cmd_args = strdup(token);
        if (!cmd_args) {
            free(cmd_copy);
            free_cmd_list(clist);
            return ERR_MEMORY;
        }
        
        // Split command into arguments
        arg = strtok_r(cmd_args, " \t", &arg_saveptr);
        while (arg != NULL && arg_idx < CMD_ARGV_MAX - 1) {
            // Handle redirection for extra credit
            if (strcmp(arg, "<") == 0) {
                arg = strtok_r(NULL, " \t", &arg_saveptr);
                if (arg) {
                    clist->commands[clist->num].input_file = strdup(arg);
                }
                arg = strtok_r(NULL, " \t", &arg_saveptr);
                continue;
            } else if (strcmp(arg, ">") == 0) {
                arg = strtok_r(NULL, " \t", &arg_saveptr);
                if (arg) {
                    clist->commands[clist->num].output_file = strdup(arg);
                    clist->commands[clist->num].append_mode = false;
                }
                arg = strtok_r(NULL, " \t", &arg_saveptr);
                continue;
            } else if (strcmp(arg, ">>") == 0) {
                arg = strtok_r(NULL, " \t", &arg_saveptr);
                if (arg) {
                    clist->commands[clist->num].output_file = strdup(arg);
                    clist->commands[clist->num].append_mode = true;
                }
                arg = strtok_r(NULL, " \t", &arg_saveptr);
                continue;
            } else {
                // Regular argument
                clist->commands[clist->num].argv[arg_idx] = strdup(arg);
                arg_idx++;
            }
            
            arg = strtok_r(NULL, " \t", &arg_saveptr);
        }
        
        // Null-terminate the argument array
        clist->commands[clist->num].argv[arg_idx] = NULL;
        clist->commands[clist->num].argc = arg_idx;
        
        free(cmd_args);
        clist->num++;
        
        token = strtok_r(NULL, "|", &saveptr);
    }
    
    free(cmd_copy);
    
    // Check if we have too many commands
    if (token != NULL) {
        fprintf(stderr, CMD_ERR_PIPE_LIMIT, CMD_MAX);
        free_cmd_list(clist);
        return ERR_TOO_MANY_COMMANDS;
    }
    
    return OK;
}

/**
 * free_cmd_list - Free memory allocated for a command list
 * 
 * @param clist: Pointer to command_list_t structure to free
 * 
 * @return OK on success
 */
int free_cmd_list(command_list_t *clist) {
    for (int i = 0; i < clist->num; i++) {
        // Free all argument strings
        for (int j = 0; j < clist->commands[i].argc; j++) {
            if (clist->commands[i].argv[j]) {
                free(clist->commands[i].argv[j]);
                clist->commands[i].argv[j] = NULL;
            }
        }
        
        // Free the command buffer
        free(clist->commands[i]._cmd_buffer);
        clist->commands[i]._cmd_buffer = NULL;
        
        // Free input/output redirection files
        if (clist->commands[i].input_file) {
            free(clist->commands[i].input_file);
            clist->commands[i].input_file = NULL;
        }
        
        if (clist->commands[i].output_file) {
            free(clist->commands[i].output_file);
            clist->commands[i].output_file = NULL;
        }
    }
    
    // Reset command count
    clist->num = 0;
    
    return OK;
}

/**
 * alloc_cmd_buff - Allocate memory for a command buffer
 * 
 * @param cmd_buff: Pointer to cmd_buff_t structure to allocate
 * 
 * @return OK on success, ERR_MEMORY on failure
 */
int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }
    
    cmd_buff->argc = 0;
    cmd_buff->input_file = NULL;
    cmd_buff->output_file = NULL;
    cmd_buff->append_mode = false;
    
    return OK;
}

/**
 * free_cmd_buff - Free memory allocated for a command buffer
 * 
 * @param cmd_buff: Pointer to cmd_buff_t structure to free
 * 
 * @return OK on success
 */
int free_cmd_buff(cmd_buff_t *cmd_buff) {
    free(cmd_buff->_cmd_buffer);
    cmd_buff->_cmd_buffer = NULL;
    cmd_buff->argc = 0;
    
    if (cmd_buff->input_file) {
        free(cmd_buff->input_file);
        cmd_buff->input_file = NULL;
    }
    
    if (cmd_buff->output_file) {
        free(cmd_buff->output_file);
        cmd_buff->output_file = NULL;
    }
    
    return OK;
}

/**
 * build_cmd_buff - Parse a command string into a command buffer structure
 * 
 * @param cmd_line: The input command string to parse
 * @param cmd_buff: Pointer to cmd_buff_t structure to store parsed command
 * 
 * @return OK on success, error code on failure
 */
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    char *token;
    char *saveptr;
    
    // Check if cmd_line is empty
    if (!cmd_line || strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }
    
    // Clear existing command buffer
    clear_cmd_buff(cmd_buff);
    
    // Make a copy of the command line
    strcpy(cmd_buff->_cmd_buffer, cmd_line);
    
    // Split the command into arguments
    token = strtok_r(cmd_buff->_cmd_buffer, " \t", &saveptr);
    
    while (token != NULL && cmd_buff->argc < CMD_ARGV_MAX - 1) {
        // Handle redirection for extra credit
        if (strcmp(token, "<") == 0) {
            token = strtok_r(NULL, " \t", &saveptr);
            if (token) {
                cmd_buff->input_file = strdup(token);
            }
        } else if (strcmp(token, ">") == 0) {
            token = strtok_r(NULL, " \t", &saveptr);
            if (token) {
                cmd_buff->output_file = strdup(token);
                cmd_buff->append_mode = false;
            }
        } else if (strcmp(token, ">>") == 0) {
            token = strtok_r(NULL, " \t", &saveptr);
            if (token) {
                cmd_buff->output_file = strdup(token);
                cmd_buff->append_mode = true;
            }
        } else {
            // Regular argument
            cmd_buff->argv[cmd_buff->argc++] = token;
        }
        
        token = strtok_r(NULL, " \t", &saveptr);
    }
    
    // Null-terminate the argument array
    cmd_buff->argv[cmd_buff->argc] = NULL;
    
    return OK;
}

/**
 * clear_cmd_buff - Reset a command buffer structure without freeing memory
 * 
 * @param cmd_buff: Pointer to cmd_buff_t structure to clear
 * 
 * @return OK on success
 */
int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->argc = 0;
    memset(cmd_buff->argv, 0, sizeof(char*) * CMD_ARGV_MAX);
    
    if (cmd_buff->input_file) {
        free(cmd_buff->input_file);
        cmd_buff->input_file = NULL;
    }
    
    if (cmd_buff->output_file) {
        free(cmd_buff->output_file);
        cmd_buff->output_file = NULL;
    }
    
    cmd_buff->append_mode = false;
    return OK;
}

/**
 * close_cmd_buff - Final cleanup for a command buffer
 * 
 * @param cmd_buff: Pointer to cmd_buff_t structure to close
 * 
 * @return OK on success
 */
int close_cmd_buff(cmd_buff_t *cmd_buff) {
    return free_cmd_buff(cmd_buff);
}