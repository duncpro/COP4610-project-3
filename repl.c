#include "repl.h"
#include "string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void repl() {
    continue_repl = true;
    printf("FAT32 Tools Started\n");
 
    while (continue_repl) {
        printf("Waiting for input... Enter a command now.\n");
        char* line_buffer = NULL;
        size_t line_buffer_size;
        int line_length = getline(&line_buffer, &line_buffer_size, stdin);
        remove_all('\n', line_buffer);
        dispatch(parse_command(trim(line_buffer)));
        free(line_buffer);
    }

}

char* action(struct command cmd) {
    if (cmd.total_words == 0) return NULL;
    return cmd.words[0];
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

void dispatch(struct command cmd) {
    if (action(cmd) == NULL) return;
    if (strcmp(action(cmd), "exit") == 0) continue_repl = false;
}