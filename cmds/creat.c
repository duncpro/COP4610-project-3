#include "./../tool_context.h"
#include "./../fat32.h"
#include "./../path.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void creat_cmd(struct command_context context) {
    if (context.arg_count != 1) {
        printf("Invalid Usage: Expected a filename as the only argument.\n");
        return;
    }

    uint32_t cluster_id;

    bool is_root_dir = (strcmp(context.tool_context->cwd, "") == 0);
    if (is_root_dir) {
        cluster_id = context.tool_context->bpb.root_cluster_id;
    } else {
        struct directory_entry* entry = get_entry_by_absolute_path_string(context.tool_context->bpb, context.tool_context->image_fd, context.tool_context->cwd);
        if (entry == NULL) {
            printf("The current working directory does not exist, therefore a file cannot be created within it.\n");
            free(entry);
            return;
        }
        cluster_id = entry->cluster_id;
        free(entry);
    }


    create_file(context.tool_context->bpb, context.tool_context->image_fd, cluster_id, context.args[0], "");
    printf("Created new file: %s\n", context.args[0]);
}