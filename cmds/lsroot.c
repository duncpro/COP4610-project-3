#include "./../fat32.h"
#include "./../tool_context.h"
#include <stdio.h>



void lsroot_cmd(struct command_context context) {
    struct directory dir = read_directory(context.tool_context->bpb, context.tool_context->bpb.root_cluster_id, context.tool_context->image_fd);
    printf("total entries: %i\n", dir.total_entries);
    for (int i = 0; i < dir.total_entries; i++) {
        struct directory_entry entry = dir.entries[i];
        printf("%s %s\n", entry.file_name, is_directory(entry) ? "(d)" : "");
    }
    free_directory(dir);
}