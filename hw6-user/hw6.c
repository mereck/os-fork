#include <stdio.h>
#include <sys/utsname.h>
#include "inject.h"

int main() {
	char *src,dest;
	src = "reallyLongSource.txt";
	dest = "dest.txt";
    int ret = test_cowcopy(src,dest);
    if (ret != 0) {
        printf("test cowcopy ret: %d\n", ret);
        printf("Error! Test cowcopy failed!\n");
        return 1;
    } else {
	printf("Launched test_cowcopy with src:%s dest: %s\n",src,dest);
	}

	src = "..";
	dest = "dest.txt";
    ret = test_cowcopy(src,dest);
    if (ret != 0) {
        printf("test cowcopy ret: %d\n", ret);
        printf("Error! Test cowcopy failed!\n");
        return 1;
    } else {
	printf("Launched test_cowcopy with src:%s dest: %s\n",src,dest);
	}


    return 0;
}
