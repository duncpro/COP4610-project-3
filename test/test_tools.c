#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool assert_true(bool condition, char* test_name) {
    printf("%s: %s\n", test_name, condition ? "PASSED" : "FAILED");
    return condition;
}

void assert_int_equals(int expected, int actual, char* test_name) {
    bool passed = assert_true(expected == actual, test_name);

    if (!passed) {
        printf("Expected: %i\n", expected);
        printf("Actual: %i\n", actual);
    }
}

void assert_str_equals(char* expected, char* actual, char* test_name) {
    bool passed = assert_true(strcmp(expected, actual) == 0, test_name);
    
    if (!passed) {
        printf("Expected: %s\n", expected);
        printf("Actual: %s\n", actual);
    }
}

char* mutable_str(char* string_literal) {
    char* new_str = malloc(sizeof(char) * (strlen(string_literal) + 1));
    strcpy(new_str, string_literal);
    return new_str;
}