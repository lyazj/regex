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

size_t bitset_hash(const bitset_t *s)
{
  size_t n64 = s->n / 64, m64 = s->n % 64, h = 0;
  if(m64) {
    h = s->data[n64] & (((uint64_t)1 << m64) - 1);
  } else {
    h = 0;
  }
  while(n64) h += s->data[--n64];  /* [XXX] */
  return h;
}

int bitset_equ(const bitset_t *s1, const bitset_t *s2)
{
  assert(s1->n == s2->n);
  for(size_t i = 0; i < bitset_bufsize(s1->n) / 8; ++i) {
    if(s1->data[i] != s2->data[i]) {
      size_t more;
      if((i + 1) * 64 <= s1->n) return 0;

      /* Need to strip redundant bits. */
      more = (i + 1) * 64 - s1->n;
      if((s1->data[i] ^ s2->data[i]) << more) return 0;
      return 1;
    }
  }
  return 1;
}

int bitset_cmp(const bitset_t *s1, const bitset_t *s2)
{
  assert(s1->n == s2->n);
  for(size_t i = 0; i < bitset_bufsize(s1->n) / 8; ++i) {
    if(s1->data[i] != s2->data[i]) {
      uint64_t u1 = s1->data[i];
      uint64_t u2 = s2->data[i];
      uint64_t ux;
      if((i + 1) * 64 > s1->n) {
        /* Need to strip redundant bits. */
        size_t more = (i + 1) * 64 - s1->n;
        u1 <<= more;
        u2 <<= more;
      }
      ux = u1 ^ u2;
      if(ux == 0) return 0;
      if(u1 & (ux & -ux)) return 1;
      return -1;
    }
  }
  return 0;
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

static size_t count_zero64(uint64_t u)
{
  size_t c = 0;
  while(u) ++c, u ^= u & -u;
  return c;
}

size_t bitset_count(const bitset_t *s)
{
  size_t n64 = s->n / 64, m64 = s->n % 64, c = 0;
  if(m64) {
    c = count_zero64(s->data[n64] & (((uint64_t)1 << m64) - 1));
  } else {
    c = 0;
  }
  while(n64) c += count_zero64(s->data[--n64]);
  return c;
}

void bitset_print(const bitset_t *s)
{
  size_t u;
  printf("[");
  if(bitset_get_first(s, &u)) {
    printf("%zu", u);
    while(bitset_get_next(s, &u)) {
      printf(",%zu", u);
    }
  }
  printf("]");
}
