#include <stdio.h>
#include <sys/utsname.h>
#include "inject.h"

int main() {
	char *src,dest;
	src = "source.txt";
	dest = "dest.txt";
    int ret = test_cowcopy(src,dest);
    if (ret != 0) {
        printf("test cowcopy ret: %d\n", ret);
        printf("Error! Test cowcopy failed!\n");
        return 1;
    } else {
	printf("Launched test_cowcopy\n");
	}

    return 0;
}
