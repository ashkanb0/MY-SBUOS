#include <stdlib.h>
#include <stdio.h>



int main(int argc, char* argv[], char* envp[]);

// char buff [50];
// char* printing;
char* format;

void _start(long stack_top) {

	int argc=1;
	char** envp=0;
	char** argv=0;
	int res;


	argc = *((&stack_top)+2);
	argv = (char**)(&stack_top +3);
	envp = (char**)(&stack_top +4+argc);
	

	res = main(argc, argv, envp);
	exit(res);
}
