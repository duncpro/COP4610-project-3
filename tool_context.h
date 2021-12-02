#ifndef FSU_TOOL_CONTEXT
#define FSU_TOOL_CONTEXT

#include <stdbool.h>
#include <stdio.h>
#include "fat32.h"

/**
 * Global state which is available over the entire application.
 */
struct tool_context {
    /**
     * The file descriptor of the currently open disk image file.
     */
    int image_fd;  

    /**
     * A C string containing the name of the disk image file which is currently open.
     * This is the simple name of the file, not an absolute or relative path to the file.
     */
    char image_file_name[FILENAME_MAX + 1];

    struct bpb bpb;

    /**
     * The fully qualified absolute path of the current working directory (inside of the open disk image).
     * Not to be confused with the actual current working directory which the application was run in.
     */ 
    char* cwd;
};

void set_context_cwd(struct tool_context* context, char* new_cwd);

/**
 * Context which is made available to all REPL command handlers.
 */
struct command_context {
    struct tool_context* tool_context;
    int arg_count;
    char** args;
};
#endif