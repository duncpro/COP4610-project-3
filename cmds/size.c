#include "./../tool_context.h"
#include <stdio.h>
void size_cmd(struct command_context context) {
    if (context.arg_count != 1) {
        printf("Invalid Usage: Expected a single argument, the path to the file.\n");
        return;
    }
}