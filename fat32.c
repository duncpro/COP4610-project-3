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

unsigned long byte_position(struct bpb bpb, unsigned int sector_id) {
    return bpb.bytes_per_sector * sector_id;
}

unsigned int fat_entry_offset(unsigned int data_cluster_id) {
    return 4 /* bytes */ * data_cluster_id;
}

unsigned int fat_entry_sector_position(struct bpb bpb, unsigned int data_cluster_id) {
    return bpb.total_reserved_sectors + (fat_entry_offset(data_cluster_id) / bpb.bytes_per_sector);
}

unsigned int intra_sector_offset(struct bpb bpb, unsigned int data_cluster_id) {
    return fat_entry_offset(data_cluster_id) % bpb.bytes_per_sector;
}

unsigned long fat_entry_byte_position(struct bpb bpb, unsigned int data_cluster_id) {
    return (fat_entry_sector_position(bpb, data_cluster_id) * bpb.bytes_per_sector) 
        + intra_sector_offset(bpb, data_cluster_id);
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
    if (is_unallocated_fat_entry(initial_entry) || is_faulty_fat_entry(initial_entry) || is_reserved_entry(initial_entry, bpb) || is_final_fat_entry(initial_entry)) {
        return 0;
    }

    unsigned int entry = initial_entry;
    int count = 0;
    while (is_allocated_fat_entry(entry, bpb)) {
        count++;
        entry = next_fat_entry(bpb, entry, image_fd);
    }

    return count;
}

struct cluster_list scan_fat(struct bpb bpb, unsigned int initial_entry, int image_fd) {
    struct cluster_list cluster_list = {
        .cluster_ids = malloc(sizeof(unsigned int) * entry_chain_length(bpb, initial_entry, image_fd)),
        .total_clusters = entry_chain_length(bpb, initial_entry, image_fd)
    };

    if (cluster_list.total_clusters == 0) return cluster_list;

    cluster_list.cluster_ids[0] = initial_entry;
    if (cluster_list.total_clusters == 1) return cluster_list;

    
    unsigned int entry = initial_entry;
    for (int i = 1; i < cluster_list.total_clusters; i++) {
        entry = next_fat_entry(bpb, entry, image_fd);
        cluster_list.cluster_ids[i] = entry;
    }

    return cluster_list;
}

void free_cluster_list(struct cluster_list cluster_list) {
    free(cluster_list.cluster_ids);
}

void free_directory(struct directory directory) {
    free(directory.entries);
}

void read_bytes(char* bytes, int field, int length, int image_fd) {
    for (int i = 0; i < length; i++) {
        int byte = read_unisgned_int(image_fd, field + i, 1);
        bytes[i] = byte;
    }
}

void read_dir_entry_name(unsigned long dir_start_byte_position, struct directory_entry* entry, int image_fd) {
    char raw[FAT_MAX_FILE_NAME + 1];
    raw[FAT_MAX_FILE_NAME] = '\0';
    read_bytes(raw, dir_start_byte_position, FAT_MAX_FILE_NAME, image_fd);
    memcpy(&(entry->file_name), trim_leading(raw), FAT_MAX_FILE_NAME);
}

bool is_free_directory(int image_fd, int directory_start_byte_position) {
    unsigned int value = read_unisgned_int(image_fd, directory_start_byte_position, 4);
    return value == 0x00 || value == 0xE5;
} 

struct directory read_directory_segment(struct bpb bpb, unsigned int cluster_id, int image_fd) {
    char file_name[FAT_MAX_FILE_NAME + FAT_EXTENSION_LENGTH];
    struct directory_entry* entries = malloc(sizeof(struct directory_entry) * 0);
    int total_entries = 0;
    
    while (!is_free_directory(image_fd, (cluster_id * bpb.sectors_per_cluster * bpb.bytes_per_sector ) + (total_entries * 32))) {
        entries = realloc(entries, sizeof(struct directory_entry) * (total_entries + 1));
        unsigned long directory_start_position = (cluster_id * bpb.sectors_per_cluster * bpb.bytes_per_sector ) + (total_entries * 32);
        read_dir_entry_name(directory_start_position, &(entries[total_entries]), image_fd);
        total_entries++;
    }
    
    struct directory segment = {
        .total_entries = total_entries,
        .entries = entries
    };
    return segment;
}


struct directory read_directory(struct bpb bpb, unsigned int initial_cluster_id, int image_fd) {
    struct cluster_list cluster_list = scan_fat(bpb, initial_cluster_id, image_fd);

    printf("Total clusters in directory: %i\n", cluster_list.total_clusters);

    struct directory full_directory = {
        .total_entries = 0,
        .entries = malloc(sizeof(struct directory_entry) * 0)
    };

    for (int i = 0; i < cluster_list.total_clusters; i++) {
        unsigned int cluster_id = cluster_list.cluster_ids[i];
        struct directory segment = read_directory_segment(bpb, cluster_id, image_fd);
        for (int k = 0; k < segment.total_entries; k++) {
            full_directory.entries = realloc(full_directory.entries, sizeof(struct directory_entry) * (full_directory.total_entries + 1));
            full_directory.entries[full_directory.total_entries] = segment.entries[k];
            full_directory.total_entries++;
        }
        free_directory(segment);
    }

    free_cluster_list(cluster_list);
    return full_directory;
}