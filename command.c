#include "command.h"
#include "string_utils.h"
#include <stdlib.h>
#include <string.h>

char* action(struct command cmd) {
    if (cmd.total_words == 0) return NULL;
    return cmd.words[0];
}

char** args(struct command cmd) {
    if (cmd.total_words <= 1) return NULL;
    return &(cmd.words[1]);
}

struct command parse_command(char* str) {
    struct command cmd;

    cmd.raw = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(cmd.raw, str);

    if (strlen(cmd.raw) == 0) {
        cmd.total_words = 0;
        cmd.words = NULL;
        return cmd;
    }

    cmd.total_words = 1 + count(' ', cmd.raw);

    // Make room for null terminators inserted by strtok.
    cmd.raw = realloc(cmd.raw, sizeof(char) * (strlen(cmd.raw) + (1 + (cmd.total_words - 1))));

    cmd.words = malloc(sizeof(char*) * cmd.total_words);

    char* word = strtok(cmd.raw, " ");
    cmd.words[0] = word;

    int i = 1;
    while (i < cmd.total_words) {
        word = strtok(NULL, " ");
        cmd.words[i] = word;
        i++;
    }

    return cmd;
}

void free_command(struct command command) {
    free(command.raw);
    free(command.words);
}