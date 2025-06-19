#include <stdio.h>
#include <string.h>
#include <unistd.h>
// #include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>


#define TCP_PORT 5100
#define MAX_EVENT 32

void setnonblocking(int fd){
    int opts = fcntl(fd, F_GETFL);
    opts |= O_NONBLOCK;
    fcntl(fd, F_SETFL, opts);
}

int main(int argc, char **argv){
    int ssock, csock;
    socklen_t clen;
    int n, epfd, nfds = 1;
    struct sockaddr_in servaddr, cliaddr;
    
    char mesg[BUFSIZ];

    int maxfd, client_index, start_index;
    int client_fd[5] = {0};

    // epoll 이벤트 처리를 위한 선언
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENT];

    // 소켓 생성
    if((ssock = socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("socket()");
        return -1;
    }

    // 서버를 nonblock 으로 설정
    setnonblocking(ssock);
    
    // 주소 구조체에 주소 지정하여 커널에 바인딩할 준비
    memset(&servaddr, 0 , sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(TCP_PORT);

    // 바인딩 설정(servaddr)에 맞춰 바인딩 진행 -> 커널에 등록
    if(bind(ssock, (struct sockaddr * )&servaddr, sizeof(servaddr))<0){
        perror("bind()");
        return -1;
    }

    // 동시에 여러 클라이언트의 접속을 처리하기 위해 대기 큐 설정
    if(listen(ssock, 8)<0){
        perror("listen()");
        return -1;
    }

    // epoll을 허용할 event수에 맞춰 커널에 등록
    epfd = epoll_create(MAX_EVENT);
    if(epfd == -1){
        perror("epoll_create()");
        return -1;
    }

    // 감시하고 싶은 서버 소켓 등록
    ev.events = EPOLLIN;
    ev.data.fd = ssock;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, ssock, &ev) == -1){
        perror("epoll_ctl()");
        return 1;
    }

    // 서버 시작
    clen = sizeof(struct sockaddr_in);
    do{
        // event유무 확인하는데 500ms 만큼 timeout걸림
        epoll_wait(epfd, events, MAX_EVENT, 500);

        // select와 마찬가지로 fd를 등록하고 수를 파악하기 위해 nfds 사용
        for(int i=0; i<nfds; i++){
            // 이벤트의 소켓이 서버이면 = client가 접속하면
            if(events[i].data.fd == ssock){
                csock = accept(ssock, (struct sockaddr*)&cliaddr, &clen);
                if(csock > 0){
                    // 네트워크(n)주소를 문자열(p)로 변경
                    inet_ntop(AF_INET,&cliaddr.sin_addr, mesg, BUFSIZ);
                    printf("Client is connected : %s\n", mesg);

                    // 클라이언트를 논블로킹으로 설정
                    setnonblocking(csock);

                    // 클라이언트 소켓의 이벤트 설정을 위한 임시이벤트 설정(ev)
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = csock;

                    // 임시이벤트를 등록
                    epoll_ctl(epfd, EPOLL_CTL_ADD, csock, &ev);
                    nfds++;
                    continue;
                }
            } else if(events[i].events & EPOLLIN){
                if(events[i].data.fd < 0) continue;
                memset(mesg, 0 , sizeof(mesg));
                if((n=read(events[i].data.fd, mesg, sizeof(mesg)))>0){
                    printf("Received data : %s", mesg);
                    write(events[i].data.fd, mesg,n);
                    close(events[i].data.fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    nfds--;
                }
            }
        }

    }while(strncmp(mesg, "q", 1));



    close(ssock);
    return 0;
}