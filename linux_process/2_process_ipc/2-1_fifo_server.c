#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>


#define FIFOFILE "fifo"

void blockSignal(int signal){
    printf("block %d",signal);
}

int main(int argc, char **argv){
    int n, fd;
    char buf[BUFSIZ];

    unlink(FIFOFILE);

    if(mkfifo(FIFOFILE, 0666) < 0){
        perror("mkfifo()");
        return -1;
    }

    if((fd = open(FIFOFILE, O_RDONLY)) < 0){
        perror("open()");
        return -1;
    }

    while((n = read(fd, buf, sizeof(buf)))>0){
        buf[n] = '\0';
        printf("n = %d\n",n);
        printf("%s", buf);
    }
    close(fd);
    return 0;
}