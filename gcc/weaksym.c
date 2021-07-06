// this arttribute will allow us to invoke `old` instead of the new if there is no definition of `new` provided. Otherwise a strong definition will be used for `new`.
#undef weak_alias
#define weak_alias(old, new) \
	extern __typeof(old) new __attribute__((weak, alias(#old)))

static void dummy1(void) {}
weak_alias(dummy1, foo);

int main() {
  foo();
  return 0;
}
