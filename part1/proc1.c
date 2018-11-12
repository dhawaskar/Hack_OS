#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h> 
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/pid.h>
#include <linux/pid_namespace.h>
int p_id=9449;
struct pid *pid_struct;
struct task_struct *task;

int read_proc( void)
{
	pid_struct = find_get_pid(p_id);
	task = pid_task(pid_struct,PIDTYPE_PID);
	printk("\n***********name %s\n********",task->comm);
	return 0;
}


int proc_init (void) {
	read_proc();
    	return 0;
}

void proc_cleanup(void) {
	printk(KERN_INFO " Inside cleanup_module\n");
}

MODULE_LICENSE("GPL");   
module_init(proc_init);
module_exit(proc_cleanup);

