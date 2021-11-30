#ifndef FSU_DISPATCH
#define FSU_DISPATCH

#include "command.h"
#include "tool_context.h"

int dispatch(struct command cmd, struct tool_context* tool_context);

#endif