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

void *Calloc(size_t n, size_t s)
{
  void *ptr = Malloc(n * s);
  memset(ptr, 0, n * s);
  return ptr;
}

void *Realloc(void *ptr, size_t s)
{
  if(s == 0) {
    free(ptr);
    return NULL;
  }
  ptr = realloc(ptr, s);
  if(ptr == NULL) {
    perror("realloc");
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
