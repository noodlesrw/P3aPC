#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv, char **envp) {
	int myArgs = 3;
	char *newargv[argc + myArgs];
	
	newargv[0] = argv[0];    
	newargv[1] = "-cpu";
	newargv[2] = "cortex-a8";
   
	memcpy(&newargv[myArgs], &argv[1], sizeof(*argv) * (argc - 1));    
	newargv[argc + myArgs -1] = NULL;
	return execve("/home/nathan/android/qemu-0.15.1/arm-linux-user/qemu-arm", newargv, envp);
}
