KDIR = /usr/src/linux

obj-m := 2_gpio_module.o 3_gpiofunction_module.o

default:
	$(MAKE) -C $(KDIR) M=$$PWD modules

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
