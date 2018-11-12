#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> 
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

asmlinkage long (*original_getpid)(void);

asmlinkage long new_getpid(void) {
	printk("***************************************GETPID HIJACKED***********************");
	printk("you modified the sytem call\n");
    	return original_getpid();
}


static int hijack_init(void) {
	printk(KERN_ALERT "\nHIJACK INIT\n");
	sys_call_table = find_sys_call_table();
	original_getpid = (void*)(sys_call_table[__NR_getpid]);
	printk(KERN_ERR "real_openat addr :%llx ",original_getpid);
  	printk(KERN_ERR "_NR_openat:%d ",__NR_getpid);
  	printk(KERN_ERR "hook_openat addr :%llx ",new_getpid);
	printk("Now lets change the address of getpid\n\n");
	write_cr0 (read_cr0 () & (~ 0x10000));
  	sys_call_table[__NR_getpid]=(void *)&new_getpid;
	write_cr0 (read_cr0 () | 0x10000);
	printk("\naddresss of getpid is changed\n");
 	return 0;
}
 
static void hijack_exit(void) {
 	write_cr0 (read_cr0 () & (~ 0x10000));
	sys_call_table[__NR_getpid]=(void *)original_getpid;
	write_cr0 (read_cr0 () | 0x10000);
 	printk("MODULE EXIT\n");
	return;
}
 
module_init(hijack_init);
module_exit(hijack_exit);
