#pragma once
#include <stddef.h>

void *Malloc(size_t);
void *Calloc(size_t, size_t);
void *Realloc(void *, size_t);
char *Strdup(const char *);
void Free(void *);
