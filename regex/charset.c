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
    printf("\\%03o", uc);
  } else if(strchr("\\-[].*+?|()", uc)) {
    printf("\\%c", uc);
  } else {
    printf("%c", uc);
  }
}

static void charset_print_range(unsigned char first, unsigned char last)
{
  if(first == last) {
    charset_print_char(first);
  } else {
    charset_print_char(first);
    if(first + 1 != last) printf("-");
    charset_print_char(last);
  }
}

void charset_print(const charset_t *s)
{
  unsigned char uc;

  if(charset_get_unique(s, &uc)) {
    charset_print_char(uc);
  } else {
    unsigned first, last = -1;
    printf("[");
    if(charset_get_first(s, &uc)) do {
      if(last == (unsigned)-1) {  /* init */
        first = last = uc;
      } else if(uc == last + 1) {  /* coalesce */
        last = uc;
      } else {  /* non-contiguous */
        charset_print_range(first, last);
        first = last = uc;
      }
    } while(charset_get_next(s, &uc));
    if(last != (unsigned)-1) {
      charset_print_range(first, last);
    }
    printf("]");
  }
}
