INCLUDES = -I ./source

all:
	aarch64-linux-gnu-gcc ./source/as5600.c -o ./build/as5600.o

clean:
	rm -rf ./build/as5600.o