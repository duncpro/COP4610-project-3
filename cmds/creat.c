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

    char filename[FILENAME_MAX + 1];
    filename[0] = '\0';
    char extension[FAT_EXTENSION_LENGTH + 1];
    extension[0] = '\0';
    int part = 0;
    for (int i = 0; i < strlen(context.args[0]); i++) {
        if (context.args[0][i] == '.') {
            part = 1;
            continue;
        }
        if (part == 0) {
            filename[strlen(filename)] = context.args[0][i];
            filename[strlen(filename)] = '\0';
            if (strlen(filename) > FAT_FILENAME_LENGTH) {
                printf("Unsupported Filename Length: FAT filesystem names may be up to a maximum of 8 characters long.\n");
                return;
            }
        }
        if (part == 1) {
            extension[strlen(extension)] = context.args[0][i];
            extension[strlen(extension)] = '\0';

            if (strlen(extension) > FAT_EXTENSION_LENGTH) {
                printf("Unsupported Extension Length: FAT filesystem extensions may be up to a maximum of three characters long.\n");
                return;
            }
        }
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

    bool created = create_file(context.tool_context->bpb, context.tool_context->image_fd, cluster_id, filename, extension);

    if (created) {
        printf("Created new file: \"%s\" in current working directory.\n", context.args[0]);
    } else {
        printf("Failed to create new file.\n");
    }
}