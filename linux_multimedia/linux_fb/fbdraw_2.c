#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdint.h>


#define FBDEVICE "/dev/fb0"

typedef unsigned char ubyte;


struct fb_var_screeninfo vinfo;	/* 프레임 버퍼 정보 처리를 위한 구조체 */

unsigned short makepixel(unsigned char r, unsigned char g, unsigned char b) {
    return (unsigned short)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
}

static int drawpoint(int fd, int x, int y, unsigned short color)
{

    /* 색상 출력을 위한 위치 계산 : offset  = (X의_위치+Y의_위치x해상도의_넓이)x2  */
    // printf("%d, %d\n",x,y);
    int offset = (x + y*vinfo.xres)*2;
    lseek(fd, offset, SEEK_SET);
    write(fd, &color, 2);
    return 0;
}




static void drawline(int fd, int start_x, int end_x , int y, ubyte r, ubyte g, ubyte b)
{
	ubyte a = 0xFF;
	for(int x = start_x; x<end_x; x++){
		drawpoint(fd, x, y, makepixel(r,g,b));
	}
}

static void drawcircle(int fd, int center_x, int center_y, int radius, ubyte r, ubyte g, ubyte b){
    int x = radius, y=0;
    int radiusError = 1-x;

    while(x>=y){
        drawpoint(fd,  x+center_x,  y+center_y, makepixel(r, g, b));
        drawpoint(fd,  y+center_x,  x+center_y, makepixel(r, g, b));
        drawpoint(fd, -x+center_x,  y+center_y, makepixel(r, g, b));
        drawpoint(fd, -y+center_x,  x+center_y, makepixel(r, g, b));
        drawpoint(fd, -x+center_x, -y+center_y, makepixel(r, g, b));
        drawpoint(fd, -y+center_x, -x+center_y, makepixel(r, g, b));
        drawpoint(fd,  x+center_x, -y+center_y, makepixel(r, g, b));
        drawpoint(fd,  y+center_x, -x+center_y, makepixel(r, g, b));

        y++;
        if(radiusError <0){
            radiusError += 2*y +1;
        }else{
            x--;
            radiusError += 2*(y-x+1);
        }
    }
    
}

static void drawface(int fd, int start_x, int start_y, int end_x, int end_y, ubyte r, ubyte g, ubyte b){
    ubyte a = 0xFF;

    if(end_x == 0) end_x = vinfo.xres;
    if(end_y == 0) end_y = vinfo.yres;

    for(int y= start_y; y < end_y; y++){
        drawline(fd, start_x, end_x, y, r,g,b);
    }
}


static int drawpoint_mmap(ubyte *pfb, int x, int y, unsigned short color)
{
    if(x>vinfo.xres)
        return 0;
    if(y>vinfo.yres)
        return 0;
    *(pfb + x*2 + y*vinfo.xres*2 + 0) = (ubyte)(color);
    *(pfb + x*2 + y*vinfo.xres*2 + 1) = (ubyte)(color>>8);

    return 0;
}

static void drawline_mmap(ubyte *pfb, int start_x, int end_x , int y, ubyte r, ubyte g, ubyte b)
{
	for(int x = start_x; x<end_x; x++){
		drawpoint_mmap(pfb, x, y, makepixel(r,g,b));
	}
}

static void drawface_mmap(ubyte *pfb, int start_x, int start_y, int end_x, int end_y, ubyte r, ubyte g, ubyte b){
    if(end_x == 0) end_x = vinfo.xres;
    if(end_y == 0) end_y = vinfo.yres;

    for(int y= start_y; y < end_y; y++){
        drawline_mmap(pfb, start_x, end_x, y, r,g,b);
    }
}

// test ========================================================================================================================
static void drawPointWithRotate(int fd, int center_x, int center_y, int x, int y, int rotate, unsigned short color){
    double px, py;
    double len_x = x - center_x;
    double len_y = y - center_y;
    int plus_x=1, plus_y=1;
    if(len_x>=0 && len_y<0){ // 1사분면
        plus_x = 1;
        plus_y = 1;
    }else if(len_x<0 && len_y<=0){ // 2사분면
        plus_x = 1;
        plus_y = -1;
    }else if(len_x<=0 && len_y>0){ // 3사분면
        plus_x = -1;
        plus_y = -1;
    }else if(len_x>0 && len_y>=0){  // 4사분면
        plus_x = -1;
        plus_y = 1;
    }
    
    double len = sqrt(len_x*len_x + len_y*len_y);
    double r_rad = (rotate*M_PI)/180;
    double vec_len = 2*sin(r_rad/2)*len;
    
    // printf("r_rad(%f) : ",r_rad);
    px = x + sin(r_rad/2)*vec_len*plus_x;
    py = y + cos(r_rad/2)*vec_len*plus_y;

    drawpoint(fd, px,py,color);
    // printf("len(), vec_len(), ");
    // printf("px(%f), py(%f), rotate(%d), len(%f) cos(%f) sin(%f)\n",px,py,rotate,len,cos(r_rad),sin(r_rad));
}

static void drawLineWithRotate(int fd, int center_x, int center_y, int start_x, int end_x, int y, int rotate, unsigned short color){
    for(int x = start_x; x<end_x; x++){
		drawPointWithRotate(fd, center_x, center_y , x, y, rotate, color);
	}
}

static void drawRectWithRotate(int fd, int center_x, int center_y, int w, int h, int rotate, unsigned short color){
    //null
    int start_x = center_x - w/2;
    int end_x = center_x + w/2;
    int start_y = center_y - h/2;
    int end_y = center_y + h/2;
    for(int y=start_y; y<end_y; y++){
        drawLineWithRotate(fd, center_x, center_y, start_x, end_x, y, rotate, color);
    }
}


static void draw_korea_flag(){
    int s_x = 0;
    int s_y = 0;
    int w = 1920/3;
    int h = 1080;
}
// test ========================================================================================================================
// static void drawline(int fd, int start_x, int end_x , int y, ubyte r, ubyte g, ubyte b);
int main(int argc, char **argv)
{
    int fbfd, status, offset;
    ubyte *pfb;

    fbfd = open(FBDEVICE, O_RDWR); 	/* 사용할 프레임 버퍼 디바이스를 연다. */
    if(fbfd < 0) {
        perror("Error: cannot open framebuffer device");
        return -1;
    }

    /* 현재 프레임 버퍼에 대한 화면 정보를 얻어온다. */
    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        perror("Error reading fixed information");
        return -1;
    }
    
    pfb = (ubyte *)mmap(0, vinfo.xres *vinfo.yres * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
#define RUN_LEV 2
#if RUN_LEV==1
    drawpoint(fbfd, 50, 50, makepixel(255, 0, 0));            /*  Red 점을 출력 */
    drawpoint(fbfd, 100, 100, makepixel(0, 255, 0));        	/*  Green 점을 출력 */
    drawpoint(fbfd, 150, 150, makepixel(0, 0, 255));        	/*  Blue 점을 출력 */
#elif RUN_LEV==2
    drawline(fbfd,100,1000,200, 255,0,0);
    drawcircle(fbfd, 200, 200, 100, 255, 0, 255);


    int s_x = 0;
    int s_y = 0;
    int w = 1920/3;
    int h = 1080;
    // fd 그대로 사용할 경우 너무 느림
    // drawface(fbfd, s_x, s_y, s_x+w, s_y+h,255,255,255);
    // s_x += w;
    // drawface(fbfd, s_x, s_y, s_x+w, s_y+h,255,255,255);
    // s_x += w;
    // drawface(fbfd, s_x, s_y, s_x+w, s_y+h,255,255,255);

    // mmap으로 화면에 출력할때 버퍼로 접근해서 빠르게 출력
    // drawface_mmap(pfb, s_x, s_y, s_x+w, s_y+h,255,0,0);
    // s_x += w;
    // drawface_mmap(pfb, s_x, s_y, s_x+w, s_y+h,0,255,0);
    // s_x += w;
    // drawface_mmap(pfb, s_x, s_y, s_x+w, s_y+h,0,0,255);


    // 빈화면 출력 
    int center_x = 300;
    int center_y = 300;
    int start_x = 200;
    int end_x = 400;
    int angle = 45;
    int offset_y = 300;
    drawface_mmap(pfb, s_x, s_y, s_x+w*3, s_y+h,255,255,255);
    //static void drawLineWithRotate(int fd, int center_x, int center_y, int start_x, int end_x, int y, int rotate, unsigned int color){
    drawline(fbfd,200,400,300, 0,255,0);
    drawLineWithRotate(fbfd, center_x, center_y, start_x, end_x, offset_y, angle,makepixel(255, 0, 0));
    drawpoint(fbfd, center_x, center_y, makepixel(255, 0, 0));
    //static void drawRectWithRotate(int fd, int center_x, int center_y, int w, int h, int rotate, unsigned short color){
    // drawRectWithRotate(fbfd, 300,300, 200,100,45,makepixel(255, 0, 0));
    
#endif
    close(fbfd); 		/* 사용이 끝난 프레임 버퍼 디바이스를 닫는다. */

    return 0;
}
