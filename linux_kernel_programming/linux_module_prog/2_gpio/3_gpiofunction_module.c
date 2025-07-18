#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/uaccess.h>


#define USE_GPIO_H 1
#if USE_GPIO_H // gpio.h
#include <linux/gpio.h>
#endif


MODULE_LICENSE("GPL");
MODULE_AUTHOR("DaeO Hong");
MODULE_DESCRIPTION("Raspberry Pi GPIO LED Device Module");

#if 0
#define BCM_IO_BASE 0x20000000    /* Raspberry Pi B/B+의 I/O Peripherals 주소 */
#define BCM_IO_BASE  0x3F000000 /* Raspberry Pi 2/3의 I/O Peripherals 주소 */
#else
#define BCM_IO_BASE 0xFE000000 /* Raspberry Pi 4의 I/O Peripherals 주소 */
#endif
#define GPIO_BASE (BCM_IO_BASE + 0x200000)
#define GPIO_SIZE  (256) /* GPIO 컨트롤러의 주소 */
/* 0x7E2000B0 – 0x7E2000000 + 4 = 176 + 4 = 180 */

/* GPIO 설정 매크로 */
#define GPIO_IN(g) (*(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))) /* 입력 설정 */
#define GPIO_OUT(g) (*(gpio+((g)/10)) |= (1<<(((g)%10)*3))) /* 출력 설정 */

#define GPIO_SET(g) (*(gpio+7) = 1<<g) /* 비트 설정 */
#define GPIO_CLR(g) (*(gpio+10) = 1<<g) 
#define GPIO_GET(g) (*(gpio+13)&(1<<g)) /* 설정된 비트 해제 */

// 디바이스 파일의 주 번호와 부 번호
#define GPIO_MAJOR  200
#define GPIO_MINOR  0
#define GPIO_DEVICE "gpioled"

#if USE_GPIO_H==0 // not use gpio.h 
#define GPIO_LED    18
/* 현재 GPIO의 비트에 대한 정보 획득 */
volatile unsigned *gpio; /* I/O 접근을 위한 volatile 변수 */
#else
#define GPIO_LED    512+18
#endif

static char msg[BLOCK_SIZE] = {0};

static int gpio_open(struct inode *, struct file *);
static ssize_t gpio_read(struct file *, char *, size_t, loff_t *);
static ssize_t gpio_write(struct file *, const char *, size_t, loff_t *);
static int gpio_close(struct inode *, struct file *);

static struct file_operations gpio_fops = {
    .owner = THIS_MODULE,
    .read = gpio_read,
    .write = gpio_write,
    .open = gpio_open,
    .release = gpio_close,
};


struct cdev gpio_cdev;

int init_module(void){
    dev_t devno;
    unsigned int count;
#if USE_GPIO_H==0// not use gpio.h 
    static void *map;
#endif
    
    int err;

    printk(KERN_INFO "Hello GPIO module!!!\n");


    devno = MKDEV(GPIO_MAJOR, GPIO_MINOR);
    register_chrdev_region(devno, 1, GPIO_DEVICE);

    cdev_init(&gpio_cdev, &gpio_fops);

    gpio_cdev.owner = THIS_MODULE;
    count = 1;
    err = cdev_add(&gpio_cdev, devno, count);
    if(err < 0){
        printk("Error : Device Add\n");
        return -1;
    }

    printk("'sudo mknod /dev/%s c %d 0'\n", GPIO_DEVICE, GPIO_MAJOR);
    printk("'sudo chmod 666 /dev/%s'\n", GPIO_DEVICE);

#if USE_GPIO_H==0// not use gpio.h 
    map = ioremap(GPIO_BASE, GPIO_SIZE);
    if(!map){
        printk("Error : mapping GPIO memory\n");
        iounmap(map);
        return -EBUSY;
    }
    
    gpio = (volatile unsigned int *)map;

    GPIO_IN(GPIO_LED);
    GPIO_OUT(GPIO_LED);
#else
    gpio_request(GPIO_LED, "gpioled");
    gpio_direction_output(GPIO_LED, 1);
#endif
    return 0;
}

void cleanup_module(void){
    dev_t devno = MKDEV(GPIO_MAJOR, GPIO_MINOR);
    unregister_chrdev_region(devno, 1);

    cdev_del(&gpio_cdev);

#if USE_GPIO_H==0 // not use gpio.h 
    if(gpio){
        iounmap(gpio);
    }
#else
    gpio_free(GPIO_LED);
    gpio_direction_output(GPIO_LED, 0);
#endif

    printk(KERN_INFO "Good-bye GPIO Module!!! \n");
}

static int gpio_open(struct inode *inod, struct file *fil){
    try_module_get(THIS_MODULE);
    printk("GPIO Device opened(%d/%d)\n", imajor(inod), iminor(inod));
    return 0;
}

static int gpio_close(struct inode *inod, struct file *fil){
    module_put(THIS_MODULE);
    printk("GPIO Device closed(%d)\n", MAJOR(fil->f_path.dentry->d_inode->i_rdev));
    return 0;
}

static ssize_t gpio_read(struct file *inode, char *buff, size_t len, loff_t *off){
    int count;
    strcat(msg, " from Kernel");
    count = copy_to_user(buff, msg, strlen(msg)+1);

    printk("GPIO Device(%d) read : %s(%d)\n", MAJOR(inode->f_path.dentry->d_inode->i_rdev), msg, count);

    return count;
}

static ssize_t gpio_write(struct file *inode, const char *buff, size_t len, loff_t *off){
    short count;
    memset(msg, 0, BLOCK_SIZE);
    count = copy_from_user(msg, buff, len);

#if USE_GPIO_H==0 // not use gpio.h 
    (!strcmp(msg, "0"))?GPIO_CLR(GPIO_LED):GPIO_SET(GPIO_LED);
#else
    gpio_set_value(GPIO_LED, (!strcmp(msg, "0"))?0:1);
#endif
    printk("GPIO Device(%d) write : %s(%ld)\n", MAJOR(inode->f_path.dentry->d_inode->i_rdev), msg, len);

    return count;
}
