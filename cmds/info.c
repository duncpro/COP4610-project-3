#include "./../fat32.h"
#include "./../tool_context.h"
#include <stdio.h>

void info_cmd(struct command_context context) {
    struct bpb bpb = read_bpb(context.tool_context->image_fd);
    printf("=== FAT32 IMAGE INFO ===\n");
    printf("bytes per sector: %i\n", bpb.bytes_per_sector);
    printf("sectors per cluster: %i\n", bpb.sectors_per_cluster);
    printf("total reserved sectors: %i\n", bpb.total_reserved_sectors);
    printf("total FATs: %i\n", bpb.total_fats);
    printf("total sectors: %i\n", bpb.total_sectors);
    printf("FAT size: %i sectors\n", bpb.fat_size);
    printf("root cluster id: %i\n", bpb.root_cluster_id);
}