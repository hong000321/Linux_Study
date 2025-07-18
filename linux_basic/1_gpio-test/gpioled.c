#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


int ledControl(int gpio, int value){
	int fd;
	char buf[BUFSIZ];
	char valBuf[BUFSIZ];
	sprintf(valBuf, "%d",value);

	fd = open("/sys/class/gpio/export", O_WRONLY);
	sprintf(buf, "%d", gpio);
	write(fd, buf, strlen(buf));
	close(fd);

	sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio);
	fd = open(buf, O_WRONLY);
	write(fd, "out", 4);
	close(fd);

	sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
	fd = open(buf, O_WRONLY);

	write(fd, valBuf,2);
	close(fd);

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	sprintf(buf, "%d", gpio);
	write(fd, buf, strlen(buf));
	close(fd);

	return 0;
}

int main(int argc, char **argv){
	int gno, val;
	if(argc < 3) {
		printf("Usage : %s <GPIO_NO> <1/0>\n", argv[0]);
		return -1;
	}
	gno = atoi(argv[1]);
	val = atoi(argv[2]);
	ledControl(gno,val);

	return 0;
}
