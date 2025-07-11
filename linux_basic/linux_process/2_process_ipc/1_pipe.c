#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char **argv){
    pid_t pid;
    int pfd[2];
    char line[BUFSIZ];
    int status;
    
    if(pipe(pfd) <0){
        perror("pipe()");
        return -1;
    }

    if((pid=fork()) <0){
        perror("fork()");
        return -1;
    }else if(pid == 0){
        // close(pfd[0]);
        dup2(pfd[1],1);
        execl("/bin/date", "date", NULL);
        sleep(1);
        char tmp[BUFSIZ];
        if(read(pfd[0], tmp, BUFSIZ) <0){
            perror("read()");
            return -1;
        }
        printf("in CHILD : %s\n",tmp);
        // close(pfd[1]);
        _exit(127);
    }else{
        // close(pfd[1]);
        sleep(0.5);
        if(read(pfd[0], line, BUFSIZ) <0){
            perror("read()");
            return -1;
        }
        sleep(0.1);
        char tmp[BUFSIZ] = "test";
        write(pfd[1],tmp,5);
        printf("In PARENT : %s", line);
        // close(pfd[0]);
        waitpid(pid, &status, 0);
    }
    return 0;
}