#include "fat32.h"
#include "string_utils.h"
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int ith_bit(unsigned char byte, unsigned int i) {
    return (byte >> i) & 1;
}

unsigned int little_endian_unsigned_int(unsigned char* bytes, unsigned int bytes_length) {
    int sum = 0;
    if (bytes_length > sizeof(unsigned int)) {
        printf("The length of the integer byte array representation exceeds that which can be stored in C Langage Integer");
        return -1;
    }
    for (int i = 0; i < sizeof(unsigned int); i++) {
        if (i >= bytes_length) break;
        int byte_significance = 8 * i;
        sum += (bytes[i] << byte_significance);
    }
    return sum;
}

unsigned int read_unisgned_int(int fd, unsigned int field, unsigned int field_length) {
    unsigned char buffer[field_length];
    int total_read = pread(fd, buffer, field_length, field);
    if (total_read < field_length) {
        printf("Malformed FAT 32 image.\n");
        printf("Expected the BPB to contain a required field but it did not.\n");
        return -1;
    }
    return little_endian_unsigned_int(buffer, total_read);
}

struct bpb read_bpb(int fd) {
    struct bpb bpb = {
        .bytes_per_sector = read_unisgned_int(fd, 11, 2),
        .sectors_per_cluster = read_unisgned_int(fd, 13, 1),
        .total_reserved_sectors = read_unisgned_int(fd, 14, 2),
        .total_fats = read_unisgned_int(fd, 16, 1),
        .total_sectors = read_unisgned_int(fd, 32, 4),
        .fat_size = read_unisgned_int(fd, 36, 2),
        .root_cluster_id = read_unisgned_int(fd, 44, 4)
    };
    return bpb;
}

bool is_directory(struct directory_entry entry) {
    return ith_bit(entry.attributes, 0x10) == 1;
}