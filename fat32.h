#ifndef FSU_FAT
#define FSU_FAT

/**
 * Assembles a C Language integer given a little endian byte array.
 */ 
int little_endian_int(unsigned char* bytes, int bytes_length);

#endif