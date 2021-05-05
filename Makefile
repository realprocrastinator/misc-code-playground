CC=gcc
CCFLAG=-O0 -Wall -Werror

all: measure_syscall

measure_syscall:
		$(CC) $(CCFLAG) -o measure_syscall measure_sys_cnt.c -lpthread

clean:
		rm -f measure_syscall