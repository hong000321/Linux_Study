#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int g_var = 1;
pthread_mutex_t mid;
void *inc_function(void *);
void *dec_function(void *);

int main(int argc, int **argv){
    pthread_t ptInc, ptDec;

    pthread_mutex_init(&mid, NULL);

    pthread_create(&ptInc,NULL,inc_function,NULL);
    pthread_create(&ptDec,NULL,dec_function,NULL);

    pthread_join(ptInc,NULL);
    pthread_join(ptDec,NULL);
    printf("main : %d\n", g_var);
    
    pthread_mutex_destroy(&mid);

    return 0;
}


void *inc_function(void *arg){
    pthread_mutex_lock(&mid);
    printf("Inc : %d < Before\n", g_var);
    sleep(0.1);
    g_var++;
    printf("Inc : %d > After\n",g_var);
    pthread_mutex_unlock(&mid);
    return NULL;
}

void *dec_function(void *arg){
    pthread_mutex_lock(&mid);
    printf("Dec : %d < Before\n", g_var);
    sleep(0.1);
    g_var--;
    printf("Dec : %d > After\n",g_var);
    pthread_mutex_unlock(&mid);
    return NULL;
}