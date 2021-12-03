build/fat32_tools.o: fat32.c repl.c string_utils.c main.c cmds/info.c cmds/size.c cmds/ls.c cmds/cd.c command.c dispatch.c path.c tool_context.c
	mkdir -p build
	cc -g -o ./build/fat32_tools.o fat32.c repl.c string_utils.c main.c command.c dispatch.c path.c tool_context.c

test:
	mkdir -p build
	mkdir -p build/test
	cc -o ./build/test/fat32.o test/fat32.c
	cc -o ./build/test/string_utils.o test/string_utils.c 
	cc -o ./build/test/command.o test/command.c
	cc -o ./build/test/path.o test/path.c
	./build/test/fat32.o
	./build/test/string_utils.o
	./build/test/command.o	
	./build/test/path.o

run: build/fat32_tools.o
	./build/fat32_tools.o ./assets/canvas_example.img

.PHONY: test