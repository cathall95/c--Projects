#include <stdio.h>
#include <stdlib.h>

int main()
{
  char c;
  FILE *from, *to;

  from = fopen("file.txt", "r");
  to = fopen("copy.txt", "w+");
  if (from == NULL)
  {
    perror("file.txt");
    exit(1);
  }

  /* file exists, so start reading */
  while ((c = getc(from)) != EOF)
   {
    fputc(c, to);
   }

  fclose(from);
  fclose(to);
  exit(0);
}
