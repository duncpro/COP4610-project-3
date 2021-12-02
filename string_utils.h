#ifndef FSU_STRING_UTILS
#define FSU_STRING_UTILS

/**
 * Returns a pointer to the given string, offset by the number of leading spaces within the string.
 */
char* trim_leading(char* str);

/**
 * Inserts a null terminator after the last non whitespace character in the string.
 */
void trim_trailing(char* str);

/**
 * Composition of trim_leading and trim_trailing.
 * See the aforementioned function's documentation for a description of the behavior of this function.
 */ 
char* trim(char* str);

/**
 * Returns the index after the rightmost character matching c.
 * If no characters match the given character then zero is returned instead.
 */
int index_after_last(char c, char* in);

int count(char c, char* in);

void remove_all(char c, char* in);

#endif