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
	if(argc < 2)
		printf("sleep: need an argumet\n");
	int s = atoi(argv[1]);
	printf("sleeping for %d seconds:\n", s);
	sleep(s);
	return 0;
}
