#ifndef FSU_TOOL_CONTEXT
#define FSU_TOOL_CONTEXT

/**
 * Global state for the FAT32 tools application.
 */
struct tool_context {
    int image_fd;  
};

/**
 * Context which is made available to FAT32 Tools REPL commands.
 */
struct command_context {
    struct tool_context* tool_context;
    int arg_count;
    char** args;
};
#endif