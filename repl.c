#include "repl.h"
#include "dispatch.h"
#include "string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

void print_prompt(struct tool_context context) {
    printf("%s> ", context.image_file_name);
}


void repl(struct tool_context context) {
    int repl_status = REPL_STATUS_CONTINUE;

    printf("Type \"exit\" to shutdown REPL.\n");
 
    while (repl_status == REPL_STATUS_CONTINUE) {
        print_prompt(context);
        char* line_buffer = NULL;
        size_t line_buffer_size;
        int line_length = getline(&line_buffer, &line_buffer_size, stdin);
        remove_all('\n', line_buffer);
        repl_status = dispatch(parse_command(trim(line_buffer)), &context);
        free(line_buffer);
    }
}