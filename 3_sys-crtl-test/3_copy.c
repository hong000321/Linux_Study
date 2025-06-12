#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>


int main(void){
    int base_fd, out_fd;
    int n;
    char buf[BUFSIZ];

    base_fd = open("sample1.txt",O_RDONLY|O_CREAT, 644);
    
    out_fd = open("sample2.txt",O_WRONLY|O_CREAT|O_TRUNC, 644);

    while((n =read(base_fd,buf,sizeof(buf)))>0){
        write(out_fd,buf,n);
    }

    close(base_fd);
    close(out_fd);
    return 0;
}