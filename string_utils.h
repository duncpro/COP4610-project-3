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

int count(char c, char* in);

#endif