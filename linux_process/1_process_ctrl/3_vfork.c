#include <stdio.h>
#include <unistd.h>


static int g_var = 1;
char str[] = "PID";


int main(int argc, char ** argv){

    int var;
    pid_t pid;
    var = 88;
    printf("Start From PID=%d\n", getpid());
    if((pid = vfork()) <0){
        perror("[ERROR] : vfork()");
    }else if(pid == 0){
        g_var++;
        var++;
        printf("Parent %s from Child Process(%d) : %d\n", str, getpid(), getppid());
        printf("pid = %d, Global var = %d , var = %d\n", getpid(), g_var , var);
        _exit(0);
    } else{
        g_var++;
        var++;
        printf("Child %s from Parent Process(%d) : %d\n", str, getpid(), pid);
    }

    printf("pid = %d, Global var = %d , var = %d\n", getpid(), g_var , var);
    
    return 0;
}