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

    
    fd_set readfd;
    int maxfd, client_index, start_index;
    int client_fd[5] = {0};

    // 소켓 생성
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("socket()");
        return -1;
    }
    
    // 주소 구조체에 주소 지정하여 커널에 바인딩할 준비
    memset(&servaddr, 0 , sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[2]);//htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    // 바인딩 설정(servaddr)에 맞춰 바인딩 진행
    if(bind(sockfd, (struct sockaddr * )&servaddr, sizeof(servaddr))<0){
        perror("bind()");
        return -1;
    }

    // 동시에 여러 클라이언트의 접속을 처리하기 위해 대기 큐 설정
    if(listen(sockfd, 8)<0){
        perror("listen()");
        return -1;
    }
    FD_ZERO(&readfd);
    maxfd = sockfd;
    client_index = 0;
    
    do{
        // 읽기 동작 감지를 위한 fd_set 자료형 설정
        FD_SET(sockfd, &readfd);
        
        // 클라이언트의 시작 주소부터 마지막 주소까지 fd_set 자료형에 설정
        for(start_index=0; start_index<client_index; start_index++){
            FD_SET(client_fd[start_index], &readfd);
            if(client_fd[start_index] > maxfd)
                maxfd = client_fd[start_index];
        }
        maxfd = maxfd +1;

        // select() 함수에서 읽기가 가능한 부분만 조사
        select(maxfd, &readfd, NULL, NULL, NULL);
        if(FD_ISSET(sockfd, &readfd)){
            len = sizeof(struct sockaddr_in);

            // 클라이언트의 요청 받아들이기
            int csock = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
            if(csock < 0){
                perror("accept()");
                return -1;
            } else{
                // 네트워크 주소를 문자열로 변경
                inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);
                printf("Client is connected : %s\n", mesg);
                

                // 새로 접속한 클라이언트의 소켓 번호를 fd_set에 추가
                FD_SET(csock, &readfd);
                client_fd[client_index] = csock;
                client_index++;
                continue;
            }
            if(client_index == 5) break;
        }

        // 읽기 가능했던 소켓이 클라이언트였던 경우
        for(start_index=0; start_index<client_index; start_index++){
            // for 루프를 이용해서 클라이언트들을 모두 조사
            if(FD_ISSET(client_fd[start_index], &readfd)){
                memset(mesg, 0, sizeof(mesg));
                if((n=read(client_fd[start_index], mesg, sizeof(mesg)))>0){
                    printf("Received data : %s : %d", mesg, client_index);
                    int tmp_index;
                    
                    for(tmp_index=0; tmp_index<client_index; tmp_index++){
                        if(tmp_index==start_index)
                            continue;
                        write(client_fd[tmp_index], mesg, n);
                    }

                    // close(client_fd[start_index]);

                    // // 클라이언트 소켓을 지운다
                    // FD_CLR(client_fd[start_index], &readfd);
                    // client_index--;
                }
            }
        }
    }while(strncmp(mesg, "q", 1));


    close(sockfd);
    return 0;
}