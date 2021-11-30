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
    int root_cluster_position;
};

struct bpb read_bpb(int fd);

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