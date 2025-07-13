/**
 * @file v4l2_server.c
 * @author Dae O Hong  (hong000321@gmail.com)
 * @brief v4l2를 사용하여 pi-camera의 영상을 frame단위로 client로 부터 받아 fb0에 출력하는 server
 * @version 0.1
 * @date 2025-07-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <linux/videodev2.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define FRAMEBUFFER_DEVICE  "/dev/fb0"  // 출력할 display frame buffer
#define WIDTH               640         // 640*480 크기의 v4l2 입력
#define HEIGHT              480

// RGB 값의 boundary check 를 위한 매크로
#define SATURATE_RGB(a) ((a)>255)?255:((a)<0)?0:(a)

// frame buffer의 정보가 담길 struct로 다양한 함수에서 사용할 예정이라 전역 변수로 설정
static struct fb_var_screeninfo vinfo;


/**
 * @brief server로써 동작하기 위해 필요한 설정을 진행하는 함수.
 * 
 * @param sockfd    : client가 server에 접속하기 위해 사용하는 socket file descriptor
 * @param address   : server가 client에 접속을 허용할 주소
 * @param port      : server가 client에 접속을 허용할 포트
 * @param csock     : 실제 영상 데이터를 받을 client socket file descriptor
 * @return int      : 에러 발생 시 -1, 정상 완료 시 0
 */
int init_socket(int *sockfd, const char *address, int port, int *csock)
{
    char data[BUFSIZ];
    socklen_t len;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    // 소켓 생성
    if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket()");
        return -1;
    }

    // 주소 구조체에 주소 지정하여 커널에 바인딩할 준비
    // AF_INET : IPv4 사용
    // s_addr에 문자열 address를 inet_addr함수를 사용하여 정수형으로 변환하여 입력
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, address, &(servaddr.sin_addr.s_addr));
    servaddr.sin_port = htobe16(port); // h(ost)tob(ig)e(ndian)16(bit) host 아키텍처의 엔디안에서 big endian 16bit 값으로 변환

    // 바인딩 설정(servaddr)에 맞춰 바인딩 진행
    if (bind(*sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind()");
        return -1;
    }

    // 동시에 여러 클라이언트의 접속을 처리하기 위해 대기 큐 설정
    if (listen(*sockfd, 2) < 0)
    {
        perror("listen()");
        return -1;
    }
    len = sizeof(cliaddr);
    
    // sockfd에서 client 접속이 확인 되면 해당 소켓을 csock 파일디스크립터에 저장
    *csock = accept(*sockfd, (struct sockaddr *)&cliaddr, &len);
    inet_ntop(AF_INET, &cliaddr.sin_addr, data, BUFSIZ); // 네트워크 주소를 문자열로 변환하여 data에 저장
    printf("Client is connected : %s\n", data);

    return 0;
}

/**
 * @brief client로 부터 받은 frame data를 display frame buffer에 출력하기 위해 fb 초기화
 * 
 * @param fb_fd     : display frame buffer의 file descriptor를 저장할 int변수
 * @param fbp       : display frame buffer의 fd에 하나씩 read write하면 kernel을 거치기 때문에 
 *                      속도가 느림으로 fbp라는 사용자영역에서 직접 접근가능한 fb 포인터로 만들어 출력
 * @return int      : 에러 발생 시 -1, 정상 완료 시 0
 */
int init_fb(int *fb_fd, uint16_t **fbp, uint32_t *screensize){
    // /dev/fb0를 open하여 해당 fd를 frame buffer file descriptor에 저장
    *fb_fd = open(FRAMEBUFFER_DEVICE, O_RDWR);
    if (*fb_fd == -1){
        perror("Error opening framebuffer device");
        return -1;
    }

    // 화면(fb)의 정보를 가져와서 vinfo 구조체의 주소에 결과를 저장
    // FBIOGET_VSCREENINFO 는 화면 가변정보를 가져오겠다는 명령 상수
    if (ioctl(*fb_fd, FBIOGET_VSCREENINFO, &vinfo)){
        perror("Error reading variable information");
        close(*fb_fd);
        return -1;
    }

    uint32_t fb_width = vinfo.xres;
    uint32_t fb_height = vinfo.yres;
    *screensize = fb_width * fb_height * vinfo.bits_per_pixel / 8;

    // read() write()를 통해 fb_fd로 직접 쓰고 읽을 수 있지만 이렇게 할 경우 kernel을 거쳐서
    // 쓰기, 읽기가 진행되기 때문에 동작이 느려짐
    // 따라서 mmap을 사용하여 커널이 가지고 있는 /dev/fb0의 메모리를
    // 사용자영역에서도 사용할 수 있게, fbp를 통하여 직접 접근이 가능하도록 만들어 준다.
    *fbp = mmap(0, *screensize, PROT_READ | PROT_WRITE, MAP_SHARED, *fb_fd, 0);
    if ((intptr_t)(*fbp) == -1)
    {
        perror("Error mapping framebuffer device to memory");
        close(*fb_fd);
        return -1;
    }

    return 0;
}

/**
 * @brief socket으로부터 받은 v4l2 raw frame data를 가공하여 16bit rgb(565)로 /dev/fb0 에 출력하는 함수
 * 
 * @param fbp       : display frame buffer의 fd에 하나씩 쓰면 속도가 느림으로 fbp라는 임시 array를 만들어 한번에 출력
 * @param data      : v4l2 raw frame data가 담긴 uint8_t 타입의 array
 * @param width     : 이미지 가로 크기 (WIDTH  = 640)
 * @param height    : 이미지 세로 크기 (HEIGHT = 480)
 */
void display_frame(uint16_t *fbp, uint8_t *data, int width, int height)
{
    int x_offset = (vinfo.xres - width) / 2;
    int y_offset = (vinfo.yres - height) / 2;

    // YUYV -> RGB565 변환하여 프레임버퍼에 출력
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; x += 2)
        {
            // yuv 공식에 따라 픽셀 2개의 데이터를 계산
            uint8_t Y1 = data[(y * width + x) * 2];
            uint8_t U = data[(y * width + x) * 2 + 1];
            uint8_t Y2 = data[(y * width + x + 1) * 2];
            uint8_t V = data[(y * width + x + 1) * 2 + 1];

            int R1 = Y1 + 1.402 * (V - 128);
            int G1 = Y1 - 0.344136 * (U - 128) - 0.714136 * (V - 128);
            int B1 = Y1 + 1.772 * (U - 128);

            int R2 = Y2 + 1.402 * (V - 128);
            int G2 = Y2 - 0.344136 * (U - 128) - 0.714136 * (V - 128);
            int B2 = Y2 + 1.772 * (U - 128);

            // 0~255 를 벗어나는 값들을 0혹은 255로 saturation 시킴
            R1 = SATURATE_RGB(R1);
            G1 = SATURATE_RGB(G1);
            B1 = SATURATE_RGB(B1);

            R2 = SATURATE_RGB(R2);
            G2 = SATURATE_RGB(G2);
            B2 = SATURATE_RGB(B2);

            // RGB565 포맷으로 변환 (R: 5비트, G: 6비트, B: 5비트)
            uint16_t pixel1 = ((R1 & 0xF8) << 8) | ((G1 & 0xFC) << 3) | (B1 >> 3);
            uint16_t pixel2 = ((R2 & 0xF8) << 8) | ((G2 & 0xFC) << 3) | (B2 >> 3);

            // frame buffer를 메모리에 직접 써서 픽셀 정보(RGB-2byte)를 입력
            fbp[(y + y_offset) * vinfo.xres + (x + x_offset)] = pixel1;
            fbp[(y + y_offset) * vinfo.xres + (x + x_offset + 1)] = pixel2;
        }
    }
}

int main(int argc, char **argv)
{

    // init network
    int sockfd = -1;
    int csock = -1;
    char *address = "127.0.0.1";
    int port = 5100;
    int ret = init_socket(&sockfd, address, port, &csock);
    if(ret<0){
        printf("Failed to connect soket\n");
        return -1;
    }

    // init v4l2 cam data buffer
    char *v4l2_data = malloc(WIDTH*HEIGHT*2);
    if (!v4l2_data) {
        perror("Failed to allocate buffer");
        return 1;
    }

    // init display frame buffer
    int fb_fd;
    uint16_t *fbp;
    uint32_t screan_size;
    ret = init_fb(&fb_fd, &fbp, &screan_size);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
    if(ret == -1){
        printf("frame buffer initialization is failed!!!\n");
        return -1;
    }

    // client에서 보낸 v4l2 camera image를 display_frame을 사용하여 frame buffer에 출력
    int stop = 0;
    while (1)
    {
        int ret, size=0;
        memset(v4l2_data,0,WIDTH*HEIGHT*2);
        while(1){
            // client로 부터 데이터 수신
            // client로 부터 받는 데이터는 실제 frame크기가 아닌 쪼개진 데이터일 수 있기 때문에
            // 받은 데이터 크기를 확인하여 실제 frame크기가 될때까지 받도록 while문을 돌림
            ret = read(csock, v4l2_data+size, WIDTH*HEIGHT*2);
            if (ret == -1) {
                perror("Failed to read frame");
                break;
            }
            if(ret == 0){
                stop = 1;
                break;
            }
            size += ret;
            if(size >= WIDTH*HEIGHT*2){
                break;
            }
        }
        if(stop){
            printf("client is disconnected\n");
            break;
        }
        // v4l2_data 읽어온 프레임 데이터를 처리
        printf("Captured frame size: %d bytes\n", size);
        display_frame(fbp, v4l2_data, WIDTH, HEIGHT);
    }

    // 모든 file descriptor와 버퍼들 해제
    free(v4l2_data);
    close(fb_fd);
    close(sockfd);
    close(csock);
    munmap(fbp,screan_size);

    return 0;
}