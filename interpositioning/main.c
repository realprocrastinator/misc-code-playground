// This is a simulation stub of the seL4 user application

// Here the std_printf() will be the actual printf() in the real seL4 system
// provided by musl

int std_printf();

int main() {
  std_printf();
  return 0;
}