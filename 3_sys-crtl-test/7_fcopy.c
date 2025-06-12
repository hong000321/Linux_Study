// #include <fcntl.h>
#include <stdio.h>
// #include <unistd.h>
#define FILE1 "sample1.txt"
#define FILE2 "sample2.txt"


int main(void){
    // int base_fd, out_fd;
    FILE *base_fd, *out_fd;
    int n;
    char buf[BUFSIZ];

    // base_fd = open("sample1.txt",O_RDONLY|O_CREAT, 644);
    base_fd = fopen(FILE1,"r");
    if(base_fd == nullptr){
        perror(FILE1);
        return -1;
    }

    // out_fd = open("sample2.txt",O_WRONLY|O_CREAT|O_TRUNC, 644);
    out_fd = fopen(FILE2,"w");
    if(out_fd == nullptr){
        perror(FILE2);
        return -1;
    }

    // while((n =read(base_fd,buf,sizeof(buf)))>0){
    //     write(out_fd,buf,n);
    // }
    while((n = fread(buf,sizeof(char),BUFSIZ,base_fd))>0){
        fwrite(buf,sizeof(char),n,out_fd);
    }

    fclose(base_fd);
    fclose(out_fd);
    return 0;
}