#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());
    printf("Real GID: %d\n", getgid());
    printf("Effective GID: %d\n", getegid());

    return 0;
}