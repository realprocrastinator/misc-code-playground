
void _start(void) {
//     mywrite();
//     myexit();

    char *msg = "helloworld\n";

    asm (
        "mov $1, %%rax;"   // syscall number must be put in %rax
        "mov $1, %%rdi;"   // folow the calling convention of x86, this will be the stout fd
        "mov  %0, %%rsi;"   // address of string to output
        "mov $11, %%rdx;"  // bytes we need to write
        "syscall;"
        :
        : "r" (msg)
    );
    asm (
        "mov $60, %%rax;"     // exit is 60
        "xor %%rdi, %%rdi;"    // return code 0
        "syscall;"
        :
        :          
    );
}