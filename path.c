#include <string.h>
#include <stdio.h>
#include "path.h"
#include "string_utils.h"

void filename_from_path(char* path, char* filename) {
    int j = 0;
    for (int i = index_after_last('/', path); i < strlen(path); i++) {
        filename[j++] = path[i];
    }
    filename[j] = '\0';
}