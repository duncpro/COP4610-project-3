#include "fat32.h"
#include "string_utils.h"
#include "path.h"
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int ith_bit(unsigned char byte, unsigned int i) {
    return (byte >> i) & 1;
}

uint32_t combine_ints(uint16_t high, uint16_t low) {
    uint32_t combined = 0;
    combined += (((uint32_t) high) << 16);
    combined += ((uint32_t) low);
    return combined;
}

uint32_t little_endian_unsigned_int(uint8_t* bytes, int length) {
    int sum = 0;
    for (int i = 0; i < length; i++) {
        sum += (((uint32_t) bytes[i]) << (i * 8));
    }
    return sum;
}

uint32_t read_unisgned_little_endian_int(int fd, uint32_t address, uint8_t field_length) {
    uint8_t buffer[field_length];
    int total_read = pread(fd, buffer, field_length, address);
    if (total_read < field_length) {
        printf("Malformed FAT 32 image.\n");
        printf("Expected the BPB to contain a required field but it did not.\n");
    }
    return little_endian_unsigned_int(buffer, total_read);
}

struct bpb read_bpb(int fd) {
    struct bpb bpb = {
        .bytes_per_sector = read_unisgned_little_endian_int(fd, 11, 2),
        .sectors_per_cluster = read_unisgned_little_endian_int(fd, 13, 1),
        .total_reserved_sectors = read_unisgned_little_endian_int(fd, 14, 2),
        .total_fats = read_unisgned_little_endian_int(fd, 16, 1),
        .total_sectors = read_unisgned_little_endian_int(fd, 32, 4),
        .fat_size = read_unisgned_little_endian_int(fd, 36, 4),
        .root_cluster_id = read_unisgned_little_endian_int(fd, 44, 4)
    };
    return bpb;
}

bool is_directory(struct directory_entry entry) {
    return entry.attributes & 0x10;
}

unsigned int find_initial_data_sector(struct bpb bpb) {
    return bpb.total_reserved_sectors + (bpb.total_fats * bpb.fat_size);
}

unsigned int find_leading_sector_for_cluster(struct bpb bpb, unsigned int cluster_id) {
    return ((cluster_id - bpb.root_cluster_id) * bpb.sectors_per_cluster) + find_initial_data_sector(bpb);
}

unsigned int next_cluster_id(unsigned int current_cluster_id, int image_fd, struct bpb bpb) {
    unsigned int fat_start_sector_id = bpb.total_reserved_sectors + (8 / bpb.bytes_per_sector);
    unsigned int fat_start = (fat_start_sector_id * bpb.bytes_per_sector); /* bytes */;
    unsigned int entry_position = fat_start + (current_cluster_id * 4);
    return read_unisgned_little_endian_int(image_fd, entry_position, 4);
}

struct cluster_list scan_fat(struct bpb bpb, unsigned int start_at_cluster_id, int image_fd) {
    struct cluster_list cluster_list = {
        .total_clusters = 0,
        .cluster_ids = NULL
    };
    unsigned int cluster_id = start_at_cluster_id;
    while (cluster_id > 0x0000002 && cluster_id < 0xFFFFFF6) {
        cluster_list.cluster_ids = (unsigned int*) realloc(cluster_list.cluster_ids, sizeof(unsigned int) * (cluster_list.total_clusters + 1));
        cluster_list.cluster_ids[cluster_list.total_clusters] = cluster_id;
        cluster_list.total_clusters++;
        cluster_id = next_cluster_id(cluster_id, image_fd, bpb);
    }
    return cluster_list;
}

bool is_directory_terminator(int image_fd, unsigned int entry_pos) {
    uint8_t byte[1];
    int prefix = pread(image_fd, byte, 1, entry_pos);
    return byte[0] == 0x00 || byte[0] == 0xE5 || byte[0] == 0x02;
}

uint8_t read_byte(int image_fd, uint32_t address) {
    uint8_t byte[1];
    pread(image_fd, byte, 1, address);
    return byte[0];
}

bool is_long_dir_name(int image_fd, unsigned int entry_pos) {
    return read_byte(image_fd, entry_pos + 11) & 0x0F; 
}

struct directory read_directory(struct bpb bpb, unsigned int initial_dir_cluster_id, int image_fd) {
    struct directory dir = {
        .total_entries = 0,
        .entries = NULL
    };

    struct cluster_list cluster_list = scan_fat(bpb, find_leading_sector_for_cluster(bpb, initial_dir_cluster_id), image_fd);
    for (int i = 0; i < cluster_list.total_clusters; i++) {
        unsigned int dir_sector_pos = (find_leading_sector_for_cluster(bpb, initial_dir_cluster_id) * bpb.bytes_per_sector);
        unsigned int offset = 0;
        while (!is_directory_terminator(image_fd, dir_sector_pos + offset) && offset < (bpb.bytes_per_sector * bpb.sectors_per_cluster)) {

            if (!is_long_dir_name(image_fd, dir_sector_pos + offset)) {
                dir.entries = (struct directory_entry*) realloc(dir.entries, sizeof(struct directory_entry) * (dir.total_entries + 1));
                struct directory_entry new_entry;

                // file name
                pread(image_fd, new_entry.file_name, 8, offset + dir_sector_pos);
                new_entry.file_name[8] = '\0';
                trim_leading(new_entry.file_name);

                new_entry.attributes = read_unisgned_little_endian_int(image_fd, dir_sector_pos + offset + 11, 1);
                new_entry.file_size = read_unisgned_little_endian_int(image_fd, dir_sector_pos + offset + 28, 4);
                new_entry.cluster_id = combine_ints(
                    read_unisgned_little_endian_int(image_fd, dir_sector_pos + offset + 20, 2),
                    read_unisgned_little_endian_int(image_fd, dir_sector_pos + offset + 26, 2)
                );

                dir.entries[dir.total_entries] = new_entry;
                dir.total_entries++;
            }

            offset += 32;
        }
    }

    return dir;
}

void free_directory(struct directory dir) {
    free(dir.entries);
}

/**
 * Returns a pointer to the directory entry which matches the given child path segment.
 * If no entry exists within the given directory which matches the given child path segment then
 * a NULL pointer is returned instead.
 */
struct directory_entry* get_entry_by_path_segment(struct directory* parent_dir, char* child_path_segment) {
    for (int i = 0; i < parent_dir->total_entries; i++) {\
    char current_entry_name[FAT_EXTENSION_LENGTH + FAT_MAX_FILE_NAME + 1];
    current_entry_name[0] = '\0';
    strcat(current_entry_name, parent_dir->entries[i].file_name);
    strcat(current_entry_name, parent_dir->entries[i].extension);
        if (strcmp(current_entry_name, child_path_segment) == 0) {
            return &(parent_dir->entries[i]);
        }
    }
    return NULL;
}

/**
 * Returns a pointer to a dynamically allocated directory_entry struct representing the 
 */
struct directory_entry* find_directory_entry(struct bpb bpb, int image_fd, char* path_str) {
    struct fat_path* path = parse_path(path);
    struct directory parent_dir = read_directory(bpb, bpb.root_cluster_id, image_fd);
    for (int i = 0; i < path->total_segments; i++) {
        bool is_final_segment = path->total_segments == (i - 1);

        struct directory_entry* entry = get_entry_by_path_segment(&parent_dir, path->segments[i]);

        if (!is_final_segment && !is_directory(*entry)) {
            return NULL;
        }


    }
}