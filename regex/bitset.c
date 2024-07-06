#include "bitset.h"
#include "util.h"
#include <assert.h>
#include <stdio.h>

void bitset_create(bitset_t *bitset, size_t n)
{
  bitset->n = n;
  bitset->data = (uint64_t *)Malloc(bitset_bufsize(n));
}

void bitset_destroy(bitset_t *bitset)
{
  bitset->n = 0;
  Free(bitset->data);
  bitset->data = NULL;
}

int bitset_equ(const bitset_t *s1, const bitset_t *s2)
{
  assert(s1->n == s2->n);
  for(size_t i = 0; i < bitset_bufsize(s1->n) / 8; ++i) {
    if(s1->data[i] != s2->data[i]) {
      if((i + 1) * 64 <= s1->n) return 0;

      /* Need to strip redundant bits. */
      unsigned more = (i + 1) * 64 - s1->n;
      if((s1->data[i] ^ s2->data[i]) << more) return 0;
      return 1;
    }
  }
  return 1;
}

int bitset_get_first(const bitset_t *s, size_t *up)
{
  for(size_t i = 0; i < bitset_bufsize(s->n) / 8; ++i) {
    uint64_t u64 = s->data[i];
    size_t u = i * 64;
    if(!u64) continue;
    while(!(u64 & 1)) {
      ++u;
      u64 >>= 1;
    }
    if(u >= s->n) return 0;
    *up = u;
    return 1;
  }
  return 0;
}

int bitset_get_next(const bitset_t *s, size_t *up)
{
  size_t i = (*up + 1) / 64;
  size_t j = (*up + 1) % 64;
  uint64_t u64 = s->data[i] >> j;
  size_t u;
  for(; i < bitset_bufsize(s->n) / 8; ++i) {
    if(u64) {
      while(!(u64 & 1)) {
        ++j;
        u64 >>= 1;
      }
      u = i * 64 + j;
      if(u >= s->n) return 0;
      *up = u;
      return 1;
    }
    if(i + 1 < bitset_bufsize(s->n) / 8) {
      j = 0;
      u64 = s->data[i + 1];
    }
  }
  return 0;
}

void bitset_print(const bitset_t *s)
{
  printf("[");
  size_t u;
  if(bitset_get_first(s, &u)) {
    printf("%zu", u);
  }
  while(bitset_get_next(s, &u)) {
    printf(",%zu", u);
  }
  printf("]");
}
