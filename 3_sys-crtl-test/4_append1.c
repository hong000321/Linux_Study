#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>


int main(void){
    int dest_fd, fst_fd, snd_fd;
    int n;
    char buf[BUFSIZ];

    dest_fd = open("sample.txt",O_RDWR|O_CREAT, 644);
    fst_fd = open("sample1.txt",O_RDONLY|O_CREAT, 644);
    snd_fd = open("sample2.txt",O_RDONLY|O_CREAT, 644);

    while((n =read(fst_fd,buf,sizeof(buf)))>0){
        write(dest_fd,buf,n);
    }
    while((n =read(snd_fd,buf,sizeof(buf)))>0){
        write(dest_fd,buf,n);
    }

    close(dest_fd);
    close(fst_fd);
    close(snd_fd);
    return 0;
}