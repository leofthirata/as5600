INCLUDES = -I ./source

all:
	mkdir -p ./build
	aarch64-linux-gnu-gcc ./source/as5600.c -o ./build/as5600.o

clean:
	rm -rf ./build/as5600.o