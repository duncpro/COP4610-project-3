#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "path.h"
#include "fat32.h"
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

    char* path_segment = (char*) malloc(sizeof(char) * (FAT_EXTENSION_LENGTH + FAT_MAX_FILE_NAME + 1));
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