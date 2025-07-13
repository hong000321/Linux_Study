/**
 * @file v4l2_client.c
 * @author Dae O Hong  (hong000321@gmail.com)
 * @brief v4l2를 사용하여 pi-camera의 영상을 frame단위로 server에 보내는 client
 * @version 0.1
 * @date 2025-07-13
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define VIDEO_DEVICE        "/dev/video0"   // 출력할 display frame buffer
#define WIDTH               640             // 640*480 크기의 v4l2 입력
#define HEIGHT              480

/**
 * @brief client로써 동작하기 위해 필요한 설정을 진행하는 함수.
 * 
 * @param sockfd    : client가 server에 접속하기 위해 사용하는 socket file descriptor
 * @param address   : server의 주소
 * @param port      : server의 포트
 * @return int      : 에러 발생 시 -1, 정상 완료 시 0
 */
int init_socket(int *sockfd, const char *address, int port){
    int n;
    socklen_t clisize;
    struct sockaddr_in servaddr, cliaddr;

    if((*sockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("socket()");
        return -1;
    }

    // 소켓이 접속할 주소 지정
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    inet_pton(AF_INET, address, &(servaddr.sin_addr.s_addr));
    servaddr.sin_port = htons(port);

    //지정한 주소로 접속
    if(connect(*sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0){
        perror("connect()");
        return -1;
    }

    printf("connected to %s:%d\n",address, port);
    return 0;
}

/**
 * @brief 영상의 정보를 v4l2로 가져오기위해 v4l2를 초기화 하는 과정을 정의한 함수
 * 
 * @param fd            : video device를 open한 파일디스크립터를 저장할 변수
 * @param fmt           : v4l2 포맷을 저장할 구조체
 * @param v4l2_data     : 캡처한 이미지를 저장할 v4l2_data로 초기화 과정이 포함됨
 * @return int          : 에러 발생 시 -1, 정상 완료 시 0
 */
int init_v4l2(int *fd, struct v4l2_format *fmt, char **v4l2_data){
    *fd = open(VIDEO_DEVICE, O_RDWR);
    if (*fd == -1) {
        perror("Failed to open video device");
        return -1;
    }
    fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt->fmt.pix.width = WIDTH;
    fmt->fmt.pix.height = HEIGHT;
    fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt->fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (ioctl(*fd, VIDIOC_S_FMT, fmt) == -1) {
        perror("Failed to set format");
        close(*fd);
        return -1;
    }

    *v4l2_data = malloc(fmt->fmt.pix.sizeimage);
    printf("buffer addr = %p\n",*v4l2_data);
    if (!(*v4l2_data)) {
        perror("Failed to allocate v4l2_data");
        close(*fd);
        return -1;
    }
}

/**
 * @brief vl42로 읽은 프레임을 server로 전달하는 함수
 * 
 * @param sockfd    : 전달할 server socket
 * @param data      : 전달할 프레임 데이터
 * @param size      : 프레임의 크기
 * @return int      : 에러 발생 시 -1, 정상 완료 시 0
 */
int send_frame_to_server(int sockfd, uint8_t *data, int size) 
{
    if(send(sockfd, data, size,MSG_NOSIGNAL)<=0){
        perror("send()");
        return -1;
    }
    return 0;
}

int main() 
{
    // init network
    int sockfd;
    char *address = "127.0.0.1";
    int port = 5100;
    int ret = init_socket(&sockfd, address, port);
    if(ret == -1){
        printf("socket initialization is failed!! \n");
        return -1;
    }

    // init v4l2 cam
    int videofd;
    struct v4l2_format fmt;
    char *v4l2_data;
    ret = init_v4l2(&videofd, &fmt, &v4l2_data);
    if(ret == -1){
        printf("v4l2 initialization is failed!!!\n");
        return -1;
    }

    // v4l2로 camera에서 image frame data를 가져온 후 server로 전달하는 반복문
    while (1) {
        ret = read(videofd, v4l2_data, fmt.fmt.pix.sizeimage);
        if (ret == -1) {
            perror("Failed to read frame");
            break;
        }

        // buffer에 읽어온 프레임 데이터를 처리
        printf("Captured frame size: %d bytes\n", ret);
        ret = send_frame_to_server(sockfd, v4l2_data, ret); // server로 이미지 전송
        sleep(0.5);
        if(ret<0){
            return -1;
        }
    }

    // 모든 file descriptor와 버퍼들 해제
    free(v4l2_data);
    close(videofd);
    close(sockfd);
    printf("프로그램 종료!!\n");

    return 0;
}