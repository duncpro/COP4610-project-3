#include "./../fat32.c"
#include "./fat32_test_tools.c"
#include "./../string_utils.c"
#include "./../path.c"
#include <fcntl.h>
#include <unistd.h>

void test_little_endian_int_general_case() {
    unsigned char bytes[] = {0x01, 0x00};
    int expected = 1;
    int actual = little_endian_unsigned_int(bytes, 2);
    assert_int_equals(expected, actual, "little_endian_int (general)");
}

void test_combine_ints() {
    uint16_t low = 1;
    uint16_t high = 1;
    int expected = 65537;
    assert_int_equals(expected, combine_ints(high, low), "combine_ints (general)");
}

void test_little_endian_int_single_case() {
    unsigned char bytes[] = {0x01};
    int expected = 1;
    int actual = little_endian_unsigned_int(bytes, 1);
    assert_int_equals(expected, actual, "little_endian_int (single byte)");
}

void test_read_bpb() {
    int image_fd = open("./assets/canvas_example.img", O_RDONLY);
    struct bpb bpb = read_bpb(image_fd);
    assert_real_bytes_per_sector_val(bpb.bytes_per_sector, "read_pbp (bytes per sector)");
    assert_real_sectors_per_cluster_val(bpb.sectors_per_cluster, "read_pbp (sectors per cluster)");
    close(image_fd);
}

void test_ith_bit() {
    unsigned char byte = 2;
    
    bool first_bit_correct = (ith_bit(byte, 0) == 0);
    bool second_bit_correct = (ith_bit(byte, 1) == 1);

    assert_true(first_bit_correct && second_bit_correct, "ith_bit");
}

int main() {
    test_little_endian_int_general_case();
    test_little_endian_int_single_case();
    test_read_bpb();
    test_ith_bit();
    test_combine_ints();
    return 0;
}