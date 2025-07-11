#include <unistd.h>
#include <linux/unistd.h>
#include <stdio.h>

int add(int a, int b, int *ret) {
	return syscall(__NR_add, a, b, ret);
}


int main(int argc, char **argv){
	int ret;
	add(23, 53, &ret);
	printf("The result of adding 23 and 53 is: %d\n", ret);
	return 0;
}
