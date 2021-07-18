CC = gcc
CCFLAG = -g -O0 -Wall -Werror

ALL_TARGETS = handle_segv sud-parent-child vm_fault sud

all: $(ALL_TARGETS)

handle_segv: handle_segv.c
		$(CC) $(CCFLAG) -o $@ $<

sud-parent-child: sud-parent-child.c
		$(CC) $(CCFLAG) -o $@ $<

vm_fault: vm_fault.c
		$(CC) $(CCFLAG) -o $@ $<

sud: sud.c
		$(CC) $(CCFLAG) -o $@ $<

clean:
		rm -rf $(ALL_TARGETS) 