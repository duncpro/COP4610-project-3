#include "./../fat32.h"
#include "./../tool_context.h"
#include <stdio.h>

void info_cmd(struct command_context context) {
    struct bpb bpb = context.tool_context->bpb;
    printf("=== FAT32 IMAGE INFO ===\n");
    printf("bytes per sector: %i\n", bpb.bytes_per_sector);
    printf("sectors per cluster: %i\n", bpb.sectors_per_cluster);
    printf("total reserved sectors: %i\n", bpb.total_reserved_sectors);
    printf("total FATs: %i\n", bpb.total_fats);
    printf("total sectors: %i\n", bpb.total_sectors);
    printf("FAT size: %i sectors\n", bpb.fat_size);
    printf("root cluster id: %i\n", bpb.root_cluster_id);

    if (context.tool_context->verbose) {
        printf("=== FAT32 DERIVED IMAGE INFO ===\n");
        printf("root directory length (in clusters): %i\n", entry_chain_length(bpb,
            bpb.root_cluster_id, context.tool_context->image_fd));
        printf("maximum total data clusters: %i\n", total_data_clusters(bpb));
        printf("maximum FAT entry: %i\n", max_fat_entry(bpb));   
    }
}