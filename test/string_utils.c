#include "./../string_utils.c"
#include "test_tools.c"

void test_trim_leading_noop() {
    char* str = "hello gwen";

    assert_str_equals(str, trim_leading(str), "trim_leading (noop)");
}

void test_trim_leading_single() {
    char* str = " hello gwen";
    char* expected = "hello gwen";

    assert_str_equals(expected, trim_leading(str), "trim_leading (single)");
}

void test_trim_leading_general() {
    char* str = "  hello gwen";
    char* expected = "hello gwen";
    assert_str_equals(expected, trim_leading(str), "trim leading (general)");
}

void test_trim_trailing_noop() {
    char* str = "hello gwen";
    char* mutable_str = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(mutable_str, str);
    trim_trailing(mutable_str);
    assert_str_equals(mutable_str, mutable_str, "trim trailing (noop)");
}

void test_trim_trailing_single() {
    char* str = "hello gwen ";
    char* expected = "hello gwen";

    char* mutable_str = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(mutable_str, str);
    trim_trailing(mutable_str);

    assert_str_equals(expected, mutable_str, "trim trailing (single)");
}

void test_trim_trailing_general() {
    char* str = "hello gwen  ";
    char* expected = "hello gwen";

    char* mutable_str = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(mutable_str, str);
    trim_trailing(mutable_str);

    assert_str_equals(expected, mutable_str, "trim trailing (general)");
}

void smoke_test_trim() {
    char* str = " hello gwen ";
    char* expected = "hello gwen";

    char* mutable_str = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(mutable_str, str);
    
    assert_str_equals(expected, trim(mutable_str), "trim (smoke)");
}

int main() {
    test_trim_leading_noop();
    test_trim_leading_single();
    test_trim_leading_general();
    test_trim_trailing_noop();
    test_trim_trailing_single();
    test_trim_trailing_general();
    smoke_test_trim();
    return 0;
}