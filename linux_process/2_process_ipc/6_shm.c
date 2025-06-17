#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>

#define SHM_KEY 0x12345

int main(int argc, char **argv){
    int i, pid, shmid;
    int *cVal;
    void *shmem = (void*)0;

    if((pid = fork()) ==0){
        shmid = shmget((key_t)SHM_KEY, sizeof(int), 0);
        if(shmid == -1){
            perror("shmget()");
            return -1;
        }

        shmem = shmat(shmid, (void*)0, 0666|IPC_CREAT);
        if(shmem == (void*)-1){
            perror("shmat()");
            return -1;
        }
        cVal = (int *)shmem;
        *cVal = 1;
        for(i = 0; i<3; i++){
            *cVal += 1;
            printf("Child(%d) : %d\n", i, *cVal);
            sleep(1.2);
        }
    } else if(pid >0){
        sleep(0.1);
        shmid = shmget((key_t)SHM_KEY, sizeof(int), 0666 | IPC_CREAT);
        if(shmid == -1){
            perror("shmget()");
            return -1;
        }

        shmem = shmat(shmid, (void*)0, 0);
        if(shmem == (void*)-1){
            perror("shmat()");
            return -1;
        }
        cVal = (int *)shmem;
        for(i=0; i<3; i++){
            sleep(1);
            printf("Parent(%d) : %d\n", i, *cVal);
        }
    }
    shmctl(shmid, IPC_RMID, 0);


    return 0;
}