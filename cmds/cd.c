#include "./../tool_context.h"
#include "./../path.h"
#include <stdio.h>

void cd_cmd(struct command_context context) {
    if (context.arg_count != 1) {
        printf("Invalid Usage: Expected relative or absolute path as only argument for CD command.\n");
    }
    relativize()
}