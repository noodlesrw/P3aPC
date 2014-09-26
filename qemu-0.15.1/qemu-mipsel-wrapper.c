#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv, char **envp) {
	int myArgs = 5;
	char *newargv[argc + myArgs];
	
	newargv[0] = argv[0];    
	newargv[1] = "-cpu";
	newargv[2] = "mips";
	newargv[3] = "-m";
	newargv[4] = "128";
   
	memcpy(&newargv[myArgs], &argv[1], sizeof(*argv) * (argc - 1));    
	newargv[argc + myArgs -1] = NULL;
	return execve("/home/nathan/android/qemu-0.15.1/mipsel-linux-user/qemu-mipsel", newargv, envp);
}
