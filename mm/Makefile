CC = gcc
CCFLAG = -g -O0 -Wall -Werror

ALL_TARGETS = handle_segv mmap

all: $(ALL_TARGETS)

handle_segv: handle_segv.c
		$(CC) $(CCFLAG) -o $@ $<

mmap: mmap.c
		$(CC) $(CCFLAG) -o $@ $<

clean:
		rm -rf $(ALL_TARGETS) 