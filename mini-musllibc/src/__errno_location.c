#include "errno.h"
#include "internal_pthread_impl.h"

// TODO: migirate pthread here instead of this hacky way

typedef struct pthread * pthread_t;

int *__mini_errno_location(void)
{
	return &__mini_pthread_self()->errno_val;
}

weak_alias(__mini_errno_location, ___mini_errno_location);