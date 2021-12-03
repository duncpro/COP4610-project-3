#include "./../tool_context.h"
#include "./../fat32.h"
#include "./../path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void size_cmd(struct command_context context) {
    if (context.arg_count != 1) {
        printf("Invalid Usage: Expected a single argument, the path to the file.\n");
        return;
    }

    struct fat_path* abs_target_path = create_absolute_path_from_strs(context.args[0], context.tool_context->cwd);

    struct directory_entry* entry = get_entry_by_absolute_path(*abs_target_path, context.tool_context->image_fd, context.tool_context->bpb);
    free(abs_target_path);

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