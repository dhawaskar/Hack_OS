#include <syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <linux/kernel.h>

struct procinfo {
        pid_t pid;
        pid_t ppid;
        struct timespec start_time;
        int num_siblings;
};

int main(){
	struct procinfo p;
	struct timespec timer;
	printf("\n*****Process information of %d process*****\n",9449);
	int pid=syscall(113,9449,NULL);
	if(pid==-1){
		perror("getprocinfo");
		exit(-1);
	}
	printf("Return value is %d\n",pid);
	return 0;
}
