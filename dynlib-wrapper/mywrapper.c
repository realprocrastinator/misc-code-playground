#include <stdio.h>
#include <stdarg.h>

int seL4_emu_printf (const char *restrict fmt, ...) {
  int ret;
  va_list ap;
  va_start(ap, fmt);
  ret = vprintf(fmt, ap);
  va_end(ap);
  return ret;
}