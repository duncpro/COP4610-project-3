#ifndef FSU_FAT
#define FSU_FAT

#include <stdbool.h>
#include <stdint.h>

// Low level utilities

/**
 * Reads a single 8 digit byte from the given disk image.
 * The given address is an offset from the 0, the first byte in the disk image.
 */
uint8_t read_byte(int image_fd, uint32_t address);

/**
 * Assembles a C Language integer given a little-endian byte representation
 * of an integer. 
 */ 
uint32_t from_little_endian(uint8_t* bytes, int length);

/**
 * Reads the unsigned little endian int locted at the given address in the given file.
 */
uint32_t read_unisgned_little_endian_int(int fd, uint32_t address, uint8_t field_length);

// BIOS Paramter Block

struct bpb {
    unsigned int bytes_per_sector;
    unsigned int sectors_per_cluster;
    unsigned int total_reserved_sectors;
    unsigned int total_fats;
    unsigned int total_sectors;
    unsigned int fat_size; /* in sectors */
    unsigned int root_cluster_id;
};

struct bpb read_bpb(int fd);

/**
 * Returns the sector ID of the first data sector in the disk image represented by the given BIOS parameter block. 
 */
unsigned int find_initial_data_sector(struct bpb bpb);

/**
 * Returns the sector ID of the first data sector in the given cluster.
 */
unsigned int find_leading_sector_for_cluster(struct bpb bpb, unsigned int cluster_id);

/**
 * Encapsulates all the clusters pertaining to a single file or directory. 
 */
struct cluster_list {
    unsigned int total_clusters;

    /**
     * The ids of all clusters associated with the file which this cluster_list represents.
     * This array is ordered logically. In other words, reading the contents of all the clusters in the order
     * in which they appear within the array should result in an uncurropted, fully-intact file or directory. 
     */
    unsigned int* cluster_ids;
};


/**
 * Calculates the the length of the cluster chain, starting from the given FAT entry.
 */
unsigned int calc_entry_chain_length(struct bpb bpb, unsigned int initial_entry, int image_fd);

/**
 * Traverses the FAT, finding all clusters downstream from the given cluster.
 * The returned list is ordered logically. In other words, by reading the contents of each cluster
 * in the same order in which they appear in the list, a fully intact file can be produced.
 */
struct cluster_list scan_fat(struct bpb bpb, unsigned int initial_entry, int image_fd);

/**
 * Companion to scan_fat which deallocated all the dynamically allocated memory within the returned cluster list.
 * Intended for invocation before the cluster_list passes outside of the application scope.
 */
void free_cluster_list(struct cluster_list cluster_list);

#define FAT_MAX_FILE_NAME 8
#define FAT_EXTENSION_LENGTH 3

struct directory_entry {
    char file_name[FAT_MAX_FILE_NAME + 1];
    char extension[FAT_EXTENSION_LENGTH + 1];
    unsigned int file_size;
    unsigned char attributes;
    int cluster_id;
};

bool is_directory(struct directory_entry);

struct directory {
    int total_entries;
    struct directory_entry* entries;
};


struct directory read_directory(struct bpb bpb, unsigned int initial_dir_cluster_id, int image_fd);

void free_directory(struct directory directory);

// low-level utilities

int ith_bit(unsigned char byte, unsigned int i);
#endif