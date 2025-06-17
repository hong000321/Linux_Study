#include <stdio.h>
#include <unistd.h>

#include <sys/sem.h>

int cnt = 0;
static int semid;
void p(){
    struct sembuf pbuf;
    pbuf.sem_num = 0;
    pbuf.sem_op = -1;
    pbuf.sem_flg = SEM_UNDO;
    if(semop(semid, &pbuf,1) == -1){
        perror("p : semop()");
    }
}

void v(){
    struct sembuf vbuf;
    vbuf.sem_num = 0;
    vbuf.sem_op = 1;
    vbuf.sem_flg = SEM_UNDO;
    if(semop(semid, &vbuf, 1) == -1){
        perror("v : semop()");
    }
}

int main(int argc, char **argv){
    int key = 1125;
    union semun{
        int val;
        struct semid_ds *buf;
        unsigned short int *array;
    }arg;

    if((semid = semget(key, 1 ,IPC_CREAT | 0666)) == -1){
        perror("semget()");
        return -1;
    }

    arg.val = 0;
    if(semctl(semid, 0 , SETVAL, arg) == -1){
        perror("semctl() : SETVAL");
        return -1;
    }

    while(1){
        if(cnt >= 8){
            cnt--;
            arg.val--;
            p();
            printf("decrease : %d : %d\n",cnt, arg.val);
            break;
        } else{
            cnt++;
            arg.val++;
            v();
            printf("increase : %d : %d\n",cnt, arg.val);
            sleep(1);
        }
    }
    if(semctl(semid, 0, IPC_RMID, arg) ==-1){
        perror("semctl() : IPC_RMID");
        return -1;
    }
    return 0;
}