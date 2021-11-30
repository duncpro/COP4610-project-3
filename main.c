#include "repl.h"
#include "tool_context.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Bad Usage: Expected path to FAT32 disk image as only argument.\n");
        return 1;
    }

    struct tool_context context = { 
        .image_fd = open(argv[1], O_RDWR)
    };

    if (context.image_fd == -1) {
        printf("Bad Usage: The file located at the given path could not be opened. Check the path and try again.\n");
        return 1;
    }

    repl(context);

    close(context.image_fd);
    return 0;
}