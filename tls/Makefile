CC = gcc
CCFLAG = -g -O0 -Wall -Werror

ALL_TARGETS = fsgsbase tls_test

all: $(ALL_TARGETS)

fsgsbase: fsgsbase.c tls_test.c
		$(CC) $(CCFLAG) -o $@ $<

tls_test: tls_test.c tls_megnify
		$(CC) $(CCFLAG) -o $@ $<

tls_megnify: tls_test.c
		cpp $< > tls_test.i
		objdump -d tls_test > tls_test.s

clean:
		rm -rf $(ALL_TARGETS)