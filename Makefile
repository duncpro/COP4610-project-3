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