/*
 *  intrpt.c - An interrupt handler.
 *
 *  Copyright (C) 2001 by Peter Jay Salzman
 */

/* 
 * The necessary header files 
 */

/* 
 * Standard in kernel modules 
 */
#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>	/* Specifically, a module */
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>	/* We want an interrupt */
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/semaphore.h> 

#define MY_WORK_QUEUE_NAME "WQsched.c"

static int initialised = 0;
//static struct workqueue_struct *my_workqueue;
void tasklet_fn(unsigned long); 
DECLARE_TASKLET(my_tasklet,tasklet_fn, 1);

/*tasklet Function*/
void tasklet_fn(unsigned long arg)
{
      int i=0;

        printk(KERN_INFO "Executing Tasklet Function : arg = %ld\n", arg);
        printk(KERN_INFO "Executing tasklet Function\n");
        if( initialised%2 == 0 ){
            printk(KERN_INFO "key pressed \n");
        }else{
            printk(KERN_INFO "key released \n");
        }
    if( (initialised  >= 60) && ( initialised % 10 == 0 ) ){ 
        for(i=0; i<100; i++){
            printk(KERN_INFO "the loop counter : %d \n", i);
        }
        printk(KERN_INFO "Now calling msleep of 10 secs \n");
        msleep(10);
        printk(KERN_INFO "wakeup after calling msleep of 10secs \n");
    }
}
/* 
 * This will get called by the kernel as soon as it's safe
 * to do everything normally allowed by kernel modules.
 */
static void got_char(void *scancode)
{
	printk(KERN_INFO "Scan Code %x %s.\n",
	       (int)*((char *)scancode) & 0x7F,
	       *((char *)scancode) & 0x80 ? "Released" : "Pressed");
}

/* 
 * This function services keyboard interrupts. It reads the relevant
 * information from the keyboard and then puts the non time critical
 * part into the work queue. This will be run when the kernel considers it safe.
 */
static irqreturn_t my_irq_handler(int irq, void *dev_id)
{
	/* 
	 * This variables are static because they need to be
	 * accessible (through pointers) to the bottom half routine.
	 */
	//static int initialised = 0;
	static unsigned char scancode;
	static struct work_struct task;
	unsigned char status;
    int i=0;

	/* 
	 * Read keyboard status
	 */
	status = inb(0x64);
	scancode = inb(0x60);

    printk(KERN_INFO "My IRQ Handler invoked ; initialised_values : %d \n", initialised);
    /*
    if( (initialised  >= 60) && ( initialised % 10 == 0 ) ){ 
        for(i=0; i<100; i++){
            printk(KERN_INFO "the loop counter : %d \n", i);
        }
        printk(KERN_INFO "Now calling msleep of 10 secs \n");
        msleep(10);
        printk(KERN_INFO "wakeup after calling msleep of 10secs \n");
    }
    */
    
    initialised = initialised + 1;
	if (initialised == 0) {
		//INIT_WORK(&task, got_char, &scancode);
		initialised = 1;
	} else {
		//PREPARE_WORK(&task, got_char, &scancode);
	}

    initialised = initialised + 1;
	//queue_work(my_workqueue, &task);
    //schedule_work(&my_workqueue);
    tasklet_schedule(&my_tasklet);

	return IRQ_HANDLED;
}

/* 
 * Initialize the module - register the IRQ handler 
 */
int init_module()
{
	//my_workqueue = create_workqueue(MY_WORK_QUEUE_NAME);

	/* 
	 * Since the keyboard handler won't co-exist with another handler,
	 * such as us, we have to disable it (free its IRQ) before we do
	 * anything.  Since we don't know where it is, there's no way to
	 * reinstate it later - so the computer will have to be rebooted
	 * when we're done.
	 */
	free_irq(1, NULL);

	/* 
	 * Request IRQ 1, the keyboard IRQ, to go to our irq_handler.
	 * SA_SHIRQ means we're willing to have othe handlers on this IRQ.
	 * SA_INTERRUPT can be used to make the handler into a fast interrupt.
	 */
	return request_irq(1,	/* The number of the keyboard IRQ on PCs */
			   my_irq_handler,	/* our handler */
			   IRQF_SHARED, "test_keyboard_irq_handler",
			   (void *)(my_irq_handler));
}

/* 
 * Cleanup 
 */
void cleanup_module()
{
	/* 
	 * This is only here for completeness. It's totally irrelevant, since
	 * we don't have a way to restore the normal keyboard interrupt so the
	 * computer is completely useless and has to be rebooted.
	 */
	free_irq(1, NULL);
}

/* 
 * some work_queue related functions are just available to GPL licensed Modules
 */
MODULE_LICENSE("GPL");
                
