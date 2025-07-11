#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define TCP_PORT 5100


int main(int argc, char **argv){
    int sockfd , n;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char mesg[BUFSIZ];

    // 소켓 생성
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("socket()");
        return -1;
    }
    
    // 주소 구조체에 주소 지정하여 커널에 바인딩할 준비
    memset(&servaddr, 0 , sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(TCP_PORT);

    // 바인딩 설정(servaddr)에 맞춰 바인딩 진행
    if(bind(sockfd, (struct sockaddr * )&servaddr, sizeof(servaddr))<0){
        perror("bind()");
        return -1;
    }

    // 동시에 여러 클라이언트의 접속을 처리하기 위해 대기 큐 설정
    if(listen(sockfd, 2)<0){
        perror("listen()");
        return -1;
    }
    len = sizeof(cliaddr);
    do{
        int n, csock = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
        
        inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);
        printf("Client is connected : %s\n", mesg);
        if((n=read(csock, mesg, BUFSIZ))<=0){
            perror("read()");
        }
        printf("Received data : %s",mesg);

        if(write(csock, mesg, n)<=0)
            perror("write()");
        close(csock);
    }while(strncmp(mesg, "q", 1));
    close(sockfd);
    return 0;
}