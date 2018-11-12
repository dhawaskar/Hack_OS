#include <syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <linux/kernel.h>
#include <sys/time.h>
struct procinfo {
        pid_t pid;
        pid_t ppid;
        struct timespec start_time;
        int num_siblings;
};

int main(int argc,char **argv){
	if(argc<2){
		printf("Invalid usage of command: <%s> <pid>",argv[0]);
		exit(0);
	}
	int pid_t=atoi(argv[1]);
	struct procinfo p;
	struct timespec timer;
	struct timeval start,end;
	double elapsed=0.0;
	// get infomration about the process of given ID=9449
	printf("\n*****Process information of %d process*****\n",9449);
	gettimeofday(&start,NULL);
	int pid=syscall(113,pid_t,&p);
	gettimeofday(&end,NULL);
	elapsed=(end.tv_sec-start.tv_sec)+((end.tv_usec-start.tv_usec)/1000000.0);
	printf("total time takes is %lf",elapsed);
	if(pid==-1){
		perror("getprocinfo");
		exit(-1);
	}
	printf("Return value is %d\n",pid);
	printf("\nPID of the process is\t %d\nparent id is \t%d\nnumber of siblings\t%d\n",p.pid,p.ppid,p.num_siblings);
	timer=p.start_time;
	printf("\nIts running since %ld sec and %ld nsec\n",timer.tv_sec,timer.tv_nsec);
	//get information about the current process
	printf("\n*****process information of current process****\n");
	printf("I am process %d\n",getpid());
	gettimeofday(&start,NULL);
	pid=syscall(113,0,&p);
	gettimeofday(&end,NULL);
	elapsed=(end.tv_sec-start.tv_sec)+((end.tv_usec-start.tv_usec)/1000000.0);
        printf("total time takes is %lf",elapsed);
	printf("\nPID of the current process is\t %d\nparent id is \t%d\nnumber of siblings\t%d\n",p.pid,p.ppid,p.num_siblings);
 	timer=p.start_time;
 	printf("Its running since %ld sec and %ld nsec\n",timer.tv_sec,timer.tv_nsec);
	//get information about the parent process
	printf("\n****Get process iformation of parent process****\n");
	printf("I am process %d\n",getpid());
	gettimeofday(&start,NULL);
        pid=syscall(113,-1,&p);
	gettimeofday(&end,NULL);
	elapsed=(end.tv_sec-start.tv_sec)+((end.tv_usec-start.tv_usec)/1000000.0);
        printf("total time takes is %lf",elapsed);
        printf("\nPID of the current process is\t %d\nparent id is \t%d\nnumber of siblings\t%d\n",p.pid,p.ppid,p.num_siblings);
        timer=p.start_time;
        printf("Its running since %ld sec and %ld nsec\n",timer.tv_sec,timer.tv_nsec);
	return 0;
}
