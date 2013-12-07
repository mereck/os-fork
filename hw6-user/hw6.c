#include <stdio.h>
#include <sys/utsname.h>
#include "inject.h"

int main(int argc, char *argv[]) {
	char *src;
	char *dest;
	FILE * fp;
	src = argv[1];
	dest = argv[2];
    int ret = test_cowcopy(src,dest);
    if (ret != 0) {
        printf("test cowcopy ret: %d\n", ret);
        printf("Error! Test cowcopy failed!\n");
		fp = fopen("/data/local/tmp/dest.txt","w");

        return 1;
    } else {
	printf("Launched test_cowcopy with src:%s dest: %s\n",src,dest);
	}

	
	//fp = fopen("/data/local/tmp/source.txt","r");

	//fp = fopen("/data/local/tmp/source.txt","r");
	//fp = fopen("/data/local/tmp/source.txt","r");
	//fp = fopen("/data/local/tmp/source.txt","r");
	//fp = fopen("/data/local/tmp/source.txt","r");

	return 0;

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
