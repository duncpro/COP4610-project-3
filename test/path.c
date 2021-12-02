#include "./../path.c"
#include "test_tools.c"
#include "./../string_utils.c"

void test_parse_path_empty() {
    struct fat_path* p = parse_path("");
    assert_int_equals(0, p->total_segments, "parse_path (empty)");
    free_path(p);
}

void test_parse_path_single_slash() {
    struct fat_path* p = parse_path("/");
    assert_int_equals(0, p->total_segments, "parse_path (single slash)");
    free_path(p);
}

void test_parse_path_general() {
    struct fat_path* p = parse_path("/hello/world");
    assert_int_equals(p->total_segments, 2, "parse_path (general)");
    assert_str_equals("hello", p->segments[0], "parse_path (general) (segments, 0)");
    assert_str_equals("world", p->segments[1], "parse_path (general) (segments, 1)");
    free_path(p);
}

int main() {
    test_parse_path_empty();
    test_parse_path_single_slash();
    test_parse_path_general();
    return 0;
}