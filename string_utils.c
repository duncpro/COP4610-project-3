#include <string.h>
#include "string_utils.h"

char* trim_leading(char* str) {
    int total_spaces = 0;
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] != ' ') break;
        total_spaces++;
    }
    return str + (sizeof(char) * total_spaces);
}

void trim_trailing(char* str) {
    int total_spaces = 0;
    for (int i = strlen(str) - 1; i >= 0; i--) {
        if (str[i] != ' ') break;
        total_spaces++;
    }
    str[strlen(str) - total_spaces] = '\0';
}

char* trim(char* str) {
    trim_trailing(str);
    return trim_leading(str);
}

int count(char c, char* in) {
    int count = 0;
    for (int i = 0; i < strlen(in); i++) {
        if (in[i] == ' ') count++;
    }
    return count;
}

void remove_all(char c, char* in) {
    char new[strlen(in) + 1 - count(c, in)];
    int new_length = 0;
    strcpy(new, in);
    for (int i = 0; i < strlen(in); i++) {
        if (in[i] == c) continue;
        new[new_length] = in[i];
        new_length++;
    }
    new[new_length] = '\0';
    strcpy(in, new);
}


int index_after_last(char c, char* in) {
    for (int i = strlen(in) - 1; i >= 0; i--) {
        if (in[i] == c) return i + 1;
    }
    return 0;
}