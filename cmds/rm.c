#include "./../tool_context.h"
#include "./../fat32.h"
#include "./../path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void rm_cmd(struct command_context context) {
    if (context.arg_count != 1) {
      printf("Invalid Usage: Expected a single argument, the path to the file.\n");
      return;
    }

    // The absolute path of the file the user is trying to remove.
    struct fat_path* cwd_path = parse_path(context.tool_context->cwd);
    struct fat_path* rel_path = parse_path(context.args[0]);
    struct fat_path* abs_target_path = as_absolute_path(*rel_path, *cwd_path);
    free_path(cwd_path);
    free_path(rel_path);

    struct directory_entry* entry = get_entry_by_absolute_path_string(context.tool_context->bpb, context.tool_context->image_fd, absolute_target_path_str);
    
    free(abs_target_path);

    if (entry == NULL) {
      printf("The given absolute path does not lead to a real directory entry.\n");
      return;
    }  

    if (is_directory(*entry)) {
      printf("Warning: The given absolute path leads to a directory not a file. \n");
      printf("The \"size <absolute path>\" command is intended for use exclusively with files, not directories.\n");
      printf("The file size of a directory is always zero.\n");
    }
    
    //for () {
      // Get first cluster.
      // find_leading_sector_for_cluster(struct bpb bpb, unsigned int cluster_id);

      // Traverse through FAT and deallocate all clusters.
      // next_cluster_id(unsigned int current_cluster_id, int image_fd, struct bpb bpb)
      // last cluster hit
      // first byte = 0x0
      // else
      // first byte = 0xE5
    //}
   
    
/*
• Find the entry corresponding to the file within
the current directory
• Get the first cluster 
Traverse the FAT and deallocate all clusters
• Start at file’s first cluster, deallocate (set to 0x0)
• Move to next cluster, repeat
• Delete the DIRENTRY in the current directory
• If last entry, set first byte at 0x0
• Set first byte at 0xE5 otherwise
*/
