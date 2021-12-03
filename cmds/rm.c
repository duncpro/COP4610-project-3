#include "./../fat32.h"
#include "./../tool_context.h"
#include "./../path.h"
#include <stdio.h>
#include <stdlib.h>

void rm_cmd(struct command_context context) {
    if (context.arg_count != 1) {
        printf("Invalid Usage: Expected exactly one argument, the relative path to the file to delete.\n");
        return;
    }
    struct fat_path* path = create_absolute_path_from_strs(context.args[0], context.tool_context->cwd);
    struct directory_entry* entry = get_entry_by_absolute_path(*path, context.tool_context->image_fd, context.tool_context->bpb);
    free_path(path);
    if (entry == NULL) {
        printf("The specified relative path does not lead to a file.\n");
        return;
    }
    if (is_directory(*entry)) {
        printf("The specified relative path does not lead to file, but a directory.\n");
        printf("The rm command does not support deletion of directories, only files.\n");
        free(entry);
        return;
    }
    delete_file(context.tool_context->bpb, context.tool_context->image_fd, *entry);
    free(entry);
}