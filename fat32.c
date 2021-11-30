#include "fat32.h"
#include <math.h>

int ith_bit(unsigned char byte, int i) {
    return (byte >> i) & 1;
}

int little_endian_int(unsigned char* bytes, int bytes_length) {
    int sum = 0;
    for (int i = 0; i < sizeof(int); i++) {
        int byte_significance = 8 * i;
        sum += (byte_significance * bytes[i]);
    }
    return sum;
}