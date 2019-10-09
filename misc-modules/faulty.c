/*
 * faulty.c -- a module which generates an oops when read
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 * $Id: faulty.c,v 1.3 2004/09/26 07:02:43 gregkh Exp $
 */

#include <linux/module.h>
#include <linux/init.h>

#include <linux/kernel.h> /* printk() */
#include <linux/fs.h>     /* everything... */
#include <linux/types.h>  /* size_t */
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/slab.h>

MODULE_LICENSE("Dual BSD/GPL");


int faulty_major = 0;
static struct class *faulty_class = NULL;

ssize_t faulty_read(struct file *filp, char __user *buf,
		    size_t count, loff_t *pos)
{
	int i;
	int ret;
	char stack_buf[4]={'R','B','M','N'};

	printk(KERN_INFO "faulty_read  \n" );
	/* Let's try a buffer overflow  */
    /*
	for (i = 0; i < 20; i++)
		*(stack_buf + i) = 0xff;
    */
	if (count > 4)
		count = 4; /* copy 4 bytes to the user */
	ret = copy_to_user(buf, stack_buf, count);
	printk(KERN_INFO "faulty_read  the return value copy_to_user : %d   \n", ret );
	if (!ret)
		return count;
	return ret;
}

ssize_t faulty_write (struct file *filp, const char __user *buf, size_t count,
		loff_t *pos)
{
    char *d;
    char *m1;
    char *m2;

	/* make a simple fault by dereferencing a NULL pointer */
	printk(KERN_INFO "faulty_write  \n" );

    d = kmalloc( count , GFP_KERNEL );
	printk(KERN_INFO "faulty_write : allocated %d bytes  and   \n", count );
    //copy_from_user( d, buf, count );
    memcpy( d, buf, count );
	printk(KERN_INFO "faulty_write : copied the bytes %d bytes  \n", count );
	printk(KERN_INFO "faulty_write : The string is as In D  :%s   \n",  d );
	printk(KERN_INFO "faulty_write : Now copying through memcpy - kernel to Kernel buffer \n");
    m1 = kmalloc( count , GFP_KERNEL );
	printk(KERN_INFO "faulty_write : allocated %d bytes  for M1   \n", count );
    memcpy(m1, d, count );
	printk(KERN_INFO "faulty_write : copied the bytes %d bytes  \n", count );
	printk(KERN_INFO "faulty_write : The string is M1 :%s   \n",  d );
	//*(int *)0 = 0;
	printk(KERN_INFO "faulty_write : Now copying through memcpy - user to Kernel buffer \n");
    m2 = kmalloc( count , GFP_KERNEL );
	printk(KERN_INFO "faulty_write : allocated %d bytes  for M2   \n", count );
    memcpy(m2, buf, count );
	printk(KERN_INFO "faulty_write : copied the bytes %d bytes  \n", count );
	printk(KERN_INFO "faulty_write : The string is M2 :%s   \n",  d );
	return 0;
}



struct file_operations faulty_fops = {
	.read =  faulty_read,
	.write = faulty_write,
	.owner = THIS_MODULE
};


int faulty_init(void)
{
	int result;

	printk(KERN_INFO "faulty_init  \n" );
	/*
	 * Register your major, and accept a dynamic number
	 */
	result = register_chrdev(faulty_major, "faulty", &faulty_fops);
	if (result < 0)
		return result;
	if (faulty_major == 0)
		faulty_major = result; /* dynamic */

	faulty_class = class_create(THIS_MODULE, "faulty");
	if(faulty_class)
		device_create(faulty_class, NULL, MKDEV(faulty_major, 0), NULL, "faulty");

	return 0;
}

void faulty_cleanup(void)
{
	printk(KERN_INFO "faulty_cleanup  \n" );
	if(faulty_class)
	{
		device_destroy(faulty_class, MKDEV(faulty_major, 0));
		class_destroy(faulty_class);
	}
	unregister_chrdev(faulty_major, "faulty");
}

module_init(faulty_init);
module_exit(faulty_cleanup);

