#include <linux/module.h>
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



struct task_struct *task,*task1,*task2;
struct list_head * travel_ptr;
unsigned long long start_time;

struct procinfo {
	pid_t pid;
	pid_t ppid;
	struct timespec start_time;
	int num_siblings;
};

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


asmlinkage long (*original_ni_sys)(void);

asmlinkage long getprocinfo(int p_id,struct procinfo *inf) {
	struct pid *pid_struct;
	int siblings=0;
        printk(KERN_ALERT "NEW SYSTEM CALL ADDED VIA LKM");
	if(p_id>0){
		pid_struct = find_get_pid(p_id);
		if(pid_struct==NULL){
			printk(KERN_ERR "\nProcess you have asked does not exist\n");
			return -1*EINVAL;
		}
        	task = pid_task(pid_struct,PIDTYPE_PID);
	}else if (p_id==0){
		task=current;
	}
	else if(p_id<0){
		task=current;
		task=task->parent;
	}
	inf->pid=task->pid;
	task1=task;
	task2=task;
        task=task->parent;
	inf->ppid=task->pid;
        list_for_each(travel_ptr,&(task1->sibling)){
                siblings++;
        }
	inf->num_siblings=siblings;
        start_time=task2->start_time;
        printk("Start time of process is \t%llu",start_time);
	jiffies_to_timespec(start_time,&(inf->start_time));
        return 0;
}


static int systemcall_init(void) {
	printk(KERN_ALERT "\nADDING SYSTEM CALL\n");
        sys_call_table = find_sys_call_table();
        original_ni_sys = (void*)(sys_call_table[113]);
        write_cr0 (read_cr0 () & (~ 0x10000));
        sys_call_table[113]=(void *)&getprocinfo;
        write_cr0 (read_cr0 () | 0x10000);
        printk("\naddresss of sys_ni_syscall is changed\n");
        return 0;
}
 
static void systemcall_exit(void) {
 	printk("REMOVING ADDED SYSTEM CALL via LKM\n");
	write_cr0 (read_cr0 () & (~ 0x10000));                         
        sys_call_table[113]=(void *)original_ni_sys;                   
        write_cr0 (read_cr0 () | 0x10000);                             
        printk("MODULE EXIT\n");
	return;
}
 
MODULE_LICENSE("GPL");   
module_init(systemcall_init);
module_exit(systemcall_exit);
