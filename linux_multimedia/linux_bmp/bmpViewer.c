#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

#include "bmpHeader.h"

#define FBDEVFILE "/dev/fb0"
#define LIMIT_UBYTE(n) (n>UCHAR_MAX)?UCHAR_MAX:(n<0)?0:n

typedef unsigned char ubyte;

extern int readBmp(char *filename, ubyte **pData, int *cols, int *rows);

unsigned short makepixel(unsigned char r, unsigned char g, unsigned char b) {
    return (unsigned short)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
}

int main(int argc, char **argv){
    int cols, rows, bmp_color = 24;
    int fb_pixel_size = 2;
    ubyte r, g, b, a = 255;

    ubyte *pData, *pBmpData, *pFbMap;
    struct fb_var_screeninfo vinfo;
    int fbfd;

    if(argc != 2){
        printf("Usage: ./%s xxx.bmp\n", argv[0]);
        return 0;
    }

    fbfd = open(FBDEVFILE, O_RDWR);
    if(fbfd < 0){
        perror("open()");
        return -1;
    }

    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) < 0){
        perror("ioctl() : FBIOGET_VSCREENINFO");
        return -1;
    }
    int fb_size = vinfo.xres * vinfo.yres * fb_pixel_size;
    int bmp_size = vinfo.xres * vinfo.yres * sizeof(ubyte) * bmp_color/8;
    
    // readBmp를 통해 가져온 raw image
    pData = (ubyte *)malloc(bmp_size);

    // raw image 를 fb에 전달하기 전 임시 저장소
    pBmpData = (ubyte *)malloc(fb_size);

    pFbMap = (ubyte *)mmap(0, fb_size, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

    if((unsigned)pFbMap == (unsigned)-1){
        perror("mmap()");
        return -1;
    }

    if(readBmp(argv[1], &pData, &cols, &rows) < 0){
        perror("readBmp()");
        return -1;
    }

    for(int y=0, bmp_index, fb_index; y < rows; y++){
        bmp_index = (rows-y-1)*cols*bmp_color/8;
        fb_index = y * vinfo.xres * fb_pixel_size;

        for(int x = 0; x<cols; x++){
            b = LIMIT_UBYTE(pData[bmp_index + x*bmp_color/8 + 0]);
            g = LIMIT_UBYTE(pData[bmp_index + x*bmp_color/8 + 1]);
            r = LIMIT_UBYTE(pData[bmp_index + x*bmp_color/8 + 2]);

            unsigned short fb_color = makepixel(r,g,b);

            *(pBmpData + x*fb_pixel_size + fb_index + 0) = (ubyte)fb_color;
            *(pBmpData + x*fb_pixel_size + fb_index + 1) = (ubyte)(fb_color>>8);
        }
    }
    memcpy(pFbMap, pBmpData, fb_size);

    munmap(pFbMap, fb_size);
    free(pBmpData);
    free(pData);
    close(fbfd);

    return 0;
}