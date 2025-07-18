KDIR = /usr/src/linux

obj-m := 1_gpio_irq_module.o

default:
	$(MAKE) -C $(KDIR) M=$$PWD modules

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
