#include <stdio.h>
#include <string.h>
#include <unistd.h>
// #include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>


#define TCP_PORT 5100
#define MAX_EVENT 32
#define DEBUG 1
#define MAX_NAME 100
#if DEBUG==2
    #define dprint(...) printf(__VA_ARGS__)
    #define iprint(...) printf(__VA_ARGS__)
#elif DEBUG==1
    #define dprint(...) printf(__VA_ARGS__)
    #define iprint(...)
#else //DEBUG==0
    #define dprint(...)
    #define iprint(...)
#endif
int stop = 0;
struct client_s {
    int fd;
    char name[MAX_NAME];
};

void signalStop(){
    stop = 1;
}

void setnonblocking(int fd){
    int opts = fcntl(fd, F_GETFL);
    opts |= O_NONBLOCK;
    fcntl(fd, F_SETFL, opts);
}

int findClientIndexByFd(int fd, struct client_s clients[], int csize){
    for(int i=0; i<csize; i++){
        if(fd == clients[i].fd){
            return i;
        }
    }
}

void deleteClient(struct client_s clients[], int index, int *nfds){
    shutdown(clients[index].fd,SHUT_RDWR);
    close(clients[index].fd);
    clients[index].fd=0;
    
    clients[index].fd = clients[(*nfds)-1].fd;
    memset(clients[index].name,0,MAX_NAME);
    strcpy(clients[index].name,clients[(*nfds)-1].name);
    
    (*nfds)--;
    dprint("client %d is deleted\n",index);
    dprint("nfds = %d\n",*nfds);
}

void deleteAllClient(struct client_s clients[], int *nfds){
    int orig_nfds = *nfds;
    for(int i=orig_nfds-1; i>=0; i--){
        deleteClient(clients,i,nfds);
    }
    dprint("\n");
    for(int i=0; i<MAX_EVENT; i++){
        if(i%10==0){
            dprint("\n");
        }
        dprint("%d ",clients[i].fd);
    }
    dprint("\n");
}

void broadcastChat(char tmp[], int inputfd, struct client_s clients[], int *nfds, int ssock){
    for(int j=0; j<*nfds; j++){
        dprint("curr(%d) events fd = %d,   client fd = %d\n",j,inputfd,clients[j].fd);
        if((clients[j].fd==0) || (inputfd==ssock) || (inputfd==clients[j].fd)){
            iprint("skip !!!!!\n");
            continue;
        }
        iprint("write!!!!!\n");
        // dprint("socket event ID = %d\n",j);
        int ret = send(clients[j].fd, tmp, BUFSIZ, MSG_NOSIGNAL);
        dprint("====== ret = %d\n",ret);
        if(ret<0){
            perror("send() ");
            deleteClient(clients,j,nfds);
            continue;
        }
    }
}

int main(int argc, char **argv){
    int ssock, csock;
    socklen_t clen;
    int n, epfd, nfds = 0;
    struct sockaddr_in servaddr, cliaddr;
    int port;
    char *address="192.168.2.95";
    char mesg[BUFSIZ];

    // epoll 이벤트 처리를 위한 선언
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENT];
    struct client_s clients[MAX_EVENT];

    if(argc == 2){
        port = atoi(argv[1]);
    }else if(argc == 1){
        port = 5101;
    }else if(argc == 3){
        address = argv[1];
        port = atoi(argv[2]);
    }else{
        printf("usage : %s <Port>\n", argv[0]);
        printf("usage : %s <IP address> <Port>\n", argv[0]);
        return -1;
    }

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
    servaddr.sin_addr.s_addr = inet_addr(address);//htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);//htons(TCP_PORT);
    printf("Server Net = %s:%d\n",address,port);
    

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
    nfds++;
    signal(SIGINT,signalStop);
    signal(SIGTERM,signalStop);
    // 서버 시작
    clen = sizeof(struct sockaddr_in);
    do{
        
        // event유무 확인하는데 500ms 만큼 timeout걸림
        int ev_size = epoll_wait(epfd, events, MAX_EVENT, 500);

        // select와 마찬가지로 fd를 등록하고 수를 파악하기 위해 nfds 사용
        for(int i=0; i<ev_size; i++){
            // 이벤트의 소켓이 서버이면 = client가 접속하면
            if(events[i].data.fd == ssock){
                dprint("1 event i = %d\n",i);
                csock = accept(ssock, (struct sockaddr*)&cliaddr, &clen);
                if(csock > 0){
                    // 네트워크(n)주소를 문자열(p)로 변경
                    inet_ntop(AF_INET,&cliaddr.sin_addr, mesg, BUFSIZ);

                    // 클라이언트를 논블로킹으로 설정
                    setnonblocking(csock);

                    // 클라이언트 소켓의 이벤트 설정을 위한 임시이벤트 설정(ev)
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = csock;
                    clients[nfds].fd = csock;

                    // 임시이벤트를 등록
                    epoll_ctl(epfd, EPOLL_CTL_ADD, csock, &ev);
                    sleep(0.1);
                    printf("Client is connected : %s (%d)\n", mesg,nfds);
                    char tmp[BUFSIZ];
                    memset(tmp, 0 , sizeof(tmp));
                    strcat(tmp, "이름을 입력하세요 : \n");
                    
                    // dprint("clients[i].name[0] = %d",clients[i].name[0]);
                    write(ev.data.fd, tmp,31);
                    nfds++;
                    continue;
                }
            } else if(events[i].events & EPOLLIN){
                dprint("2 event i = %d ,   fd = %d\n",i,events[i].data.fd);
                if(events[i].data.fd <= 0){
                    continue;
                }
                memset(mesg, 0 , sizeof(mesg));
                if((n=read(events[i].data.fd, mesg, sizeof(mesg)))>0){
                    char tmp[BUFSIZ];
                    memset(tmp, 0 , sizeof(tmp));
                    int cindex = findClientIndexByFd(events[i].data.fd, clients, nfds);
                    dprint("find fd %d(%d) in cindex %d(%d)\n",events[i].data.fd,i,cindex,clients[cindex].fd);
                    if(clients[cindex].name[0] == 0){
                        strcpy(clients[cindex].name,mesg);
                        printf("client %d name is %s\n",cindex,mesg);
                        sprintf(tmp, "%s 님이 입장하였습니다.",clients[cindex].name);

                        dprint("before nfds = %d \n",nfds);
                        broadcastChat(tmp, events[i].data.fd, clients, &nfds, ssock);
                        
                        // dprint("size = %d\n",n);
                        // clients[i].name[n-1]=0;
                        continue;
                    }
                    
                    
                    strcat(tmp,clients[cindex].name);
                    strcat(tmp, " : ");
                    strcat(tmp, mesg);
                    dprint("%s\n", tmp);
                    dprint("before nfds = %d \n",nfds);
                    broadcastChat(tmp, events[i].data.fd, clients, &nfds, ssock);
                    
                    // write(events[i].data.fd, mesg,n);
                    
                    // close(events[i].data.fd);
                    // epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    // nfds--;
                }
            }
        }
    }while(!stop);

    deleteAllClient(clients,&nfds);
    shutdown(ssock,SHUT_RDWR);
    close(ssock);
    
    return 0;
}