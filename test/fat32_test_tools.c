#include "test_tools.c"

void assert_real_sectors_per_cluster_val(int actual, char* test_name) {
    bool is_real;
    switch (actual) {
    case 1:
    case 2:
    case 4:
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
        is_real = true;
        break;
    default:
        is_real = false;
        break;
    }
    assert_true(is_real, test_name);
}

void assert_real_bytes_per_sector_val(int actual, char* test_name) {
    bool is_real;
    switch (actual) {
    case 512:
    case 1024:
    case 2048:
    case 4096:
        is_real = true;
        break;
    default:
        is_real = false;
        break;
    }
    assert_true(is_real, test_name);
}