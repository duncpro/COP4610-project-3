#include "./../tool_context.h"
#include "./../path.h"
#include "./../fat32.h"
#include <stdio.h>

void cd_cmd(struct command_context context) {
    if (context.arg_count != 1) {
        printf("Invalid Usage: Expected relative or absolute path as only argument for CD command.\n");
    }
    
    // Find the absolute path of the target directory
    struct fat_path* root = parse_path(context.tool_context->cwd);
    struct fat_path* relative = parse_path(context.args[0]);
    struct fat_path* absolute_target = as_absolute_path(*relative, *root);
    free_path(root);
    free_path(relative);

    find_directory_entry(context.tool_context->bpb, context.tool_context->image_fd, )

    free_path(absolute_target);
}