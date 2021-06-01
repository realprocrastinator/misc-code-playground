
volatile const char* msgg = "helloworld\n";

void _start(void) {
    asm (
        "mov %%rax, %%rsi;"
        "mov $1, %%rax;"   // syscall number must be put in %rax
        "mov $1, %%rdi;"   // folow the calling convention of x86, this will be the stout fd
        // "mov %[msg], %%rsi;"   // address of string to output
        "mov $12, %%rdx;"  // bytes we need to write
        "syscall;"
        :
        : [msg] "r" (msgg)
        : "rsi"
    );
    asm (
        "mov $60, %%rax;"     // exit is 60
        "xor %%rdi, %%rdi;"    // return code 0
        "syscall;"
        :
        :          
    );
}
