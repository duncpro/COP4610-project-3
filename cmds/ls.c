#include "./../fat32.h"
#include "./../tool_context.h"
#include "./../path.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void ls_cmd(struct command_context context) {
    uint32_t cwd_cluster_id;

    // Determine cluster id of the current working directory
    struct fat_path* cwd_path = parse_path(context.tool_context->cwd);
    struct fat_path* target_dir_path;
    if (context.arg_count == 1) {
        struct fat_path* sub_dir_path = parse_path(context.args[0]);
        target_dir_path = as_absolute_path(*sub_dir_path, *cwd_path);
        free_path(sub_dir_path);
        free_path(cwd_path);
    } else {
        target_dir_path = cwd_path;
    }
    if (target_dir_path->total_segments == 0) {
        cwd_cluster_id = context.tool_context->bpb.root_cluster_id;
    } else {
        char* target_dir_path_str = as_path_string(*target_dir_path);
        struct directory_entry* dir_entry = find_directory_entry(context.tool_context->bpb, context.tool_context->image_fd, target_dir_path_str);
        free(target_dir_path_str);
        
        if (dir_entry == NULL) {
            printf("Warning: The working directory does not point to a real directory entry.\n");
            printf("The directory does not exist therefore no files exist in the current directory\n");
            return;
        }
        if (!is_directory(*dir_entry)) {
            printf("Warning: The current path points to a file not an actual directory.\n");
            printf("Files contain no discernable subfiles therefore there are no children at the current path.y\n");
            free(dir_entry);
            free_path(target_dir_path);
            return;
        }
        cwd_cluster_id = dir_entry->cluster_id;
        free(dir_entry);
    }
    
    
    struct directory dir = read_directory(context.tool_context->bpb, cwd_cluster_id, context.tool_context->image_fd);
    printf("total entries: %i\n", dir.total_entries);
    for (int i = 0; i < dir.total_entries; i++) {
        struct directory_entry entry = dir.entries[i];
        printf("%s%s\n", entry.file_name, is_directory(entry) ? " (d)" : entry.extension);
    }
    printf("(d) signifies a sub-directory in the current directory, not a file.\n");
    free_directory(dir);
}