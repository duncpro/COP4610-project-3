#include "repl.h"
#include "string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void repl() {
    run_repl = true;

    while (run_repl) {
        char** line_buffer = NULL;
        size_t line_buffer_size;
        int line_length = getline(line_buffer, &line_buffer_size, stdin);
        free(line_buffer);
    }

}

char* action(struct command* cmd) {
    return cmd->words[0];
}

struct command parse_command(char* potentially_immutable_str) {
    struct command cmd;

    char* mutable_str = malloc(sizeof(char) * (strlen(potentially_immutable_str) + 1));
    strcpy(mutable_str, potentially_immutable_str);
    cmd.raw = mutable_str;
    
    char* trimmed_str = trim(mutable_str);

    if (strlen(trimmed_str) == 0) {
        cmd.total_words = 0;
        cmd.words = NULL;
        return cmd;
    }

    cmd.total_words = 1 + count(' ', trimmed_str);

    // Make room for null terminators inserted by strtok.
    mutable_str = realloc(mutable_str, sizeof(char) * (strlen(mutable_str) + (1 + (cmd.total_words - 1))));
    cmd.raw = mutable_str;

    cmd.words = malloc(sizeof(char*) * cmd.total_words);

    char* word = strtok(trimmed_str, " ");
    cmd.words[0] = word;

    int i = 1;
    while (i < cmd.total_words) {
        word = strtok(NULL, " ");
        cmd.words[i] = word;
        i++;
    }

    return cmd;
}

void free_command(struct command* command) {
    free(command->raw);
    free(command->words);
}