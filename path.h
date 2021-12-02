#include <stdint.h>

#ifndef FSU_PATH
#define FSU_PATH

/**
 * Extracts the filename segment from the given path.
 * Writes the filename to the given pointer as a null terminated c string.
 * Does not make any assumptions about the size of path nor filename.
 */
void filename_from_path_string(char* path, char* filename);

struct fat_path {
    uint32_t total_segments;
    char** segments;
};

/**
 * Parses the given string representation of a path into a fat_path struct.
 * Callers should take care to eventually pass the returned path object to free_path to avoid memory
 */
struct fat_path* parse_path(char* str);

/**
 * Creates an absolute path using the given relative path and and absolute root path.
 * Both inputs remain unchanged, and a new absolute fat_path is allocated and returned.
 * The caller should take care to release the returned path as well the input paths when they are no longer needed.
 */ 
struct fat_path* as_absolute_path(struct fat_path relative_path, struct fat_path absolute_root);

void free_path(struct fat_path*);

#endif