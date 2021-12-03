#include "./../tool_context.h"
#include "./../path.h"
#include "./../fat32.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cd_cmd(struct command_context context) {
    if (context.arg_count != 1) {
        printf("Invalid Usage: Expected relative or absolute path as only argument for CD command.\n");
    }
    
    // Find the absolute path of the target directory.
    // TODO: This could probably be made shorter.
    struct fat_path* root = parse_path(context.tool_context->cwd);
    struct fat_path* relative = parse_path(context.args[0]);
    struct fat_path* absolute_target = as_absolute_path(*relative, *root);
    char* absolute_target_path_str = as_path_string(*absolute_target);
    free_path(root);
    free_path(relative);
    free_path(absolute_target);

    struct directory_entry* dir_entry = find_directory_entry(context.tool_context->bpb, context.tool_context->image_fd, absolute_target_path_str);

    if (dir_entry == NULL) {
        bool is_pointing_to_fat_root_dir = (strcmp(absolute_target_path_str, "") == 0);
        if (is_pointing_to_fat_root_dir) {
            set_context_cwd(context.tool_context, "");
        }

        if (!is_pointing_to_fat_root_dir) {
            printf("The given relative path does not point to a directory, in fact, it doesn't point to anything.\n");
        }

       
        free(dir_entry);
        free(absolute_target_path_str);
        return;
    }

    if (!is_directory(*dir_entry)) {
        printf("The given relative path does not point to a directory, but instead a file.\n");
    };

    if (is_directory(*dir_entry)) {
        set_context_cwd(context.tool_context, absolute_target_path_str);
    }

    free(dir_entry);
    free(absolute_target_path_str);
}