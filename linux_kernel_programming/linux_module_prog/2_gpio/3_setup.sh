sudo rm -rf /dev/gpioled
sudo rmmod 3_gpiofunction_module
sudo insmod 3_gpiofunction_module.ko
sudo mknod /dev/gpioled c 200 0
sudo chmod 666 /dev/gpioled

