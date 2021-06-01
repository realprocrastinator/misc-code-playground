CC = gcc
CCFLAG = -Wall -Werror -g -O0
all: print mylib.so

print: main.o muslprintf.o
		$(CC) $(CCFLAG) $^ -o $@ -ldl

main.o: main.c
		$(CC) $(CCFLAG) -I. -o $@ -c $^ 

muslprintf.o: muslprintf.c
		$(CC) $(CCFLAG) -I. -o $@ -c $^

mylib.so: dwprintf.c
		$(CC) $(CCFLAG) -I. -fPIC -shared -o $@ $^

clean:
		rm *.o *.so print