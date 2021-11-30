#include "repl.h"
#include "tool_context.h"
#include "fat32.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Bad Usage: Expected path to FAT32 disk image as first argument.\n");
        return 1;
    }

    struct tool_context context = { 
        .image_fd = open(argv[1], O_RDWR),
        .verbose = argc == 3 && strcmp(argv[2], "verbose") == 0
    };

    if (context.image_fd == -1) {
        printf("Bad Usage: The file located at the given path could not be opened. Check the path and try again.\n");
        return 1;
    }

    printf("FAT32 Tools\n");
    if (context.verbose) {
        printf("Verbose logging enabled.\n");
    }

    context.bpb = read_bpb(context.image_fd);

    repl(context);

    close(context.image_fd);
    return 0;
}