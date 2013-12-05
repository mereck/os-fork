#include <sys/syscall.h>

#define __NR_cowcopy 378

int test_cowcopy(const char *src, const char *dest) {
	return syscall(__NR_cowcopy, src, dest);
}

