#include "fat32.h"
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

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

unsigned long byte_position(struct bpb bpb, unsigned int sector_id) {
    return bpb.bytes_per_sector * sector_id;
}

unsigned int fat_entry_offset(unsigned int data_cluster_id) {
    return 4 /* bytes */ * data_cluster_id;
}

unsigned int fat_entry_sector_position(struct bpb bpb, unsigned int data_cluster_id) {
    return bpb.total_reserved_sectors + (fat_entry_offset(data_cluster_id) / bpb.bytes_per_sector);
}

unsigned long fat_entry_byte_position(struct bpb bpb, unsigned int data_cluster_id) {
    int intra_sector_offset = fat_entry_offset(data_cluster_id) % bpb.bytes_per_sector;
    return (fat_entry_sector_position(bpb, data_cluster_id) * bpb.bytes_per_sector) + intra_sector_offset;
}

bool is_final_fat_entry(unsigned int entry) {
    return entry == 0xFFFFFFFF || entry == 0xFFFFFF8 || entry == 0xFFFFFFE;
}

bool is_allocated_fat_entry(unsigned int entry, struct bpb bpb) {
    return entry > 0x0000002 && entry <= max_fat_entry(bpb);
}

bool is_unallocated_fat_entry(unsigned int entry) {
    return entry == 0x0000000;
}

bool is_faulty_fat_entry(unsigned int entry) {
    return entry == 0xFFFFFF7;
}

unsigned int total_data_sectors(struct bpb bpb) {
    return bpb.total_sectors - (bpb.total_reserved_sectors + (bpb.total_fats * bpb.fat_size));
}

unsigned int total_data_clusters(struct bpb bpb) {
    return total_data_sectors(bpb) / bpb.sectors_per_cluster;
}

unsigned int max_fat_entry(struct bpb bpb) {
    return total_data_clusters(bpb) + 1;
}

bool is_reserved_entry(unsigned int entry, struct bpb bpb) {
    return entry > max_fat_entry(bpb);
}

unsigned int next_fat_entry(struct bpb bpb, unsigned int current_entry, int image_fd) {
    unsigned int position = fat_entry_byte_position(bpb, current_entry);
    return read_unisgned_int(image_fd, position, 4);
}
 
unsigned int entry_chain_length(struct bpb bpb, unsigned int initial_entry, int image_fd) {
    if (is_unallocated_fat_entry(initial_entry) || is_faulty_fat_entry(initial_entry) || is_reserved_entry(initial_entry, bpb)) {
        return 0;
    }

    if (is_final_fat_entry(initial_entry)) return 1;

    int entry = initial_entry;
    int count = 1;
    while (is_allocated_fat_entry(entry, bpb)) {
        count++;
        entry = next_fat_entry(bpb, entry, image_fd);
    }

    return count;
}

// struct cluster_list scan_fat(struct bpb bpb, int initial_entry) {
//     struct cluster_list cluster_list;

//     if (is_unallocated_fat_entry(initial_entry) || is_faulty_fat_entry(initial_entry)) {
//         cluster_list.cluster_ids = NULL;
//         cluster_list.total_clusters = 0;
//         return cluster_list;
//     }

//     int entry = initial_entry;
//     cluster_list.
// }

// struct directory read_directory(struct bpb bpb, unsigned int initial_cluster_id) {
     
// }

// struct directory read_directory_segment(struct bpb bpb, unsigned int cluster_id) {

// }