#include "dispatch.h"
#include "repl.h"
#include <string.h>

// include command modules
#include "cmds/info.c"
#include "cmds/size.c"
#include "cmds/ls.c"
#include "cmds/cd.c"

int dispatch(struct command cmd, struct tool_context* tool_context) {
    if (action(cmd) == NULL) return REPL_STATUS_CONTINUE;

    struct command_context context = {
        .tool_context = tool_context,
        .arg_count = cmd.total_words - 1,
        .args = args(cmd)
    };

    if (strcmp(action(cmd), "exit") == 0) {
        return REPL_STATUS_EXIT;
    }
    if (strcmp(action(cmd), "info") == 0) {
        info_cmd(context);
        return REPL_STATUS_CONTINUE;
    }
    if (strcmp(action(cmd), "size") == 0) {
        size_cmd(context);
        return REPL_STATUS_CONTINUE;
    }
    if (strcmp(action(cmd), "ls") == 0) {
        ls_cmd(context);
        return REPL_STATUS_CONTINUE;
    }
    if (strcmp(action(cmd), "cd") == 0) {
        cd_cmd(context);
        return REPL_STATUS_CONTINUE;
    }
    
    
    printf("Unknown action: %s. Consult the project spec for a list of available actions.\n", action(cmd));
    return REPL_STATUS_CONTINUE;
}