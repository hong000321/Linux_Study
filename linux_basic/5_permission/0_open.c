#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>

int main() {
    int fd = open("testfile", O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);
    close(fd);
    system("whoami");  
    
    return 0;
}