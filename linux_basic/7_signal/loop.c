#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void sigHd1(int no){
	printf("%d\n", no);
}

int main(void){
	signal(SIGINT,sigHd1);
	signal(SIGTSTP,sigHd1);
	printf("%d",sizeof(int));
	for(int i=0;;i++){
		printf("%10d\r",i);
		fflush(stdout);
		sleep(1);
	}
	return 0;
}

