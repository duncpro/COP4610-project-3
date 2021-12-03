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

    char filename[FAT_FILENAME_LENGTH + 1];
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
            if (strlen(filename) > FAT_FILENAME_LENGTH) {
                printf("Unsupported Filename Length: FAT filesystem names may be up to a maximum of 8 characters long.\n");
                return;
            }
            int len = strlen(filename);
            filename[len] = context.args[0][i];
            filename[len + 1] = '\0';
        }
        if (part == 1) {
            if (strlen(extension) > FAT_EXTENSION_LENGTH) {
                printf("Unsupported Extension Length: FAT filesystem extensions may be up to a maximum of three characters long.\n");
                return;
            }
            int len = strlen(extension);
            extension[len] = context.args[0][i];
            extension[len + 1] = '\0';
        }
    }

    // Figure out the cluster id of the directory which contains this new file.
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

    // Make sure the directory does not already contain a file with that name.
    {
        struct directory dir = read_directory(context.tool_context->bpb, cluster_id, context.tool_context->image_fd);

        bool conflict = false;
        for (int i = 0; i < dir.total_entries; i++) {
            struct directory_entry entry = dir.entries[i];
            if ((strcmp(entry.file_name, filename) == 0) && (strcmp(entry.extension, extension) == 0)) {
                conflict = true;
                break;
            }
        }
        if (conflict) {
            printf("A file with that name already exists within the current directory.\n");
            return;
        }
    }

    bool created = create_file(context.tool_context->bpb, context.tool_context->image_fd, cluster_id, filename, extension);

    if (created) {
        printf("Created new file: \"%s\" in current working directory.\n", context.args[0]);
    } else {
        printf("Failed to create new file.\n");
    }
}