#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

bool assertTrue(bool condition, char* test_name) {
    printf("%s: %s\n", test_name, condition ? "PASSED" : "FAILED");
    return condition;
}

void assertEquals(int expected, int actual, char* test_name) {
    bool passed = assertTrue(expected == actual, test_name);

    if (!passed) {
        printf("Expected: %i\n", expected);
        printf("Actual: %i\n", actual);
    }
}