#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define SAMPLE_FILE "sample1.txt"
int main(void){
    int fst_fd, snd_fd;
    int n;
    char buf[BUFSIZ];

    fst_fd = open(SAMPLE_FILE,O_RDWR|O_APPEND, 644);
    if(fst_fd==-1){
        printf("%s is not exist\n",SAMPLE_FILE);
        return -1;
    }
    snd_fd = open("sample2.txt",O_RDONLY|O_CREAT, 644);

    while((n =read(snd_fd,buf,sizeof(buf)))>0){
        write(fst_fd,buf,n);
    }

    close(fst_fd);
    close(snd_fd);
    return 0;
}