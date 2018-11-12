#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc,char **argv){
	if(argc==1){
		printf("ls command implemention for current directory\n");
		struct stat path_stat;
		stat(".",&path_stat`);		
	}else{
		printf("ls command implementation for give file/directory\n");
	}
	return 0;
}
