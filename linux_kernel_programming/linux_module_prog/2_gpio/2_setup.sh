sudo insmod 2_gpio_module.ko
sudo mknod /dev/gpioled c 200 0
sudo chmod 666 /dev/gpioled

