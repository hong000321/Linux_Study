#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(void){
    // close(0);
    // int n;
    // scanf("%d",&n);
    int fd1, fd2;
    fd1 = open("sample1.txt", O_WRONLY | O_CREAT, 0644);
    fd2 = open("sample2.txt", O_WRONLY | O_CREAT, 0644);
    printf("%d %d\n",fd1, fd2);

    close(fd1);
    close(fd2);
    return 0;
}