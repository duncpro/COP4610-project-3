#include "./../tool_context.h"
#include "./../fat32.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void size_cmd(struct command_context context) {
    if (context.arg_count != 1) {
        printf("Invalid Usage: Expected a single argument, the path to the file.\n");
        return;
    }

    // The absolute path of the file whose size the user is requesting.
    char absolute_path[strlen(context.args[0]) + strlen(context.tool_context->cwd)];
    absolute_path[0] = '\0';
    strcat(absolute_path, context.tool_context->cwd);
    strcat(absolute_path, context.args[0]);


    struct directory_entry* entry = find_directory_entry(context.tool_context->bpb, context.tool_context->image_fd, absolute_path);
    if (entry == NULL) {
        printf("The given absolute path does not lead to a real directory entry.\n");
        return;
    }

    if (is_directory(*entry)) {
        printf("Warning: The given absolute path leads to a directory not a file. \n");
        printf("The \"size <absolute path>\" command is intended for use exclusively with files, not directories.\n");
        printf("The file size of a directory is always zero.\n");
    }

    printf("File size: %i bytes\n", entry->file_size);
    
    free(entry);
}