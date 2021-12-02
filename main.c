#include "repl.h"
#include "tool_context.h"
#include "fat32.h"
#include "dispatch.h"
#include "string_utils.h"
#include "command.h"
#include "path.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Bad Usage: Expected path to FAT32 disk image as first argument.\n");
        return 1;
    }

    struct tool_context context = { 
        .image_fd = open(argv[1], O_RDWR)
    };

    filename_from_path(argv[1], context.image_file_name);

    if (context.image_fd == -1) {
        printf("Bad Usage: The file located at the given path could not be opened. Check the path and try again.\n");
        return 1;
    }

    printf("FAT32 Tools\n");

    context.bpb = read_bpb(context.image_fd);

    if (argc > 2) {
        dispatch(parse_command(argv[2]), &context);
    } else {
        repl(context);
    }


    close(context.image_fd);
    return 0;
}