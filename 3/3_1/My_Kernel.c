#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <asm/current.h>

#define procfs_name "Mythread_info"
#define BUFSIZE  1024
char buf[BUFSIZE];

static ssize_t Mywrite(struct file *fileptr, const char __user *ubuf, size_t buffer_len, loff_t *offset){
    /* Do nothing */
	return 0;
}


static ssize_t Myread(struct file *fileptr, char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/
    int len = 0;
    struct task_struct *t;

    // 檢查 offset，如果大於 0 表示已經讀取過，回傳 EOF
    if (*offset > 0) return 0; // EOF

    // 跑過所有thread
    for_each_thread(current, t) {
        if (len >= BUFSIZE) break;
        len += scnprintf(buf + len, BUFSIZE - len,
                        "PID: %d, TID: %d, Priority: %d, State: %ld\n",
                        t->tgid, t->pid, t->prio, t->__state);
        
    }

    if (copy_to_user(ubuf, buf, len)) return -EFAULT; // 將 Kernel buffer 資料複製到 User space
    *offset += len; // 更新 offset 並回傳長度
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