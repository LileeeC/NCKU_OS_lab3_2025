#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <asm/current.h>
#include <linux/uaccess.h>
#include <linux/sched.h>

#define procfs_name "Mythread_info"
#define BUFSIZE  1024
char buf[BUFSIZE]; //kernel buffer

static ssize_t Mywrite(struct file *fileptr, const char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/
    size_t to_copy = buffer_len;

    if (to_copy >= BUFSIZE) to_copy = BUFSIZE - 1;

    if (copy_from_user(buf, ubuf, to_copy))
        return -EFAULT;

    buf[to_copy] = '\0';
    return buffer_len;
    /****************/
}


static ssize_t Myread(struct file *fileptr, char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/
    int len = 0;
    unsigned long ms;
    char output_buf[BUFSIZE]; // 用來組合最終輸出的字串

    if (*offset > 0) return 0;

    // hint: current->utime/100/1000
    ms = current->utime / 100 / 1000;

    len = scnprintf(output_buf, BUFSIZE,
                    "%s\nPID: %d, TID: %d, time: %lu\n",
                    buf, current->tgid, current->pid, ms);

    if (len > buffer_len) len = buffer_len;
    if (copy_to_user(ubuf, buf, len)) return -EFAULT; // 將組合好的 output_buf 傳回給 User

    *offset += len;
    return len;
    /****************/
}

static struct proc_ops Myops = {
    .proc_read = Myread,
    .proc_write = Mywrite,
};

static int My_Kernel_Init(void){
    proc_create(procfs_name, 0644, NULL, &Myops);   
    pr_info("My kernel says Hi");
    return 0;
}

static void My_Kernel_Exit(void){
    pr_info("My kernel says GOODBYE");
}

module_init(My_Kernel_Init);
module_exit(My_Kernel_Exit);

MODULE_LICENSE("GPL");
