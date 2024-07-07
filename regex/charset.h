#pragma once
#include <string.h>
#include <stdint.h>

typedef struct charset_t {
  uint64_t data[4];  /* 256-long bitset */
} charset_t;

static inline void charset_set(charset_t *s, unsigned char uc)
{
  s->data[uc / 64] |= ((uint64_t)1) << (uc % 64);
}

static inline void charset_unset(charset_t *s, unsigned char uc)
{
  s->data[uc / 64] &= ~(((uint64_t)1) << (uc % 64));
}

static inline int charset_test(charset_t *s, unsigned char uc)
{
  return (s->data[uc / 64] >> (uc % 64)) & 1;
}

static inline void charset_empty(charset_t *s)
{
  memset(s->data, 0, sizeof s->data);
}

static inline void charset_fill(charset_t *s)
{
  memset(s->data, -1, sizeof s->data);
  charset_unset(s, 0);  /* '\0' is excluded by default */
}

static inline void charset_union(const charset_t *s1, const charset_t *s2, charset_t *s)
{
  for(unsigned i = 0; i < 4; ++i) {
    s->data[i] = s1->data[i] | s2->data[i];
  }
}

static inline void charset_intersect(const charset_t *s1, const charset_t *s2, charset_t *s)
{
  for(unsigned i = 0; i < 4; ++i) {
    s->data[i] = s1->data[i] & s2->data[i];
  }
}

static inline void charset_diff(const charset_t *s1, const charset_t *s2, charset_t *s)
{
  for(unsigned i = 0; i < 4; ++i) {
    s->data[i] = s1->data[i] & ~s2->data[i];
  }
}

unsigned charset_count(const charset_t *s);
int charset_get_first(const charset_t *s, unsigned char *ucp);
int charset_get_next(const charset_t *s, unsigned char *ucp);
int charset_get_unique(const charset_t *s, unsigned char *ucp);
void charset_print_char(unsigned char uc);
void charset_print(const charset_t *s);
