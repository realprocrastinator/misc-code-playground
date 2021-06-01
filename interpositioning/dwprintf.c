// This module simulates the dynamic glibc wrapper library used in the real seL4 emu project
#include <stdio.h>
#include <dwprintf.h>

// Question: here is we still need to use printf() provided by glibc,
// this symbol has already been resolved in the musllibc. I'm not quite sure whether it will be problem if
// we got another symbol of printf when we load the shared library. My opinion is it should be fine, as the
// printf symbol from the shared library will be resolved using GOT and PLT, so it won't be conflict with the
// symbol resolved from the musllibc. 

// Here too make it simple I just use std_printf() to refer to the real printf() in libc
int std_printf() {
  return printf("Finger Cross\n");
}

int printf_wrapper () {
  return std_printf();
}