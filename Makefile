test:
	mkdir -p build
	mkdir -p build/test
	cc -o ./build/test/fat32.o test/fat32.c
	./build/test/fat32.o

.PHONY: test