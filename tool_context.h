#ifndef FSU_TOOL_CONTEXT
#define FSU_TOOL_CONTEXT
struct tool_context {
    int image_fd;  
};

struct command_context {
    struct tool_context* tool_context;
    int arg_count;
    char** args;
};
#endif