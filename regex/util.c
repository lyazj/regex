#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void *Malloc(size_t n)
{
  void *ptr;

  if(n == 0) return NULL;
  ptr = malloc(n);
  if(ptr == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  return ptr;
}

char *Strdup(const char *s)
{
  size_t len;
  char *d;

  if(s == NULL) return NULL;
  len = strlen(s);
  d = (char *)Malloc(len + 1);
  memcpy(d, s, len + 1);
  return d;
}

void Free(void *p)
{
  free(p);
}
