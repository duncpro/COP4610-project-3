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

void test_parse_path_single_file() {
    struct fat_path* p = parse_path("hello");
    assert_int_equals(1, p->total_segments, "parse_path (single file)");
    assert_str_equals("hello", p->segments[0], "parse_path (single file, segments)");
    free_path(p);
}

void test_as_absolute_path() {
    struct fat_path* root = parse_path("hello");
    struct fat_path* relative = parse_path("world");
    struct fat_path* combined = as_absolute_path(*relative, *root);
    assert_int_equals(2, combined->total_segments, "as_absolute_path (general, total segments)");
    assert_str_equals("hello", combined->segments[0], "as_absolute_path (general, 1st segment)");
    assert_str_equals("world", combined->segments[1], "as_absolute_path (general, 2nd segment)");
    free_path(root);
    free_path(relative);
    free_path(combined);
}

void test_as_absolute_path_ascend_tree() {
    struct fat_path* root = parse_path("hello");
    struct fat_path* relative = parse_path("../");
    struct fat_path* combined = as_absolute_path(*relative, *root);
    assert_int_equals(0, combined->total_segments, "as_absolute_path (ascend tree, total segments)");
    free_path(root);
    free_path(relative);
    free_path(combined);
}

void test_as_path_str() {
    char* expected = "hello/world";
    struct fat_path* path = parse_path(expected);
    char* actual = as_path_string(*path);
    assert_str_equals(expected, actual, "as_path_str");
    free(actual);
    free_path(path);
}

int main() {
    test_parse_path_empty();
    test_parse_path_single_slash();
    test_parse_path_general();
    test_parse_path_single_file();
    test_as_absolute_path();
    test_as_absolute_path_ascend_tree();
    test_as_path_str();
    return 0;
}