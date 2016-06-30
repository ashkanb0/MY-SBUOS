#include <stdlib.h>
#include <stdio.h>

int atoi(const char* s){
	int res = 0;
	for (int i = 0; s[i] && i < 30; ++i)
	{
		res = (res*10)+(s[i] - '0');
	}
	return res;
}


int main(int argc, char const *argv[], char const *envp[])
{
	if(argc < 2){
		printf("sleep: need an argumet\n");
		return 1;
	}
	int pid = atoi(argv[1]);
	if(kill(pid , 9))
		printf("kill: no process found: %d\n", pid);
	return 0;
}
