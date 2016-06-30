#include <stdlib.h>
#include <stdio.h>
#include <sys/ps_helpers.h>

pspcb buffer [50];

int main(int argc, char const *argv[], char const *envp[])
{
	int pcount = get_ps(buffer, 50);
	printf("PID    PPID    NAME\n");
	for (int i = 0; i < pcount; ++i)
	{
		printf("%d     %d      %s\n", buffer[i].pid, buffer[i].ppid, buffer[i].pname);
	}
}
