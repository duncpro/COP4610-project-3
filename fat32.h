#ifndef FSU_FAT
#define FSU_FAT

struct bpb {
    int bytes_per_sector;
    int sectors_per_cluster;
    int total_reserved_sectors;
    int total_fats;
    int total_sectors;
    int fat_size; /* in sectors */
    int root_cluster_position;
};

/**
 * Assembles a C Language integer given a little-endian byte representation
 * of an integer.
 */ 
int little_endian_int(unsigned char* bytes, int bytes_length);

int read_int(int fd, int field, int field_length);

struct bpb read_bpb(int fd);

#endif