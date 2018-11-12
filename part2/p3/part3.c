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
#define MAX_SYSCALL 384


unsigned long **sys_call_table;
unsigned long long *address_array; 
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


asmlinkage long (*original_ni_sys)(void);
asmlinkage long addedsys(void) {
	int i=0;
	printk("check if any system call is intercepted\n");
	for(i=0;i<=MAX_SYSCALL;i++){
		if(address_array[i]!=sys_call_table[i]){
		printk("System call at address %llx is modified by function with address %llx",address_array[i],sys_call_table[i]);
		}
	}
	printk("detection is over\n");
	return 0;
}



static int ls_init(void) {
	int i=0;
	sys_call_table = find_sys_call_table();
	printk("Address of systemcall table\t%llx",sys_call_table);
	address_array=kmalloc(sizeof(long long)*MAX_SYSCALL,GFP_KERNEL);
	if(address_array==NULL){
		printk("Memory allocation failed for array\n");
		return 0;
	}
	printk("Lets store address of all system call entries\n");
	for(i=0;i<=MAX_SYSCALL;i++){
		address_array[i]=(void*)(sys_call_table[i]);
	}
	printk("Entries are\n");
	for(i=0;i<=MAX_SYSCALL;i++){
               printk("%llx\n",address_array[i]);
        }
	printk("Lets intercept system call present at 113 which is not used");
	original_ni_sys = (void*)(sys_call_table[113]);
	write_cr0 (read_cr0 () & (~ 0x10000));
	sys_call_table[113]=(void *)&addedsys;
	write_cr0 (read_cr0 () | 0x10000);
        printk("\naddresss of sys_ni_syscall is changed\n");
        return 0;
}
 
static void ls_exit(void) {
	kfree(address_array);
	write_cr0 (read_cr0 () & (~ 0x10000));
        sys_call_table[113]=(void *)original_ni_sys;
        write_cr0 (read_cr0 () | 0x10000);
        printk("MODULE EXIT\n");
	return;
}
 
MODULE_LICENSE("GPL");   
module_init(ls_init);
module_exit(ls_exit);
