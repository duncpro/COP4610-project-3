#include "./../fat32.h"
#include "./../tool_context.h"
#include <stdio.h>

void info(struct command_context context) {
    struct bpb bpb = read_bpb(context.tool_context.image_fd);
    printf("=== FAT32 IMAGE INFO ===\n");
    printf("bytes per sector: %i\n", bpb.bytes_per_sector);
    printf("sectors per cluster: %i\n", bpb.sectors_per_cluster);
}