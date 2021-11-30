build/fat32_tools.o: fat32.c repl.c string_utils.c main.c cmds/info.c cmds/size.c command.c dispatch.c
	mkdir -p build
	cc -o ./build/fat32_tools.o fat32.c repl.c string_utils.c main.c command.c dispatch.c

test:
	mkdir -p build
	mkdir -p build/test
	cc -o ./build/test/fat32.o test/fat32.c
	cc -o ./build/test/string_utils.o test/string_utils.c
	cc -o ./build/test/command.o test/command.c
	./build/test/fat32.o
	./build/test/string_utils.o
	./build/test/command.o	

run: build/fat32_tools.o
	./build/fat32_tools.o ./assets/canvas_example.img verbose

.PHONY: test