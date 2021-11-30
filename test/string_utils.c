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
    char* str = mutable_str("hello gwen");
    trim_trailing(str);
    assert_str_equals(str, str, "trim trailing (noop)");
    free(str);
}

void test_trim_trailing_single() {
    char* str = mutable_str("hello gwen ");
    char* expected = "hello gwen";

    trim_trailing(str);

    assert_str_equals(expected, str, "trim trailing (single)");
    free(str);
}

void test_trim_trailing_general() {
    char* str = mutable_str("hello gwen  ");
    char* expected = "hello gwen";

    trim_trailing(str);

    assert_str_equals(expected, str, "trim trailing (general)");
    free(str);
}

void test_trim_general() {
    char* str = mutable_str(" hello gwen ");
    char* expected = "hello gwen";
    
    assert_str_equals(expected, trim(str), "trim (general)");
    free(str);
}

void test_trim_empty() {
    char* str = mutable_str("");
    char* expected = "";
    
    assert_str_equals(expected, trim(str), "trim (empty)");
    free(str);
}

void test_count_empty() {
    char* str = "";
    int expected = 0;
    int actual = count(' ', str);
    assert_int_equals(expected, actual, "count (empty)");
}

void test_count_general() {
    char* str = " ";
    int expected = 1;
    int actual = count(' ', str);
    assert_int_equals(expected, actual, "count (general)");
}

void test_remove_all_empty() {
    char* str = mutable_str("");
    remove_all('a', str);
    assert_str_equals("", str, "remove_all (empty)");
    free(str);
}

void test_remove_all_general() {
    char* str = mutable_str("hello world");
    remove_all('l', str);
    assert_str_equals("heo word", str, "remove_all (general)");
    free(str);
}

int main() {
    test_trim_leading_noop();
    test_trim_leading_single();
    test_trim_leading_general();
    test_trim_trailing_noop();
    test_trim_trailing_single();
    test_trim_trailing_general();
    test_trim_general();
    test_count_empty();
    test_count_general();
    test_remove_all_empty();
    test_remove_all_general();
    test_trim_empty();
    return 0;
}