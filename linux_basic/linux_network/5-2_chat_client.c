#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>


#define TCP_PORT 5100


int main(int argc, char **argv){
    int sockfd , n;
    socklen_t clisize;
    struct sockaddr_in servaddr, cliaddr;
    char mesg[BUFSIZ];
    char *address = "127.0.0.1";

    if(argc < 2){
        printf("usage : %s <Port> <IP address> <name>\n", address);
        return -1;
    }
    // 소켓 생성
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("socket()");
        return -1;
    }
    
    fd_set readfd;
    FD_ZERO(&readfd);
    // 소켓이 접속할 주소 지정
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    inet_pton(AF_INET, argv[2], &(servaddr.sin_addr.s_addr));
    servaddr.sin_port = htons(atoi(argv[1]));

    //지정한 주소로 접속
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0){
        perror("connect()");
        return -1;
    }
    char my_name[BUFSIZ];
    strcpy(my_name, argv[3]);
    do {
        FD_SET(sockfd, &readfd);
        FD_SET(0, &readfd);
        select(sockfd+1, &readfd, NULL, NULL, NULL);
        if(FD_ISSET(sockfd, &readfd)){
            memset(mesg, 0 ,BUFSIZ);
            if(recv(sockfd, mesg, BUFSIZ, 0) <=0){
                perror("recv()");
                return -1;
            }
            printf("Received data : %s ", mesg);
        }else if(FD_ISSET(0, &readfd)){
            fgets(mesg, BUFSIZ, stdin);
            strcat(my_name, mesg);
            if(send(sockfd, my_name, BUFSIZ, MSG_DONTWAIT)<=0){
                perror("send()");
                return -1;
            }
        }
        
    }while(strncmp(mesg, "q", 1));
    close(sockfd);
    return 0;
}