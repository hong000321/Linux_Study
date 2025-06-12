//#include <linux/i2c-dev.h>
#include <stdio.h>
//#include <stdlib.h>
#include <linux/ioctl.h>
#include <linux/fcntl.h>



int main(void){

    int i2c_fd = open("/dev/i2c-1", O_RDWR);
    if(i2c_fd==0){
        printf("Fail to open i2c device\n");
        return 0;
    }

    return 0;
}



