#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <spawn.h>
extern char **environ; //으로도 가능

int system(const char *cmd){
    pid_t pid;
    int status;
    char *argv[] = {"sh", "-c", cmd, NULL};

    posix_spawn_file_actions_t actions;
    posix_spawnattr_t attrs;

    posix_spawn_file_actions_init(&actions);
    posix_spawnattr_init(&attrs);
    posix_spawnattr_setflags(&attrs, POSIX_SPAWN_SETSCHEDULER);

    posix_spawn(&pid, "/bin/sh", &actions, &attrs, argv, environ);
    posix_spawnattr_destroy(&attrs);
    // waitpid(pid, &status, 0);
    return status;
}

int main(int argc, char **argv, char **envp){
    while(*envp){
        printf("%s\n", *envp++);
    }
    system("who");
    system("nocommand");
    system("cal");
    return 0;
}