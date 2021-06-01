#define _GNU_SOURCE
#include <dlfcn.h>

// This module simulates the musllibc in seL4 system, but I think we
// have to implement minimal printing functions as we can't use the printf() from
// stdlibc here
// #include <stdio.h>
// #include <stdlib.h>

int seL4_DebugPutString() {
  int (*printfp)();
  // char* error;
  void *handle; 
  
  handle = dlopen("./mylib.so", RTLD_LAZY);
  if (!handle) {
    // print error
    // exit
  }

  // find the symbol in the share library
  printfp = dlsym(handle, "printf_wrapper");
  if (!printfp) {
    // print error
    // exit
  }

  // call it
  return printfp();
}

int std_printf() {
  return seL4_DebugPutString();
}