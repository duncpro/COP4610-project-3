/**
 * Extracts the filename segment from the given path.
 * Writes the filename to the given pointer as a null terminated c string.
 * Does not make any assumptions about the size of path nor filename.
 */
void filename_from_path(char* path, char* filename);