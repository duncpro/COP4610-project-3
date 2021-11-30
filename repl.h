#ifndef FSU_REPL
#define FSU_REPL

bool run_repl;

struct command {
    char** words;
    int total_words;
}

/**
 * Returns the action associated with the given command.
 * If no action is associated with the command, then a null pointer is returned instead.
 */
char* action(struct command* cmd);

void dispatch(struct command* cmd);

struct command parse_command(char* str);

#endif