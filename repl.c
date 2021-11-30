#include "repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void repl() {
    run_repl = true;

    while (run_repl) {
        char* line_buffer = NULL;
        int line_buffer_size;
        int line_length = getline(line, &line_buffer_size, stdin);
        free(line_buffer);
    }

}

char* action(struct command* cmd) {
    return cmd->words[0];
}

struct command parse_command(char* str) {
    char* word = strtok(str, " ");

    int word_count = 0;
    for (int i = 0; i < strlen(word); i++) {
        if (word[i] == ' ') {
            
        }
    }

    while (word != NULL) {

        word = strtok(str, " ");
    }

}