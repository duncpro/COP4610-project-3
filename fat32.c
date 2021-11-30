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
        return -1;
    }
    for (int i = 0; i < sizeof(int); i++) {
        if (i >= bytes_length) break;
        int byte_significance = 8 * i;
        sum += (bytes[i] << byte_significance);
    }
    return sum;
}

int read_int(int fd, int field, int field_length) {
    unsigned char buffer[field_length];
    int total_read = pread(fd, buffer, field_length, field);
    if (total_read < field_length) {
        printf("Malformed FAT 32 image.\n");
        printf("Expected the BPB to contain a required field but it did not.\n");
        return -1;
    }
    return little_endian_int(buffer, total_read);
}

struct bpb read_bpb(int fd) {
    struct bpb bpb = {
        .bytes_per_sector = read_int(fd, 11, 2),
        .sectors_per_cluster = read_int(fd, 13, 1),
        .total_reserved_sectors = read_int(fd, 14, 2),
        .total_fats = read_int(fd, 16, 1),
        .total_sectors = read_int(fd, 32, 4),
        .fat_size = read_int(fd, 36, 2),
        .root_cluster_id = read_int(fd, 44, 4)
    };
    return bpb;
}

bool is_directory(struct directory_entry entry) {
    return ith_bit(entry.attributes, 0x10) == 1;
}

long byte_position(struct bpb bpb, int sector_id) {
    return bpb.bytes_per_sector * sector_id;
}

int fat_entry_offset(int data_cluster_id) {
    return 4 /* bytes */ * data_cluster_id;
}

int fat_entry_sector_position(struct bpb bpb, int fat_id, int data_cluster_id) {
    int fat_offset = fat_id * bpb.fat_size;
    return bpb.total_reserved_sectors + (fat_offset + (fat_entry_offset(data_cluster_id) / bpb.bytes_per_sector));
}

long fat_entry_byte_position(struct bpb bpb, int fat_id, int data_cluster_id) {
    int intra_sector_offset = fat_entry_offset(data_cluster_id) % bpb.bytes_per_sector;
    return (fat_entry_sector_position(bpb, data_cluster_id, fat_id) * bpb.bytes_per_sector) + intra_sector_offset;
}