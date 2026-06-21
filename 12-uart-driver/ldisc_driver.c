#include <linux/init.h>
#include <linux/module.h>
#include <linux/tty.h>
#include <linux/tty_ldisc.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Likith");
MODULE_DESCRIPTION("Simple UART Line Discipline Driver");

#define N_MYPROTO 25  /* custom ldisc number (must be unused) */

static int byte_count = 0;

/* Called when the ldisc is attached to a tty */
static int myproto_open(struct tty_struct *tty)
{
    byte_count = 0;
    printk(KERN_INFO "ldisc: attached to %s\n", tty->name);
    return 0;
}

/* Called when the ldisc is detached */
static void myproto_close(struct tty_struct *tty)
{
    printk(KERN_INFO "ldisc: detached from %s, total bytes seen = %d\n",
           tty->name, byte_count);
}

/* Called every time bytes arrive on the UART - this is the key hook */
static void myproto_receive_buf(struct tty_struct *tty, const u8 *cp,
                                 const u8 *fp, size_t count)
{
    size_t i;

    byte_count += count;
    printk(KERN_INFO "ldisc: received %zu byte(s), total = %d\n", count, byte_count);

    for (i = 0; i < count; i++)
        printk(KERN_INFO "ldisc: byte[%zu] = 0x%02x ('%c')\n",
               i, cp[i], (cp[i] >= 32 && cp[i] < 127) ? cp[i] : '.');
}

static struct tty_ldisc_ops myproto_ldisc = {
    .owner       = THIS_MODULE,
    .num         = N_MYPROTO,
    .name        = "myproto",
    .open        = myproto_open,
    .close       = myproto_close,
    .receive_buf = myproto_receive_buf,
};

static int __init ldisc_init(void)
{
    int ret = tty_register_ldisc(&myproto_ldisc);

    if (ret) {
        printk(KERN_ERR "ldisc: failed to register, error %d\n", ret);
        return ret;
    }

    printk(KERN_INFO "ldisc: registered as N_MYPROTO=%d\n", N_MYPROTO);
    printk(KERN_INFO "ldisc: attach with: ldattach %d /dev/ttyAMA1\n", N_MYPROTO);
    return 0;
}

static void __exit ldisc_exit(void)
{
    tty_unregister_ldisc(&myproto_ldisc);
    printk(KERN_INFO "ldisc: unregistered\n");
}

module_init(ldisc_init);
module_exit(ldisc_exit);
