#include "tool_context.h"
#include <string.h>
#include <stdlib.h>

void set_context_cwd(struct tool_context* context, char* new_cwd) {
    context->cwd = (char*) realloc(context->cwd, sizeof(char) * (strlen(new_cwd) + 1));
    strcpy(context->cwd, new_cwd);
}