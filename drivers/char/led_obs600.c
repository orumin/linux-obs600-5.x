#define OBSLED_VERSION "0.2"

#include <linux/module.h>

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>
//#include <linux/smp_lock.h>

#include <asm/io.h>
#include <asm/uaccess.h>
//#include <asm/system.h>

#define GPIO0_BASE	0xef600800		/* GPIO0 Base Address */
#define GPIO0_OR	GPIO0_BASE + 0x0	/* GPIO0 Output Register */
#define GPIO0_SZ	32

#ifdef DEBUG
#define LED_DEBUG(str...)       printk (KERN_INFO str)
#else
#define LED_DEBUG(str...)       /* nothing */
#endif

static int obsled_out_reg(int led)
{
        volatile u32 *gpio_base;

        if((gpio_base = ioremap_nocache((u32)GPIO0_BASE, GPIO0_SZ)) == NULL){
                printk(KERN_ERR "%s: ioremap() return error\n", __func__);
                return -1;
        }

        if(led & 1)
                out_be32(gpio_base, in_be32(gpio_base) & ~0x80000000);
        else
                out_be32(gpio_base, in_be32(gpio_base) | 0x80000000);
        if(led & 2)
                out_be32(gpio_base, in_be32(gpio_base) & ~0x40000000);
        else
                out_be32(gpio_base, in_be32(gpio_base) | 0x40000000);
        if(led & 4)
                out_be32(gpio_base, in_be32(gpio_base) & ~0x20000000);
        else
                out_be32(gpio_base, in_be32(gpio_base) | 0x20000000);

        iounmap(gpio_base);
        return 0;
}

static ssize_t obsled_write(struct file *file, const char *buf,
		size_t count, loff_t *ppos)
{
	int err, i, led;

	if (count <= 0) {
		return 0;
        }

	for (i = 0; i < count; i++) {
		err = get_user(led, buf + i);
		if (err) {
			LED_DEBUG("%s: get_user() return error(err=%d)\n", __func__, err);
			return err;
		}
		if (led < '0' || '7' < led) {
			/* skip */
			continue;
		}
		err = obsled_out_reg(led);
		if (err) {
			LED_DEBUG("%s: obsled_out_reg() return error\n", __func__);
			return err;
		}
	}

	return count;
}

const struct file_operations obsled_fops = {
	.owner		= THIS_MODULE,
	.write		= obsled_write,
};

static struct miscdevice obsled_dev = {
	.minor	= SEGLED_MINOR,
	.name	= "segled",
	.fops	= &obsled_fops,
};

int __init obsled_init(void)
{
	printk(KERN_INFO "OBS600 LED driver v%s\n", OBSLED_VERSION);
	return misc_register(&obsled_dev);
}

void __exit obsled_cleanup(void)
{
        misc_deregister( &obsled_dev );
}

module_init(obsled_init);
module_exit(obsled_cleanup);
MODULE_LICENSE("GPL");

int obsled_out(int led)
{
	return obsled_out_reg(led);
}

