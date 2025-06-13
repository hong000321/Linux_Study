#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>

int main(void){
    char str[BUFSIZ], out[BUFSIZ];
    scanf("%s", str);
    printf("1 : %s\n", str);
    sprintf(out, "2 : %s \n", str);
    // int test = open("test.h", O_RDONLY);
    int test = open("test2.h", O_EXCL,644);
    
    FILE test;
    perror(out);
    close(test);
    return 0;
}