CROSS_COMPILE ?= 	aarch64-linux-gnu-
CC			:= $(CROSS_COMPILE)gcc
CFLAGS			:= -Os -Wall
INCLUDE                 := -I../../cswp -I../../cswp/server -I../../common_tcp
VPATH			:= ../../cswp ../../common_tcp ../../cswp/server

OBJECTS := common_tcp.o cswp_server.o cswp_impl.o cswp_server_cmdint.o cswp_server_commands.o cswp_server_impl.o cswp_buffer.o

all: build/cswp_server

build/cswp_server.elf: $(addprefix build/, $(OBJECTS))
	[ -d build ] || mkdir build/
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

build/%.o: %.c
	[ -d build ] || mkdir build/
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $^

build/%: build/%.elf
	$(CROSS_COMPILE)size $^
	$(CROSS_COMPILE)strip -o $@ $^

clean:
	-rm build/*
