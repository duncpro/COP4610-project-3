build/fat32_tools.o: fat32.c repl.c string_utils.c units.c main.c cmds/info.c
	mkdir -p build
	cc -o ./build/fat32_tools.o fat32.c repl.c string_utils.c units.c main.c

test:
	mkdir -p build
	mkdir -p build/test
	cc -o ./build/test/fat32.o test/fat32.c
	cc -o ./build/test/string_utils.o test/string_utils.c
	cc -o ./build/test/repl.o test/repl.c
	./build/test/fat32.o
	./build/test/string_utils.o
	./build/test/repl.o	

.PHONY: test