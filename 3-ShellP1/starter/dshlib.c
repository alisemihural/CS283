#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    clist->num = 0;
    memset(clist->commands, 0, sizeof(clist->commands));

    if (cmd_line == NULL || strlen(cmd_line) == 0)
    {
        return WARN_NO_CMDS;
    }

    while (isspace((unsigned char)*cmd_line)) cmd_line++;
    char *end = cmd_line + strlen(cmd_line) - 1;
    while (end > cmd_line && isspace((unsigned char)*end)) end--;
    end[1] = '\0';

    if (strlen(cmd_line) == 0)
    {
        return WARN_NO_CMDS;
    }

    char *saveptr;
    char *token = strtok_r(cmd_line, PIPE_STRING, &saveptr);
    while (token != NULL)
    {
        if (clist->num >= CMD_MAX)
        {
            return ERR_TOO_MANY_COMMANDS;
        }

        while (isspace((unsigned char)*token)) token++;
        char *cmd_end = token + strlen(token) - 1;
        while (cmd_end > token && isspace((unsigned char)*cmd_end)) cmd_end--;
        cmd_end[1] = '\0';

        char *saveptr_inner;
        char *exe = strtok_r(token, " ", &saveptr_inner);
        if (exe == NULL || strlen(exe) >= EXE_MAX)
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        strcpy(clist->commands[clist->num].exe, exe);

        char *args = saveptr_inner;
        if (args != NULL && strlen(args) >= ARG_MAX)
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        if (args != NULL)
        {
            strcpy(clist->commands[clist->num].args, args);
        }
        else
        {
            clist->commands[clist->num].args[0] = '\0';  // Ensure empty args are handled
        }

        clist->num++;
        token = strtok_r(NULL, PIPE_STRING, &saveptr);
    }

    if (clist->num == 0)
    {
        return WARN_NO_CMDS;
    }

    return OK;
}