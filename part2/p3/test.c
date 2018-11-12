#include <syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <linux/kernel.h>
#include <sys/time.h>

int main(int argc,char **argv){
	int pid;
	pid=syscall(113);
	printf("Return value from function is %d",pid);
	return 0;
}
