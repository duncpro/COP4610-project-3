#ifndef FSU_FAT
#define FSU_FAT

struct bpb {
    int bytes_per_sector;
    int sectors_per_cluster;
    int root_cluster_position;
};

/**
 * Assembles a C Language integer given a little-endian byte representation
 * of an integer.
 */ 
int little_endian_int(unsigned char* bytes, int bytes_length);

int read_bpb_field_int(int fd, int field, int field_length);

#endif