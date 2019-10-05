#include <linux/config.h>
#include <linux/module.h>
#include <linux/version.h>

#include <linux/ioport.h>

void skull_release(unsigned int port, unsigned int range)
{
   printk(KERN_INFO "skull_release  \n");
    release_region(port,range);
}

void skull_cleanup(void)
{
   printk(KERN_INFO "skull_cleanup  \n");
    /* should put real values here ... */
    /* skull_release(0,0); */
}

module_exit(skull_cleanup);




