#ifndef FSU_REPL
#define FSU_REPL

#include <stdbool.h>

bool continue_repl;

struct command {
    char** words;
    int total_words;
    char* raw;
};

/**
 * Returns the action associated with the given command.
 * If no action is associated with the command, then a null pointer is returned instead.
 */
char* action(struct command cmd);

void dispatch(struct command cmd);

/**
 * Parses the given string into a command.
 * The returned command struct contains dynamically allocated memory which must be freed 
 * at the end of the object's lifecycle, use free_command() to satisfy this constraint.
 * 
 * This function expects the input string to be sanitized. That is, there should be no trailing and leading spaces
 * and any new line characters should be removed.
 */ 
struct command parse_command(char* str);

/**
 * Frees the dynamically allocated memory stored within the given command struct.
 * This function does not free the command struct itself, so, if it was dynamically allocated,
 * the caller is responsible for freeing that struct as well.
 */
void free_command(struct command command);

void repl();
#endif