#include "./../command.c"
#include "./../string_utils.c"
#include "test_tools.c"

void test_parse_command_general() {
    char* command_str = "echo hello";
    struct command command = parse_command(command_str);
    assert_str_equals("echo", command.words[0], "parse_command (general, first arg)");
    assert_str_equals("hello", command.words[1], "parse_command (general, second arg)");
    assert_int_equals(2, command.total_words, "parse_command (general, total words)");
    free_command(command);
}

void test_parse_command_empty() {
    char* command_str = "";
    struct command command = parse_command(command_str);
    assert_int_equals(0, command.total_words, "parse_command (empty, total_words)");
    assert_true(NULL == command.words, "parse_command (empty, words)");
}

int main() {
    // test_parse_command_general();
    test_parse_command_empty();
    return 0;
}