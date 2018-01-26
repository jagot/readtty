#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <string.h>
#include <stdlib.h>

// See https://stackoverflow.com/a/2918709
char read_byte(fd_set* set, int filedesc)
{
  struct timeval timeout;
  char c=-1;
  
  timeout.tv_sec = 3;
  timeout.tv_usec = 0;

  int rv = select(filedesc + 1, set, NULL, NULL, &timeout);
  if(rv == -1)
    perror("select\n"); /* an error accured */
  else if(rv == 0)
    printf("timeout\n"); /* a timeout occured */
  else
    read(filedesc, &c, 1); /* there was data to read */

  return c;
}

void shift(char* s, int l, int n)
{
  for(int i = 0; i<l-n; i++)
    s[i] = s[i+n];
}

int main(int argc, char** argv)
{
  setbuf(stdout, NULL);
  char* pattern = "a";
  char* filename = "/dev/random";
  if(argc > 1){
    filename = argv[1];
  }
  if(argc == 3){
    pattern = argv[2];
  }

  int l = strlen(pattern);
  char* buffer = malloc(l+1);
  buffer[l] = 0;
  
  fd_set set;
  int filedesc = open(filename, O_RDONLY);

  FD_ZERO(&set); /* clear the set */
  FD_SET(filedesc, &set); /* add our file descriptor to the set */

  for(;;){
    char c = read_byte(&set, filedesc);
    if(c < 0) break;
    shift(buffer, l, 1);
    buffer[l-1] = c;
    printf("%c", c);
    if(!strcmp(buffer, pattern))
      break;
  }

  free(buffer);

  return 0;
}
