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
long byte_position(struct bpb bpb, int sector_id);

/** 
 * The position of the first byte of the desired fat entry assuming that the initial
 *  entry is at position 0.
 */ 
int fat_entry_offset(int data_cluster_id);

/**
 * Calculates the sector id of the sector containing the FAT entry for the given data cluster.
 * Since a FAT32 volume can have any number of FATs, this function accepts a fat_id parameter.
 * For an argument of 0, this function returns the entry within the first FAT of the volume.
 * For an argument of 1, this function returns the entry within the second FAT of the volume,
 * so on and so forth.
 */
int fat_entry_sector_position(struct bpb bpb, int fat_id, int data_cluster_id);

/**
 * Calculates the position of the fat entry (measured in bytes from the beginning of the volume)
 * for the given data cluster.
 */
long fat_entry_byte_position(struct bpb bpb, int fat_id, int data_cluster_id);

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
    int file_size;
    unsigned char attributes;
    int cluster_id;
};

bool is_directory(struct directory_entry);

// data type utilities

int read_int(int fd, int field, int field_length);


// low-level utilities

/**
 * Assembles a C Language integer given a little-endian byte representation
 * of an integer.
 */ 
int little_endian_int(unsigned char* bytes, int bytes_length);

int ith_bit(unsigned char byte, int i);
#endif