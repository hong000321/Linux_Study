#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void signal_handler (int signum ){
    printf("Signal is Catched!!!\n");
    if(signum == SIGIO){
        printf("SIGIO\r\n");
        exit(1);
    }
}


int main(int argc, char **argv){
    char buf[BUFSIZ];
    char i = 0;
    int fd = -1;

    memset(buf, 0, BUFSIZ);

    signal(SIGIO, signal_handler);

    printf("GPIO Set : %s\n", argv[1]);

    fd = open("/dev/gpioled", O_RDWR);
    sprintf(buf, "%s:%d", argv[1], getpid());
    
    write(fd, argv[1], strlen(argv[1]));
    read(fd, buf, strlen(argv[1]));

    printf("Read data : %s\n", buf);
    close(fd);

    return 0;
}