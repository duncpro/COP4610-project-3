#include "fat32.h"
#include "string_utils.h"
#include "misc_utils.h"
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int ith_bit(uint32_t val, unsigned int i) {
    return (val >> i) & 1;
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

void create_unsigned_little_endian_int(uint32_t value, uint8_t bytes[4]) {
    for (int b = 3; b >= 0; b--) {
        bytes[b] = (value >> (8 * b));
    }
}

void write_unisgned_little_endian_int(int fd, uint32_t address, uint32_t value, int length) {
    uint8_t bytes[4];
    create_unsigned_little_endian_int(value, bytes);
    pwrite(fd, bytes, length, address);
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

uint32_t fat_entry_position(uint32_t cluster_id, struct bpb bpb) {
    unsigned int fat_start_sector_id = bpb.total_reserved_sectors + (8 /* two entries */ / bpb.bytes_per_sector);
    unsigned int fat_start = (fat_start_sector_id * bpb.bytes_per_sector); /* bytes */;
    unsigned int entry_position = fat_start + (cluster_id * 4);
    return entry_position;
}

unsigned int next_cluster_id(unsigned int current_cluster_id, int image_fd, struct bpb bpb) {
    uint32_t entry_position = fat_entry_position(current_cluster_id, bpb);
    return read_unisgned_little_endian_int(image_fd, entry_position, 4);
}

uint32_t max_cluster_id(struct bpb bpb) {
    return (find_initial_data_sector(bpb) / bpb.sectors_per_cluster) + 1;
}

/**
 * Returns the cluster id of the first unallocated cluster within the FAT.
 * If the FAT is full then zero is returned.
 */
uint32_t first_unallocated_cluster_id(int image_fd, struct bpb bpb) {
    for (uint32_t cluster_id = bpb.root_cluster_id; cluster_id < max_cluster_id(bpb); cluster_id++) {
        if (next_cluster_id(cluster_id, image_fd, bpb) == 0x0000000) {
            return cluster_id;
        }
    }
    return 0;
}

struct cluster_list scan_fat_by_cluster(struct bpb bpb, uint32_t initial_cluster_id, int image_fd) {
    struct cluster_list cluster_list = {
        .total_clusters = 0,
        .cluster_ids = NULL
    };
    unsigned int cluster_id = initial_cluster_id;
    while (cluster_id >= 0x0000002 && cluster_id < 0xFFFFFF6) {
        cluster_list.cluster_ids = (unsigned int*) realloc(cluster_list.cluster_ids, sizeof(unsigned int) * (cluster_list.total_clusters + 1));
        cluster_list.cluster_ids[cluster_list.total_clusters] = cluster_id;
        cluster_list.total_clusters++;
        cluster_id = next_cluster_id(cluster_id, image_fd, bpb);
    }
    return cluster_list;
}

bool is_directory_terminator(int image_fd, unsigned int entry_pos) {
    uint8_t byte[1];
    int prefix = pread(image_fd, byte, 4, entry_pos);
    return byte[0] == 0x00;
}

bool is_free_directory(int image_fd, unsigned int entry_pos) {
    uint8_t byte[1];
    int prefix = pread(image_fd, byte, 4, entry_pos);
    return byte[0] == 0xE5;
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

    struct cluster_list cluster_list = scan_fat_by_cluster(bpb, initial_dir_cluster_id, image_fd);
    for (int i = 0; i < cluster_list.total_clusters; i++) {
        unsigned int dir_sector_pos = (find_leading_sector_for_cluster(bpb, initial_dir_cluster_id) * bpb.bytes_per_sector);
        unsigned int offset = 0;
        while (!is_directory_terminator(image_fd, dir_sector_pos + offset) && offset < (bpb.bytes_per_sector * bpb.sectors_per_cluster)) {
            if (!is_long_dir_name(image_fd, dir_sector_pos + offset) && !is_free_directory(image_fd, (dir_sector_pos + offset))) {
                dir.entries = (struct directory_entry*) realloc(dir.entries, sizeof(struct directory_entry) * (dir.total_entries + 1));
                struct directory_entry new_entry;

                // file name
                char filename[FAT_FILENAME_LENGTH + 1];
                pread(image_fd, filename, 8, offset + dir_sector_pos);
                filename[8] = '\0';
                strcpy(new_entry.file_name, trim(filename));
          
                // file extension
                char extension[FAT_EXTENSION_LENGTH + 1];
                pread(image_fd, extension, 3, offset + dir_sector_pos + 8);
                extension[3] = '\0';
                strcpy(new_entry.extension, trim(extension));

                new_entry.attributes = read_unisgned_little_endian_int(image_fd, dir_sector_pos + offset + 11, 1);
                new_entry.file_size = read_unisgned_little_endian_int(image_fd, dir_sector_pos + offset + 28, 4);
                new_entry.cluster_id = combine_ints(
                    read_unisgned_little_endian_int(image_fd, dir_sector_pos + offset + 20, 2),
                    read_unisgned_little_endian_int(image_fd, dir_sector_pos + offset + 26, 2)
                );
                
                new_entry.entry_byte_position = (dir_sector_pos + offset);

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


struct directory_entry* get_entry_by_absolute_path(struct fat_path path, int fd, struct bpb bpb) {
    char* absolute_path_string = as_path_string(path);
    struct directory_entry* entry = get_entry_by_absolute_path_string(bpb, fd, absolute_path_string);
    free(absolute_path_string);
    return entry;
}

/**
 * Returns a pointer to the directory entry which matches the given child path segment.
 * If no entry exists within the given directory which matches the given child path segment then
 * a NULL pointer is returned instead.
 */
struct directory_entry* get_entry_by_path_segment(struct directory* parent_dir, char* child_path_segment) {
    for (int i = 0; i < parent_dir->total_entries; i++) {
        char current_entry_name[FAT_EXTENSION_LENGTH + FAT_FILENAME_LENGTH + 1];
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
 * Returns a pointer to a dynamically allocated directory_entry struct representing the directory entry
 * associated with the given absolute path string. If the given absolute path does not point to a directory_entry
 * then a NULL pointer is returned instead. If this path points to the root directory, then of course no directory
 * entry exists for the root directory and this function will return null.
 */
struct directory_entry* get_entry_by_absolute_path_string(struct bpb bpb, int image_fd, char* path_str) {
    struct fat_path* path = parse_path(path_str);

    struct directory parent_dir = read_directory(bpb, bpb.root_cluster_id, image_fd);

    for (int i = 0; i < path->total_segments; i++) {
        bool is_final_segment = (path->total_segments - 1) == i;

        struct directory_entry* entry = get_entry_by_path_segment(&parent_dir, path->segments[i]);
        if (entry == NULL) return NULL;

        if (is_final_segment) {
            struct directory_entry* entry_copy = (struct directory_entry*) malloc(sizeof(struct directory_entry));
            memcpy(entry_copy, entry, sizeof(struct directory_entry));
            return entry_copy;
        } else {
            if (!is_directory(*entry)) return NULL;
            parent_dir = read_directory(bpb, entry->cluster_id, image_fd);
        }
    }

    return NULL;
}

uint32_t max_dir_entries_per_cluster(struct bpb bpb) {
    return (bpb.sectors_per_cluster * bpb.bytes_per_sector) / 32;
}

/**
 * Expands the given file by creating a new cluster.
 * The new cluster's ID can be found using next_cluster_id(parent_cluster_id) after invocation of this method.
 * The parent_cluster_id should be the last cluster within the cluster chain for a given file.
 * Returns true if the cluster was successfully allocated, otherwise returns false.
 */ 
bool expand_file(struct bpb bpb, int image_fd, uint32_t parent_cluster_id) {
    uint32_t free_cluster_id = first_unallocated_cluster_id(image_fd, bpb);
    if (free_cluster_id == 0) return false;
    uint32_t entry_position = fat_entry_position(parent_cluster_id, bpb);
    write_unisgned_little_endian_int(image_fd, fat_entry_position(parent_cluster_id, bpb), free_cluster_id, 4);
    write_unisgned_little_endian_int(image_fd, fat_entry_position(free_cluster_id, bpb), 0xFFFFFFFF, 4);
    return true;
}

bool create_file(struct bpb bpb, int image_fd, uint32_t initial_dir_cluster_id, char* filename, char* extension) {
    // The byte position, in the image file, of the first free directory entry spot. 
    uint32_t first_free_entry_byte_address = 0;
    struct cluster_list cluster_list = scan_fat_by_cluster(bpb, initial_dir_cluster_id, image_fd);
    
    for (uint32_t cluster_index = 0; cluster_index < cluster_list.total_clusters; cluster_index++) {
        uint32_t dir_cluster_id = cluster_list.cluster_ids[cluster_index];
        struct directory dir = read_directory(bpb, dir_cluster_id, image_fd);
        for (int dir_entry_id = dir.total_entries; dir_entry_id < max_dir_entries_per_cluster(bpb); dir_entry_id++) {
            uint32_t dir_byte_pos = (bpb.bytes_per_sector * find_leading_sector_for_cluster(bpb, dir_cluster_id)) + (dir_entry_id * 32);
            if (is_free_directory(image_fd, dir_byte_pos) || is_directory_terminator(image_fd, dir_byte_pos)) {
                first_free_entry_byte_address = dir_byte_pos;
                goto cluster_scan_over;
            }
        }
    }

    cluster_scan_over:

    // Allocate a new cluster for the directory if necessary.
    if (first_free_entry_byte_address == 0) {
        uint32_t current_last_cluster_id = cluster_list.cluster_ids[cluster_list.total_clusters - 1];
        expand_file(bpb, image_fd, current_last_cluster_id);
        uint32_t new_cluster_id = read_unisgned_little_endian_int(image_fd, fat_entry_position(current_last_cluster_id, bpb), 4);
        first_free_entry_byte_address = find_leading_sector_for_cluster(bpb, new_cluster_id);
    }

    // Allocate a new cluster to hold the file's contents.
    uint32_t file_contents_cluster_id = first_unallocated_cluster_id(image_fd, bpb);
    if (file_contents_cluster_id == 0) return false;
    write_unisgned_little_endian_int(image_fd, fat_entry_position(file_contents_cluster_id, bpb), 0xFFFFFFFF, 4);


    // Finally write the directory entry.
    pwrite(image_fd, filename, lowest_of(FAT_FILENAME_LENGTH, strlen(filename)), first_free_entry_byte_address);
    pwrite(image_fd, extension, lowest_of(FAT_EXTENSION_LENGTH, strlen(extension)), first_free_entry_byte_address + 8);
    write_unisgned_little_endian_int(image_fd, first_free_entry_byte_address + 28, 0, 4);     // Size: zero bytes
    
    uint8_t bytes[4];
    create_unsigned_little_endian_int(file_contents_cluster_id, bytes);
    pwrite(image_fd, bytes, 2, first_free_entry_byte_address + 26); // Low word
    pwrite(image_fd, bytes + (sizeof(uint8_t) * 2), 2, first_free_entry_byte_address + 20); // High word
    return true;
}

void delete_file(struct bpb bpb, int image_fd, struct directory_entry entry) {
    struct cluster_list cluster_list = scan_fat_by_cluster(bpb, entry.cluster_id, image_fd);

    // Zero out all the clusters which were associated with this file.
    for (int i = 0; i < cluster_list.total_clusters; i++) {
        uint32_t cluster_id = cluster_list.cluster_ids[i];
        uint32_t data_address = find_leading_sector_for_cluster(bpb, cluster_id) * bpb.bytes_per_sector * bpb.sectors_per_cluster;
        uint32_t cluster_length /* bytes */ = bpb.bytes_per_sector * bpb.sectors_per_cluster;
        for (uint32_t b = 0; b < cluster_length; b++) {
            uint32_t byte_address = data_address + b;
            write_unisgned_little_endian_int(0, byte_address, 0, 1);
        }
    }

    // Remove all the FAT entries.
    for (int i = 0; i < cluster_list.total_clusters; i++) {
        uint32_t cluster_id = cluster_list.cluster_ids[i];
        write_unisgned_little_endian_int(image_fd, fat_entry_position(cluster_id, bpb), 0, 4);
    }

    // Finally remove the directory entry.
    for (int i = 0; i < FAT_DIRECTORY_ENTRY_LENGTH; i++) {
        write_unisgned_little_endian_int(image_fd, entry.entry_byte_position + i, 0, 1);
    }
}