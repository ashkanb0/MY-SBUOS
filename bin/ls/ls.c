#include <stdlib.h>
#include <stdio.h>

#include <sys/dirent.h>

int main (int argc, char** argv)
{
  DIR *dp;
  struct dirent *ep;     
  dp = opendir ("./");

  if (dp != NULL)
  {
    ep = readdir (dp);
    while (ep){
      printf ("%s\n", ep->d_name);
        ep = readdir (dp);
    }

    closedir (dp);
  }
  else
    printf("%s: Couldn't open the directory\n", argv[0]);

  return 0;
}