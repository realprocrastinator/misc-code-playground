CC = gcc
CCFLAG = -Wall -g -O0

ALLSRC = $(wildcard src/*.c)
LIBSRCDIR = src
LIBOBJDIR = build/src
LIBSRC = $(addprefix build/, $(ALLSRC))
LIBOBJS = $(patsubst %.c, %.o, $(LIBSRC))

all : build_dir build/libminic.a build/server build/client

build_dir :
		mkdir -p build
		cp -r include build/include
		cp -r src build/src
		cp main.c build/main.c
		cp server.c build/server.c
		cp client.c build/client.c
		echo $(LIBSRC)

build/main.o : build/main.c
		$(CC) $(CCFLAG) -c -o $@ $<

build/server : build/server.o build/libminic.a
		$(CC) $(CCFLAG) -o $@ $^

build/client : build/client.o build/libminic.a
		$(CC) $(CCFLAG) -o $@ $^ 

build/server.o : build/server.c
		$(CC) $(CCFLAG) -c -o $@ $<

build/client.o : build/client.c
		$(CC) $(CCFLAG) -c -o $@ $<

$(LIBOBJDIR)/%.o : $(LIBSRCDIR)/%.c
		$(CC) $(CCFLAG) -c -o $@ $<

build/libminic.a : $(LIBOBJS)
		rm -f $@
		ar rc $@ $^
		ranlib $@

clean:
		rm -rf build

.PHONY: all clean install
