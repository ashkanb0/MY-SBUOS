#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[], char* envp[])
{
    int j;
    printf("argc: -%d-\n", argc);

   for (j = 0; j < argc; j++)
        printf("argv[%d]: %s\n", j, argv[j]);

   for (j = 0; envp[j]; j++)
        printf("envp[%d]: %s\n", j, envp[j]);

   exit(0);
}

