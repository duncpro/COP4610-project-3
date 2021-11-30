#ifndef FSU_FAT
#define FSU_FAT

#include <stdbool.h>

struct bpb {
    int bytes_per_sector;
    int sectors_per_cluster;
    int total_reserved_sectors;
    int total_fats;
    int total_sectors;
    int fat_size; /* in sectors */
    int root_cluster_id;
};

struct bpb read_bpb(int fd);

/**
 * Calculates the position of the first byte of the given sector.
 * The position is the distance in bytes, from the start of the volume,
 * to the first byte in the given sector.
 */
unsigned long byte_position(struct bpb bpb, unsigned int sector_id);

/** 
 * The position of the first byte of the desired fat entry assuming that the initial
 *  entry is at position 0.
 */ 
unsigned int fat_entry_offset(unsigned int data_cluster_id);

/**
 * Calculates the sector id of the sector containing the FAT entry for the given data cluster.
 */
unsigned int fat_entry_sector_position(struct bpb bpb, unsigned int data_cluster_id);

/**
 * Calculates the position of the fat entry (measured in bytes from the beginning of the volume)
 * for the given data cluster.
 */
unsigned long fat_entry_byte_position(struct bpb bpb, unsigned int data_cluster_id);

unsigned int next_fat_entry(struct bpb bpb, unsigned int current_entry, int image_fd);

/**
 * Determines if the given FAT entry is the last element in a linked list of clusters.
 */
bool is_final_fat_entry(unsigned int entry);

bool is_unallocated_fat_entry(unsigned int entry);

bool is_allocated_fat_entry(unsigned int entry, struct bpb bpb);

bool is_faulty_fat_entry(unsigned int entry);

bool is_reserved_entry(unsigned int entry, struct bpb bpb);

/**
 * Calculates the total number of data sectors within the volume when the volume is completely full.
 */
unsigned int total_data_sectors(struct bpb bpb);

/**
 * Calculates the total number of data clusters within the volume when it is completely full. 
 */
unsigned int total_data_clusters(struct bpb bpb);

/**
 * Calculates the maximum value that may be stored within a FAT entry. 
 */
unsigned int max_fat_entry(struct bpb bpb);

/**
 * Encapsulates all the clusters pertaining to a single file or directory. 
 */
struct cluster_list {
    int total_clusters;

    /**
     * The ids of all clusters associated with the file which this cluster_list represents.
     * This array is ordered logically. In other words, reading the contents of all the clusters in the order
     * in which they appear within the array should result in an uncurropted, fully-intact file or directory. 
     */
    int* cluster_ids;
};

struct directory_entry {
    /**
     * Modifiers should take care to pad any unused space within the file name
     * with trailing space. FAT file names are exactly 11 bytes long, the
     * 12th byte in this array is reserved exclusively for a null terminator.
     * This extraneous byte is included to make interoperability with C easier.
     * Users should take care to not include the null terminator when commiting
     * a file name to a FAT formatted volume.
     */
    char file_name[12];
    unsigned int file_size;
    unsigned char attributes;
    int cluster_id;
};

struct directory {
    int total_entries;
    struct directory_entry* entries;
};

bool is_directory(struct directory_entry);

// data type utilities

unsigned int read_unisgned_int(int fd, unsigned int field, unsigned int field_length);


// low-level utilities

/**
 * Assembles a C Language integer given a little-endian byte representation
 * of an integer.
 */ 
unsigned int little_endian_unsigned_int(unsigned char* bytes, unsigned int bytes_length);

int ith_bit(unsigned char byte, unsigned int i);

unsigned int entry_chain_length(struct bpb bpb, unsigned int initial_entry, int image_fd);
struct cluster_list scan_fat(struct bpb bpb, unsigned int initial_entry, int image_fd);
void free_cluster_list(struct cluster_list cluster_list);
#endif