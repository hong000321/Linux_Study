#include <stdio.h>
#include <stdlib.h>

int main(void){
    printf("Hello,");
    printf("World!");
    fflush(stdout);
    _exit(1);
    return 0;
}