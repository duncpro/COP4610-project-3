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

unsigned int initial_data_sector(struct bpb bpb) {
    return bpb.total_reserved_sectors /* + (bpb.total_fats * bpb.fat_size) */;
}

unsigned int initial_data_sector_in_cluster(struct bpb bpb, unsigned int cluster_id) {
    return ((cluster_id - 2) * bpb.sectors_per_cluster) + initial_data_sector(bpb);
}

unsigned int next_cluster_id(unsigned int current_cluster_id, int image_fd, struct bpb bpb) {
    unsigned int fat_offset = (current_cluster_id * 4);
    unsigned int current_fat_sec_id = bpb.total_reserved_sectors + (fat_offset / bpb.bytes_per_sector);
    unsigned int entry_offset = fat_offset % bpb.bytes_per_sector;
    return read_unisgned_int(image_fd, (entry_offset + current_fat_sec_id) * bpb.bytes_per_sector, 4);
}

struct cluster_list scan_fat(struct bpb bpb, unsigned int start_at_cluster_id, int image_fd) {
    struct cluster_list cluster_list = {
        .total_clusters = 0,
        .cluster_ids = malloc(0)
    };
    unsigned cluster_id = start_at_cluster_id;
    while (cluster_id > 0x0000002 && cluster_id < 0xFFFFFF6) {
        cluster_list.cluster_ids = realloc(cluster_list.cluster_ids, sizeof(unsigned int) * cluster_list.total_clusters + 1);
        cluster_list.cluster_ids[cluster_list.total_clusters] = cluster_id;
        cluster_list.total_clusters++;
        cluster_id = next_cluster_id(cluster_id, image_fd, bpb);
    }
    return cluster_list;
}

struct directory read_directory(struct bpb bpb, unsigned int initial_dir_cluster_id, int image_fd) {
    struct directory dir = {
        .total_entries = 0,
        .entries = malloc(0)
    };

    struct cluster_list cluster_list = scan_fat(bpb, initial_data_sector_in_cluster(bpb, bpb.root_cluster_id), image_fd);
    printf("total clusters: %i\n", cluster_list.total_clusters);

    for (int i = 0; i < cluster_list.total_clusters; i++) {
        unsigned int dir_sector = initial_data_sector_in_cluster(bpb, cluster_list.cluster_ids[i]);
        int local_entry_id = 0;
        bool is_last_entry = read_unisgned_int(image_fd, dir_sector + (local_entry_id * 32), 4) == 0x00;
        while (!is_last_entry) {
            local_entry_id++;
            dir.total_entries++;
            is_last_entry = read_unisgned_int(image_fd, dir_sector + (local_entry_id * 32), 4) == 0x00;
        }
    }

    return dir;
}

void free_directory(struct directory dir) {
    free(dir.entries);
}