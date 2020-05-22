#define PUSHSW_VERSION "0.1"

#include <linux/module.h>

#include <linux/types.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>

#include <asm/io.h>
#include <asm/uaccess.h>
//#include <asm/system.h>
#include <linux/errno.h>

#include <linux/obspushsw.h>

#define GPIO0_BASE	0xef600800	/* GPIO0 Base Address */
#define GPIO0_IR	0x1c		/* GPIO0 Input Register Offset */
#define GPIO0_SZ	4

#ifdef DEBUG
#define PSW_DEBUG(str...)       printk (KERN_INFO "pushsw: " str)
#else
#define PSW_DEBUG(str...)       /* nothing */
#endif

static u32 get_psw(void)
{
	u32 *gpio_ir;
	u32 val;

	if((gpio_ir = ioremap(GPIO0_BASE+GPIO0_IR, GPIO0_SZ)) == NULL){
		printk(KERN_ERR "%s: ioremap() return error\n", __func__);
		return -EIO;
	}
	val = in_be32(gpio_ir) & 0x00000001;
	iounmap(gpio_ir);

	return val ? 0 : 1;
}

static long psw_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case PSWIOC_GETSTATUS:
		return get_psw();
	default:
		break;
	}
	return (-ENOIOCTLCMD);
}

static int psw_open(struct inode *inode, struct file *file)
{
	switch (MINOR(inode->i_rdev)) {
	case PUSHSW_MINOR:
		return (0);
	default:
		return (-ENODEV);
	}
}

static int psw_release(struct inode *inode, struct file *file)
{
	return 0;
}

const struct file_operations pushsw_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl	=psw_ioctl,
	.open		=psw_open,
	.release	=psw_release,
};

static struct miscdevice pushsw_dev = {
	.minor	= PUSHSW_MINOR,
	.name	= "pushsw",
	.fops	= &pushsw_fops,
};

int __init psw_init(void)
{
	printk(KERN_INFO "Push switch driver v%s\n", PUSHSW_VERSION);
	return misc_register(&pushsw_dev);
}

void __exit psw_cleanup(void)
{
        misc_deregister( &pushsw_dev );
}

module_init(psw_init);
module_exit(psw_cleanup);
MODULE_LICENSE("GPL");
