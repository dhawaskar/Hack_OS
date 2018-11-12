#include <linux/module.h>
#include <linux/dirent.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/fcntl.h>
#include <asm/errno.h>
#include <linux/kernel.h>
#include <linux/fs.h> 
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/pid.h>
#include <linux/pid_namespace.h>
#include <linux/init.h>
#include <linux/errno.h> 
#include <linux/types.h>
#include <linux/unistd.h>
#include <asm/cacheflush.h>  
#include <asm/page.h>  
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>
#include <linux/err.h>
#include <linux/scatterlist.h>
#include <linux/mutex.h>
#include <linux/syscalls.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/jiffies.h>


unsigned long **sys_call_table;
static unsigned long **find_sys_call_table(void);

static unsigned long **find_sys_call_table() {
    unsigned long offset;
    unsigned long **sct;
    
    int flag=0;

    for(offset = PAGE_OFFSET; offset < ULLONG_MAX; offset += sizeof(void *)) {
        sct = (unsigned long **) offset;
    
        if(sct[__NR_close] == (unsigned long *) sys_close)
              if( flag==0){
                 printk(KERN_ERR "find sys call table :%lx ",sct);
                 return sct;
                }else{
                printk(KERN_ERR "find first  sys call table :%llx ",sct);
                flag--;

        }

    }   
    return NULL;
}


asmlinkage long (*original_ni_sys_166)(void);
asmlinkage long addedsys_test(void) {
	int i=0;
	printk("system call 116 is intercepted\n");
	return 0;
}



static int ls_init(void) {
	int i=0;
	sys_call_table = find_sys_call_table();
	printk("Lets intercept system call present at 166 which is not used");
	original_ni_sys_166 = (void*)(sys_call_table[166]);
	write_cr0 (read_cr0 () & (~ 0x10000));
	sys_call_table[166]=(void *)&addedsys_test;
	write_cr0 (read_cr0 () | 0x10000);
        printk("\naddresss of sys_ni_syscall is changed\n");
        return 0;
}
 
static void ls_exit(void) {
	write_cr0 (read_cr0 () & (~ 0x10000));
        sys_call_table[166]=(void *)original_ni_sys_166;
        write_cr0 (read_cr0 () | 0x10000);
        printk("MODULE EXIT\n");
	return;
}
 
MODULE_LICENSE("GPL");   
module_init(ls_init);
module_exit(ls_exit);
