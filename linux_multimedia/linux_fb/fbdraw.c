#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#define FBDEVICE "/dev/fb1"

typedef unsigned char ubyte;

struct fb_var_screeninfo vinfo;

static void drawpoint(int fd, int x, int y, ubyte r, ubyte g, ubyte b){
    ubyte a = 0xFF;

    int offset = (x + y*vinfo.xres)*vinfo.bits_per_pixel/8.;

    lseek(fd, offset, SEEK_SET);

    write(fd, &b, 1);
    write(fd, &g, 1);
    write(fd, &r, 1);
    write(fd, &a, 1);
}


int main(int argc, char **argv){
    int fbfd, status, offset;

    unsigned short pixel;

    fbfd = open(FBDEVICE, O_RDWR);
    if(fbfd <0){
        perror("Error: cannot open framebuffer device");
        return -1;
    }

    return 0;
}