#ifndef FSU_STORAGE_UNIT
#define FSU_STORAGE_UNIT

#define BYTE 0
#define KILOBYTE 1
#define MEGABYTE 2
#define GIGABYTE 3

/**
 * Calculates the number of bytes equal to the given quantity of given units.
 */ 
long bytes(long double quantity, int unit);

#endif