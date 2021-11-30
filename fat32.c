#include "fat32.h"
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int ith_bit(unsigned char byte, int i) {
    return (byte >> i) & 1;
}

int little_endian_int(unsigned char* bytes, int bytes_length) {
    int sum = 0;
    if (bytes_length > sizeof(int)) {
        printf("The length of the integer byte array representation exceeds that which can be stored in C Langage Integer");
        exit(1);
    }
    for (int i = 0; i < sizeof(int); i++) {
        if (i >= bytes_length) break;
        int byte_significance = 8 * i;
        sum += (bytes[i] << byte_significance);
    }
    return sum;
}

int read_bpb_field_int(int fd, int field, int field_length) {
    unsigned char buffer[field_length];
    int total_read = pread(fd, buffer, field_length, field);
    if (total_read < field_length) {
        printf("Malformed FAT 32 image.\n");
        printf("Expected the BPB to contain a required field but it did not.\n");
        exit(1);
    }
    return little_endian_int(buffer, total_read);
}

struct bpb read_bpb(int fd) {
    struct bpb bpb = {
        .bytes_per_sector = read_bpb_field_int(fd, 11, 2),
        .sectors_per_cluster = read_bpb_field_int(fd, 13, 1),
        .root_cluster_position = read_bpb_field_int(fd, 44, 4)
    };
    return bpb;
}