#include <stdlib.h>
#include <stdio.h>

int strcpy(const char* s, char* t){
	int i;
	for(i=0; s[i]; i++){
		t[i] = s[i];
		if (i>=1024)return -1;
	}
	t[i]=0;
	return 0;
}



char buffer [1024];


int main(int argc, char const *argv[], char const *envp[])
{
	int i = 0, fd = 0;
	if (argc>=2){
		strcpy(argv[1], buffer);
		fd = open(buffer, O_RDONLY);
		// printf("%x\n", fd);
		if (fd<0){
			printf("cat: %s: Failed to open.\n", argv[1]);
			return -1;
		}
	}
	while(1){
		i = read(fd, buffer, 1024);
		if (i<=0){
			break;
		}
		printf("%s", buffer);
	}
	close(fd);
	return 0;
}
