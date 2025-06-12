#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
    int fd1, fd2;
    char str[BUFSIZ];
    int n = read(0, str, BUFSIZ);
    write(1, str, n);

    fd1 = open("sample1.txt", O_WRONLY | O_CREAT, 0644);
    fd2 = open("sample2.txt", O_WRONLY | O_CREAT, 0644);
    write(fd1, str, n);
    write(fd2, str, n);
    
    printf("%d %d\n",fd1, fd2);

    close(fd1);
    close(fd2);
    return 0;
}