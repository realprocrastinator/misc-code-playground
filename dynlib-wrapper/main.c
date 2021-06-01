#include <stdio.h>
#include <stdlib.h>
#ifdef CONFIG_USE_DYNLOAD
#include <dlfcn.h>
#else
#include "mywrapper.h"
#endif

#ifdef CONFIG_USE_DYNLOAD
void seL4_emu_stdc_install(void *dlhdl, void **fp, const char *fn_name) {
  *fp = dlsym(dlhdl, fn_name);
}
#endif

int main () {
  char *name = "Andy";
  
#ifdef CONFIG_USE_DYNLOAD
  void *handle;
  char *error;
  int (*fp) (char*, ...);

  handle = dlopen("./libmywrapper.so", RTLD_LAZY);
  if(!handle) {
    fprintf(stderr, "%s\n", dlerror());
    exit(EXIT_FAILURE);
  }

  /* Clear any existing error */
  dlerror();

  seL4_emu_stdc_install(handle, (void **) &fp, "seL4_emu_printf");  

  if ((error = dlerror()) != NULL) {
    fprintf(stderr, "%s\n", error);
    exit(EXIT_FAILURE);
  }

  fp("Hello World from %s.\n", name);
#else
  seL4_emu_printf("Hello World from %s.\n", name);
#endif
  
  return 0;
}