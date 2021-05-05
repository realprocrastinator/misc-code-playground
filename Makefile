CC=gcc
CCFLAG=-O0 -g3 -Wall -Werror -D_GNU_SOURCE

all: measure_syscall measure_syscall.S

measure_syscall: measure_sys_cnt.c
		$(CC) $(CCFLAG) -o measure_syscall measure_sys_cnt.c -lpthread

measure_syscall.S: measure_syscall
		objdump -S measure_syscall > measure_syscall.S

clean:
		rm -f measure_syscall measure_syscall.S
