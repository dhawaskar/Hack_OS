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
#include <linux/types.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/pid.h>


//int process_init(void);



int read_proc( void)
{
	int p_id=9449;
	struct pid *pid_struct;
	struct task_struct *task,*task1,*task2;
	unsigned long long start_time;
	int siblings=0;
        struct list_head * travel_ptr;
        pid_struct = find_get_pid(p_id);
        task = pid_task(pid_struct,PIDTYPE_PID);
	task2 = pid_task(pid_struct,PIDTYPE_PID);
        printk("current process: %s, PID: %d ", task->comm, task->pid);
        task1=task->parent;
        printk("\nParent process id is \t%d and its name \t%s\n",task1->pid,task1->comm);
        list_for_each(travel_ptr,&(task->sibling)){
                siblings++;
        }
        printk("\nNumber of siblins:\t%d\n",siblings);
        start_time=task2->start_time;
        printk("Start time of process is \t%llu",start_time);
        printk("Done printing all process iformation");
        return 0;
}


/*int process_init(void)
{
	int siblings=0;
	struct list_head * travel_ptr;
	struct task_struct *task = current; // getting global current pointer
	struct task_struct *task1 = current; // getting global current pointer
	struct task_struct *task2 = current; // getting global current pointer
	unsigned long long start_time;
	
	//current process
	printk("current process: %s, PID: %d ", task->comm, task->pid);
	task=task->parent;
	printk("\nParent process id is \t%d and its name \t%s\n",task->pid,task->comm);
	list_for_each(travel_ptr,&(task1->sibling)){
		siblings++;
	}
	printk("\nNumber of siblins:\t%d\n",siblings);
	start_time=task2->start_time;
	printk("Start time of process is \t%llu",start_time);
	printk("Done printing all process iformation");	
    	return 0;
}*/

static int hijack_init(void) {
        printk("\nprocess information\n");
	//process_init();
	read_proc();
        return 0;
}

static void hijack_exit(void) {
        printk("MODULE EXIT\n");
        return;
}

module_init(hijack_init);
module_exit(hijack_exit);
