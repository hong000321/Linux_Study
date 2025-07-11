#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define TCP_PORT 5100


int main(int argc, char **argv){
    int sockfd , n;
    socklen_t clisize;
    struct sockaddr_in servaddr, cliaddr;
    char mesg[BUFSIZ];
    char *address = "127.0.0.1";
    // if(argc < 2){
    //     printf("usage : %s <IP address>\n", address);
    //     return -1;
    // }
    // 소켓 생성
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("socket()");
        return -1;
    }

    // 소켓이 접속할 주소 지정
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    inet_pton(AF_INET, address, &(servaddr.sin_addr.s_addr));
    servaddr.sin_port = htons(TCP_PORT);

    //지정한 주소로 접속
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0){
        perror("connect()");
        return -1;
    }


    fgets(mesg, BUFSIZ, stdin);
    if(send(sockfd, mesg, BUFSIZ, MSG_DONTWAIT)<=0){
        perror("send()");
        return -1;
    }

    memset(mesg, 0 ,BUFSIZ);
    if(recv(sockfd, mesg, BUFSIZ, 0) <=0){
        perror("recv()");
        return -1;
    }
    printf("Received data : %s ", mesg);

    close(sockfd);


    return 0;
}