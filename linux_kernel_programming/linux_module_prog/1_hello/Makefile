KDIR = /lib/modules/`uname -r`/build

CC = make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- 


obj-m := hello_module.o

default:
	$(CC) -C $(KDIR) M=$$PWD modules

clean:
	$(CC) -C $(KDIR) M=$$PWD clean
