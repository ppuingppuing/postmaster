#include <linux/kernel.h>     

#include <linux/init.h>  // defines __init and __exit
#include <linux/module.h>

#include <linux/kthread.h>
#include <linux/jiffies.h>

#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/wait.h> //DECLARE_WAIT_QUEUE_HEAD(wq)
#include <linux/time.h>

#include <linux/param.h>
#include <linux/delay.h>

#include <linux/proc_fs.h> //[SSeonggeuni]
#include <asm/uaccess.h>
#include <linux/slab.h>

#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fcntl.h>

#include <linux/fs.h>
#include <asm/segment.h>
#include <linux/buffer_head.h>

#include<linux/file.h>

#define INTERVAL  600    // in jiffes, 0.5s for testing..

char buff[100];

int vval, ival;

int ecsl_cnt;

struct task_struct *th_id = NULL;

static struct timer_list mytimer;

static void mytimer_function(unsigned long ptr);
static void add_mytimer(void);
static void del_mytimer(void);

extern int ecsl_read_isoc(int *val);
extern int ecsl_read_vsoc(int *val);

static int rc;

char bufs[100];
mm_segment_t old_fs;
struct file *filp;
char *filename="/data/local/tmp/test.txt";


int my_idx;

void ecsl_log()
{
	loff_t pos = 0;
	printk("[ECSL] logging success\n");

	strcpy(bufs, "[ECSL] hello my logging system!\n");
    printk("keetark");
	//rc = filp->f_op->write(filp, bufs, rc, &filp->f_pos);
	//vfs_write(filp, bufs, strlen(bufs), &filp->f_pos);
//vfs_write(filp, bufs, strlen(bufs), &pos);

}

void mytimer_function(unsigned long ptr)
{

//	ecsl_read_vsoc(&vval);
//	ecsl_read_isoc(&ival);
	//ecsl_log();

    bufs[my_idx]='a';

    my_idx++;

    if(my_idx > 100)
        my_idx = 99;

	add_mytimer();

}

void add_mytimer(void)
{
	init_timer(&mytimer);
	mytimer.function = mytimer_function;

	mytimer.data = (unsigned long) 0;
	mytimer.expires = jiffies + INTERVAL; // periodic 
	add_timer(&mytimer);
}

void del_mytimer(void)
{		
	del_timer(&mytimer);
}

static int kthread_example_thr_fun(void *arg)
{
    add_mytimer();
    printk("[ECSL] myTimer start \n");
    return 0;
} 

/*
static const struct file_operations my_proc_fops = {
        .owner = THIS_MODULE,
	.read = ecslread,
};

*/


static int hello_init(void)
{
    printk("[ECSL] Hello Kernel World.... \n");

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    filp = filp_open(filename, O_RDWR|O_APPEND, 0666);


    //dir=proc_mkdir("ecsllog",NULL);
    //proc_create("vibatt",0666,dir,&my_proc_fops);
    //ecsl_log();
    if(th_id == NULL){ 
       th_id = (struct task_struct *)kthread_run(kthread_example_thr_fun, NULL, "kthread_example");
    }

    return 0;
}

static void hello_exit(void)
{
	
    rc = filp->f_op->write(filp, bufs, rc, &filp->f_pos);
    filp_close(filp, NULL);  
    set_fs(old_fs);

    del_mytimer();
    kthread_stop(th_id);
    th_id = NULL;



    printk("[ECSL] Goodbye Kernel World \n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SSeonggeuni");
MODULE_DESCRIPTION("ECSL Kernel Module test for Pixel3");
