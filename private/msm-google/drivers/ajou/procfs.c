#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

static ssize_t file_rw_write(struct file *flip, const char *buff, size_t count, loff_t *offp);
static ssize_t file_rw_read(struct file *flip, char * buff, size_t count, loff_t *offp);

#define MAX_STR (4096)
static char str_data[MAX_STR];
static int str_len;

static struct proc_dir_entry *ajou_dir, *file_rw;

static const struct file_operations file_rw_fops = { 
    .read = file_rw_read,
    .write = file_rw_write,
};

static int file_rw_init(void){
    ajou_dir = proc_mkdir("Ajou", NULL);
    file_rw = proc_create("file_rw", 0660, ajou_dir, &file_rw_fops);
    return 0;
}
module_init(file_rw_init);

static struct proc_dir_entry *file_printk;

static ssize_t file_printk_read(struct file *flip, char * buff, size_t count, loff_t *offp);

static const struct file_operations file_printk_fops = { 
    .read = file_printk_read,
};

static int file_printk_init(void){
    file_printk = proc_create("file_printk", 0660, ajou_dir, &file_printk_fops);
    return 0;
}
module_init(file_printk_init);

static struct proc_dir_entry *file_dumpstack;

static ssize_t file_dumpstack_read(struct file *flip, char * buff, size_t count, loff_t *offp);
static const struct file_operations file_dumpstack_fops = { 
    .read = file_dumpstack_read,
};

static int file_dumpstack_init(void){
    file_dumpstack = proc_create("file_dumpstack", 0660, ajou_dir, &file_dumpstack_fops);
    return 0;
}
module_init(file_dumpstack_init);

static struct proc_dir_entry *file_ftrace;

static ssize_t file_ftrace_read(struct file *flip, char * buff, size_t count, loff_t *offp);
static const struct file_operations file_ftrace_fops = { 
    .read = file_ftrace_read,
};

static int file_ftrace_init(void){
    file_ftrace = proc_create("file_ftrace", 0660, ajou_dir, &file_ftrace_fops);
    return 0;
}
module_init(file_ftrace_init);

static ssize_t file_rw_write(struct file *flip, const char *buff, size_t count, loff_t *offp){
    int len;
    
    copy_from_user((void *)str_data, (void *)buff, count);

    str_data[count] = '0';
    str_len = count + 1;

    return count + 1;
}

static ssize_t file_rw_read(struct file *flip, char * buff, size_t count, loff_t *offp){
    int len;

    if (offp[0]!=0){
        offp[0] = 0;
        return 0;
    }
    
    len = count > str_len ? str_len : count;
  
    copy_to_user((void *)buff, (void *)str_data, len);

    *offp += len;
    
    return len;
}

static ssize_t file_printk_read(struct file *flip, char * buff, size_t count, loff_t *offp){
    printk("[Ajou] %s, %d, %s", __FILE__, __LINE__, __FUNCTION__);
    return 0;
}

static ssize_t file_dumpstack_read(struct file *flip, char * buff, size_t count, loff_t *offp){
    dump_stack();
    return 0;
}

#include <linux/delay.h>

noinline static int func3(void){
    udelay(5);
    return 0;
}
noinline static int func2(void){
    udelay(5);
    return 0;
}
noinline static int func1(void){
    func2();
    return 0;
}
static ssize_t file_ftrace_read(struct file *flip, char * buff, size_t count, loff_t *offp){
    func1();
    func3();
    return 0;
}


#include <linux/smp.h>

static struct proc_dir_entry *file_pmuserenr;

static ssize_t file_pmuserenr_read(struct file *flip, char * buff, size_t count, loff_t *offp);

static const struct file_operations file_pmuserenr_fops = { 
    .read = file_pmuserenr_read,
};

int pmuserenr_init(void)
{
    file_rw = proc_create("file_pmuserenr", 0660, ajou_dir, &file_pmuserenr_fops);
	return 0;
}

module_init(pmuserenr_init);

static void pmuserenr_wr(void *arg)
{
    u64 reg;
    asm volatile("msr PMUSERENR_EL0, %0"::"r"((u64)0xF));
    asm volatile("mrs %0, PMUSERENR_EL0":"=r"(reg));
    printk("[Ajou] %s : 0x%llx, core: %d", __FUNCTION__, reg, smp_processor_id());
    return;
}

static ssize_t file_pmuserenr_read(struct file *flip, char * buff, size_t count, loff_t *offp){
    on_each_cpu_mask(cpu_online_mask, pmuserenr_wr, (void *)NULL, 1);
    return 0;
}


