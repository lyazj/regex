#include "charset.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

unsigned charset_count(const charset_t *s)
{
  unsigned n = 0;
  for(unsigned i = 0; i < 4; ++i) {
    uint64_t u64 = s->data[i];
    while(u64) {
      ++n;
      u64 ^= u64 & -u64;
    }
  }
  return n;
}

int charset_get_first(const charset_t *s, unsigned char *ucp)
{
  for(unsigned i = 0; i < 4; ++i) {
    uint64_t u64 = s->data[i];
    unsigned char uc = i * 64;
    if(!u64) continue;
    while(!(u64 & 1)) {
      ++uc;
      u64 >>= 1;
    }
    *ucp = uc;
    return 1;
  }
  return 0;
}

int charset_get_next(const charset_t *s, unsigned char *ucp)
{
  unsigned i = (*ucp + 1) / 64;
  unsigned j = (*ucp + 1) % 64;
  uint64_t u64 = s->data[i] >> j;
  for(; i < 4; ++i) {
    if(u64) {
      while(!(u64 & 1)) {
        ++j;
        u64 >>= 1;
      }
      *ucp = i * 64 + j;
      return 1;
    }
    if(i + 1 < 4) {
      j = 0;
      u64 = s->data[i + 1];
    }
  }
  return 0;
}

int charset_get_unique(const charset_t *s, unsigned char *ucp)
{
  unsigned char uc;
  if(!charset_get_first(s, &uc)) return 0;
  if(charset_get_next(s, &uc)) return 0;
  *ucp = uc;
  return 1;
}

void charset_print_char(unsigned char uc)
{
  if(!isprint(uc)) {
    printf("\\%o", uc);
  } else if(strchr("\\-[].*+?|()", uc)) {
    printf("\\%c", uc);
  } else {
    printf("%c", uc);
  }
}

void charset_print(const charset_t *s)
{
  unsigned char uc;

  if(charset_get_unique(s, &uc)) {
    charset_print_char(uc);
  } else {
    printf("[");
    if(charset_get_first(s, &uc)) charset_print_char(uc);
    while(charset_get_next(s, &uc)) charset_print_char(uc);
    printf("]");
  }
}
