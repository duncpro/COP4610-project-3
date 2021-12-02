#include "./../tool_context.h"
#include "./../fat32.h"
#include <stdio.h>
#include <stdlib.h>

void size_cmd(struct command_context context) {
    if (context.arg_count != 1) {
        printf("Invalid Usage: Expected a single argument, the path to the file.\n");
        return;
    }

    printf("%s\n", context.args[0]);

    struct directory_entry* entry = find_directory_entry(context.tool_context->bpb, context.tool_context->image_fd, context.args[0]);
    if (entry == NULL) {
        printf("The given absolute path does not lead to a real directory entry.\n");
        return;
    }

    printf("File size: %i\n", entry->file_size);
    
    free(entry);
}