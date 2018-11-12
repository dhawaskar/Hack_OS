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

static unsigned long **find_sys_call_table(void) {
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


struct file * file_open(void) 
{
	struct file *filp = NULL;
    	mm_segment_t oldfs;
    	int err = 0;
    	oldfs = get_fs();
    	set_fs(get_ds());
    	filp = filp_open("address.txt",O_RDWR,0);
	set_fs(oldfs);
    	if (IS_ERR(filp)) {
        	err = PTR_ERR(filp);
		printk("Sorry file does not exists\nLets create one file");
		return NULL;
    	}
	else{
		printk("file exists\n");
		return filp;
	}
}

void file_close(struct file *fp){
	if(IS_ERR(fp)){
                printk("file pointer pointing to null");
                return;
        }
	filp_close(fp,NULL);
	printk("closed the file pointer\n");

}

int compute_size(struct file *file){
	char *data=NULL;
        int size=7975;
	int res;
        data=kmalloc(sizeof(char)*size,GFP_KERNEL);
        if(data==NULL){
                printk("Really sorry no kernel space is left\n");
                return 0;
        }
	if(IS_ERR(file)){
                printk("file pointer pointing to null");
                return 0;
        }
        loff_t pos=0;
        mm_segment_t oldfs;
        oldfs = get_fs();
        set_fs(get_ds());
        res=vfs_read(file, data, size,&pos);
        set_fs(oldfs);
        file->f_pos=pos;
        kfree(data);
        printk("Red %d bytes\n",res);
        return res;
}
void file_read(struct file *file,int size) 
{
	char *data=NULL;
	int i;
	int red_bytes;
	size=21;
	char tmp[16],tmp1[16];
	int flag=0;
	data=kmalloc(sizeof(char)*size,GFP_KERNEL);
	if(data==NULL){
		printk("Really sorry no kernel space is left\n");
		return;
	}
	loff_t pos=0;
   	mm_segment_t oldfs;
	oldfs = get_fs();
    	set_fs(get_ds());
	if(IS_ERR(file)){
		printk("file pointer pointing to null");
		return;
	}
	else {
		for(i=0;i<=MAX_SYSCALL;i++){
			if(i<=9){
				red_bytes=vfs_read(file, data,19,&pos);
				strncpy(tmp,data+2,16);
				printk("original address \t%s\n",tmp);
			}else if(i<=99){
				red_bytes=vfs_read(file, data,20,&pos);
				strncpy(tmp,data+3,16);
				printk("original address \t%s\n",tmp);
			}else if(i<=999){
				red_bytes=vfs_read(file, data,21,&pos);
				strncpy(tmp,data+4,16);
				printk("original address \t%s\n",tmp);
			}
			sprintf(tmp1,"%llx",address_array[i]);
			printk("**** new address is %s****\n",tmp1);
			if(strncmp(tmp,tmp1,16)!=NULL){
				printk("****************System call hack address is intercepted\t%s*******************\n",tmp);
				flag=1;
			}
			memset(tmp,'\0',16);
			memset(tmp1,'\0',16);
		}
		if(!flag){
			printk("###################   No system call is intercepted ##########################\n");
		}
		set_fs(oldfs);
		file->f_pos=pos;
		kfree(data);
		printk("Red the file pointer\n");
	}
}

int file_write(struct file *file) 
{
	int i;
	sys_call_table = find_sys_call_table();
	char str[25];
        printk("Address of systemcall table\t%llx",sys_call_table);
        printk("Lets store address of all system call entries\n");
    	mm_segment_t oldfs;
    	int ret;
	oldfs = get_fs();
    	set_fs(get_ds());
	unsigned long long offset=0;
	memset(str,0,22);
        for(i=0;i<=MAX_SYSCALL;i++){
		sprintf(str,"%d\t%llx\n",i,(void*)(sys_call_table[i]));
		printk("%s",str);
		ret = vfs_write(file,str, strlen(str), &offset);
		printk("Write %d bytes to file and string length %d\n",ret,strlen(str));
		memset(str,0,22);
        }
	set_fs(oldfs);
    	return ret;
}


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

static int hello_init(void)
{
	struct file *filp=file_open();
	int size,i;

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


	if(!filp){
		printk("create a file name address.txt for the module to work\n\n");		
		return 0;
	}	
	size=compute_size(filp);
	if(size==0){
		printk("You need to write into file first\n");
		if(file_write(filp)){
			printk("Wrote into the file \n");
			size=compute_size(filp);
			printk("Current file size is %d\n",size);
		}else printk("Sorry write was not successful\n");
	}else{
		printk("File total size is now %d",size);
		file_read(filp,size);
	}
	file_close(filp);
	return 0;
}

static void hello_exit(void) 
{  
	printk("Exit from module");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");   
