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
    if (cwd_path->total_segments == 0) {
        cwd_cluster_id = context.tool_context->bpb.root_cluster_id;
    } else {
        struct directory_entry* dir_entry = find_directory_entry(context.tool_context->bpb, context.tool_context->image_fd, context.tool_context->cwd);
        if (dir_entry == NULL) {
            printf("Warning: The current working directory does not point to a real directory entry.\n");
            printf("The current directory does not exist therefore no files exist in the current directory\n");
            return;
        }
        if (!is_directory(*dir_entry)) {
            printf("Warning: The current working directory is set to a file not an actual directory.\n");
            printf("Files contain no discernable subfiles therefore there are no children at the path of the current directory\n");
            free(dir_entry);
            return;
        }
        cwd_cluster_id = dir_entry->cluster_id;
    }
    
    
    struct directory dir = read_directory(context.tool_context->bpb, context.tool_context->bpb.root_cluster_id, context.tool_context->image_fd);
    printf("total entries: %i\n", dir.total_entries);
    for (int i = 0; i < dir.total_entries; i++) {
        struct directory_entry entry = dir.entries[i];
        printf("%s%s\n", entry.file_name, is_directory(entry) ? " (d)" : entry.extension);
    }
    printf("(d) signifies a sub-directory in the current directory, not a file.\n");
    free_directory(dir);
}