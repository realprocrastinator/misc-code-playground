#include <sys/auxv.h>
#include <elf.h>
#include <stdio.h>
#include <asm/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

/* Will be eventually in asm/hwcap.h */
#ifndef HWCAP2_FSGSBASE
#define HWCAP2_FSGSBASE        (1 << 1)
#endif

static  char tls[0x1000];

int main () {

  /*
   * The instructions are enumerated in CPUID leaf 7, bit 0 of EBX. If available /
   * proc/cpuinfo shows ‘fsgsbase’ in the flag entry of the CPUs.
   * The availability of the instructions does not enable them automatically. The kernel has to enable them explicitly in CR4. The reason for this is that older kernels make assumptions about the values in the GS register and enforce them when * GS base is set via arch_prctl(). Allowing user space to write arbitrary values to GS base would violate these assumptions and cause malfunction.
   * On kernels which do not enable FSGSBASE the execution of the FSGSBASE instructions will fault with a #UD exception.
   * The kernel provides reliable information about the enabled state in the ELF AUX vector. If the HWCAP2_FSGSBASE bit is set in the AUX vector, the kernel has FSGSBASE instructions enabled and applications can use them. The following code
   * example shows how this detection works:
   */
  unsigned val = getauxval(AT_HWCAP2);

  if (val & HWCAP2_FSGSBASE)
    printf("FSGSBASE enabled\n");

  void *fsbase;

  // get fs
  syscall(SYS_arch_prctl, ARCH_GET_FS, (unsigned long *) &fsbase);
  // arch_prctl(ARCH_GET_FS, &fsbase);

  printf("TLS at: %lx\n", (unsigned long)fsbase);

  // set fs
  syscall(SYS_arch_prctl, ARCH_SET_FS, (unsigned long) tls);
  // arch_prctl(ARCH_SET_FS, (void*)tls);

  // get fs
  syscall(SYS_arch_prctl, ARCH_GET_FS, (unsigned long *) &fsbase);
  // arch_prctl(ARCH_GET_FS, &fsbase);

  printf("new TLS at: %lx\n", (unsigned long)fsbase);

  return 0;
}

