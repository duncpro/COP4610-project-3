#ifndef FSU_REPL
#define FSU_REPL

#include "tool_context.h"
#include <stdbool.h>


#define REPL_STATUS_EXIT 1
#define REPL_STATUS_CONTINUE 0

/**
 * The FAT32 Tools Read, Evalulte, Print Loop.
 * Will block until the user dispatches an exiting command.
 */
void repl(struct tool_context context);

#endif