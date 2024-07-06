#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <err.h>

void *Malloc(size_t n)
{
  void *ptr = malloc(n);
  if(ptr == NULL) err(EXIT_FAILURE, "malloc");
  return ptr;
}

char *Strdup(const char *s)
{
  char *str = strdup(s);
  if(str == NULL) err(EXIT_FAILURE, "strdup");
  return str;
}

void Free(void *p)
{
  free(p);
}
