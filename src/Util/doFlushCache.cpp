/* Compile me with -O0 or else you'll get none. */

#include <stdlib.h>
#include <stdio.h>
#define CACHE_FLUSH_BUFFER_SIZE  16*1024*1024

void do_flush(void);
void do_misses(int, int);
void dummy(void *array);

int buf[CACHE_FLUSH_BUFFER_SIZE];
int buf_dummy = 0;
volatile int *flush = NULL;
volatile int flush_dummy = 0;
volatile double a = 0.5, b = 2.2;

int main() {
  do_flush();
}

void do_misses(int n, int size)
{
  register int i, j, tmp = buf_dummy, len = size / sizeof(int);
  dummy(buf);
  for (j = 0; j < n; j++)
    {
      for (i = 0; i < len; i++)
      {
        buf[i] = tmp;
      }
      tmp += len;
    }
  buf_dummy = tmp;
  dummy(buf);
  dummy(&buf_dummy);
}

void do_flush(void)
{
  register int i;
  if (flush == NULL)
    flush = (int *)malloc((1024*1024*16)*sizeof(int));
  if ( !flush ) return;

  dummy((void *)flush);
  for (i=0;i<(1024*1024*16);i++)
    {
      flush[i] += flush_dummy;
    }
  flush_dummy++;
  dummy((void *)flush);
  dummy((void *)&flush_dummy);
}

void dummy(void *array)
{
  /* so that compiler will not do optimization */
}
