// #include <fcntl.h>
#include <stdio.h>
// #include <unistd.h>
#define FILE1 "sample1.txt"
#define FILE2 "sample2.txt"
#define FILE3 "sample.txt"


int main(void){
    // int base_fd, out_fd;
    FILE *base1_fd, *base2_fd, *out_fd;
    int n;
    char buf[BUFSIZ];

    // base_fd = open("sample1.txt",O_RDONLY|O_CREAT, 644);
    base1_fd = fopen(FILE1,"r");
    if(base1_fd == NULL){
        perror(FILE1);
        return -1;
    }
    base2_fd = fopen(FILE2,"r");
    if(base2_fd == NULL){
        perror(FILE2);
        return -1;
    }

    // out_fd = open("sample2.txt",O_WRONLY|O_CREAT|O_TRUNC, 644);
    out_fd = fopen(FILE3,"w");
    if(out_fd == NULL){
        perror(FILE3);
        return -1;
    }

    while((n = fread(buf,sizeof(char),BUFSIZ,base1_fd))>0){
        fwrite(buf,sizeof(char),n,out_fd);
    }
    while((n = fread(buf,sizeof(char),BUFSIZ,base2_fd))>0){
        fwrite(buf,sizeof(char),n,out_fd);
    }

    fclose(base1_fd);
    fclose(base2_fd);
    fclose(out_fd);

    return 0;
}