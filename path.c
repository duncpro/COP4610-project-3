#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fat32.h"
#include "path.h"
#include "string_utils.h"

void filename_from_path_string(char* path, char* filename) {
    int j = 0;
    for (int i = index_after_last('/', path); i < strlen(path); i++) {
        filename[j++] = path[i];
    }
    filename[j] = '\0';
}

struct fat_path* parse_path(char* str) {
    struct fat_path* p = (struct fat_path*) malloc(sizeof(struct fat_path));
    p->total_segments = 0;
    p->segments = (char**) malloc(0);

    char* path_segment = (char*) malloc(sizeof(char) * (FAT_EXTENSION_LENGTH + FAT_FILENAME_LENGTH + 1));
    path_segment[0] = '\0';
    
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] != '/') {
            int length = strlen(path_segment);
            path_segment[length] = str[i];
            path_segment[length + 1] = '\0';
        }

        if (str[i] == '/' || (i == strlen(str) - 1)) {
            if (strlen(path_segment) == 0) continue;
            p->segments = (char**) realloc(p->segments, sizeof(char*) * (p->total_segments + 1));
            p->segments[p->total_segments] = (char*) malloc(sizeof(char) * (strlen(path_segment) + 1));
            strcpy(p->segments[p->total_segments], path_segment);
            path_segment[0] = '\0'; 
            p->total_segments++;  
        }
    }
    free(path_segment);
    return p;
}

void free_path(struct fat_path* path) {
    for (int i = 0; i < path->total_segments; i++) {
        free(path->segments[i]);
    }
    free(path->segments);
    free(path);
}

struct fat_path* as_absolute_path(struct fat_path relative_path, struct fat_path absolute_root) {
    struct fat_path* new_path = (struct fat_path*) malloc(sizeof(struct fat_path));

    // Copy the absolute path into the new absolute path as the prefix.
    // The new copy is completely independent of the given absolute root in terms of memory allocation.
    new_path->total_segments = absolute_root.total_segments;
    new_path->segments = (char**) malloc(sizeof(char*) * absolute_root.total_segments);
    for (int i = 0; i < absolute_root.total_segments; i++) {
        char* original_segment = absolute_root.segments[i];
        new_path->segments[i] = (char*) malloc(sizeof(char) * (strlen(original_segment) + 1));
        strcpy(new_path->segments[i], original_segment);
    }

    for (int i = 0; i < relative_path.total_segments; i++) {
        char* segment = relative_path.segments[i];
        if (strcmp(segment, ".") == 0) continue;
        if (strcmp(segment, "..") == 0) {
            if (new_path->total_segments > 0) new_path->total_segments--;
            if (new_path->total_segments == 0) continue;
            free(new_path->segments[new_path->total_segments]);
            continue;
        }
        new_path->segments = (char**) realloc(new_path->segments, sizeof(char*) * (new_path->total_segments + 1));
        new_path->segments[new_path->total_segments] = (char*) malloc(sizeof(char) * (strlen(segment) + 1));
        strcpy(new_path->segments[new_path->total_segments], segment);
        new_path->total_segments++;
    }

    return new_path;
}

char* as_path_string(struct fat_path path) {
    char* str = (char*) malloc(sizeof(char) * (1 /* null terminator */ + (path.total_segments * (FAT_FILENAME_LENGTH + FAT_EXTENSION_LENGTH + 1 /* slash */))));
    for (int i = 0; i < path.total_segments; i++) {
        if (i != 0) strcat(str, "/");
        strcat(str, path.segments[i]);
    }
    return str;
}