#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/system.h>

#ifdef DEBUG
#define LED_DEBUG(str...)	printk (KERN_INFO str)
#else
#define LED_DEBUG(str...)	/* nothing */
#endif

#define GPIO_DATA_OUT	0xD0018140
#define LED_RED			0x10000;
#define LED_YELLOW		0x40000;
#define LED_GREEN		0x100000;

void mvBoardDebugLed(unsigned int);

#define OBSLED_VERSION "0.2"

void obsled_out(int led)
{
	switch(led){
	case 0:
	case 1:
	case 6:
	case 7:
		break;
	case 2:
	case 3:
#ifdef CONFIG_MACH_OBSAX3
		led &= ~2;
		led |= 4;
#endif
		break;
	case 4:
	case 5:
#ifdef CONFIG_MACH_OBSAX3
		led &= ~4;
		led |= 2;
#endif
		break;
	default:
		return;
	}
	mvBoardDebugLed(~led);
}

static ssize_t obsled_write(struct file *file, const char *buf,
	size_t count, loff_t *ppos)
{
	int err, i, led;

	if (count <= 0)
		return 0;

	for (i = 0; i < count; i++) {
		err = get_user(led, buf + i);
		if (err) {
			LED_DEBUG("%s: get_user() return error(err=%d)\n", __func__, err);
			return err;
		}
		switch(led){
		case '0':
		case '1':
		case '6':
		case '7':
			break;
		case '2':
		case '3':
#ifdef CONFIG_MACH_OBSAX3
			led &= ~2;
			led |= 4;
#endif
			break;
		case '4':
		case '5':
#ifdef CONFIG_MACH_OBSAX3
			led &= ~4;
			led |= 2;
#endif
			break;
		default:
			continue;
		}
		mvBoardDebugLed(~led);
	}
	return count;
}

const struct file_operations obsled_fops = {
	.owner = THIS_MODULE,
	.write = obsled_write,
};

static struct miscdevice obsled_dev = {
	.minor = SEGLED_MINOR,
	.name  = "segled",
	.fops  = &obsled_fops,
};

int __init obsled_init(void)
{
	printk(KERN_INFO "OpenBlockS Family LED driver v%s\n", OBSLED_VERSION);
	return misc_register(&obsled_dev);
}

void __exit obsled_cleanup(void)
{
	misc_deregister( &obsled_dev );
}

module_init(obsled_init);
module_exit(obsled_cleanup);
MODULE_LICENSE("GPL");

