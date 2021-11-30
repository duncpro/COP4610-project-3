#include <string.h>

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