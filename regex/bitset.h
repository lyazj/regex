#pragma once
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

typedef struct bitset_t {
  size_t n;
  uint64_t *data;
} bitset_t;

void bitset_create(bitset_t *, size_t);
void bitset_destroy(bitset_t *);

static inline size_t bitset_bufsize(size_t n)
{
  return (n + 63) / 64 * 8;
}

static inline void bitset_set(bitset_t *s, size_t i)
{
  assert(i < s->n);
  s->data[i / 64] |= ((uint64_t)1) << (i % 64);
}

static inline void bitset_unset(bitset_t *s, size_t i)
{
  assert(i < s->n);
  s->data[i / 64] &= ~(((uint64_t)1) << (i % 64));
}

static inline int bitset_test(bitset_t *s, size_t i)
{
  assert(i < s->n);
  return (s->data[i / 64] >> (i % 64)) & 1;
}

static inline void bitset_empty(bitset_t *s)
{
  memset(s->data, 0, bitset_bufsize(s->n));
}

static inline void bitset_fill(bitset_t *s)
{
  memset(s->data, -1, bitset_bufsize(s->n));
}

static inline void bitset_union(const bitset_t *s1, const bitset_t *s2, bitset_t *s)
{
  assert(s1->n == s->n);
  assert(s2->n == s->n);
  for(size_t i = 0; i < bitset_bufsize(s->n) / 8; ++i) {
    s->data[i] = s1->data[i] | s2->data[i];
  }
}

static inline void bitset_intersect(const bitset_t *s1, const bitset_t *s2, bitset_t *s)
{
  assert(s1->n == s->n);
  assert(s2->n == s->n);
  for(size_t i = 0; i < bitset_bufsize(s->n) / 8; ++i) {
    s->data[i] = s1->data[i] & s2->data[i];
  }
}

static inline void bitset_diff(const bitset_t *s1, const bitset_t *s2, bitset_t *s)
{
  assert(s1->n == s->n);
  assert(s2->n == s->n);
  for(size_t i = 0; i < bitset_bufsize(s->n) / 8; ++i) {
    s->data[i] = s1->data[i] & ~s2->data[i];
  }
}

size_t bitset_hash(const bitset_t *s);
int bitset_equ(const bitset_t *s1, const bitset_t *s2);
int bitset_cmp(const bitset_t *s1, const bitset_t *s2);
int bitset_get_first(const bitset_t *s, size_t *up);
int bitset_get_next(const bitset_t *s, size_t *up);
size_t bitset_count(const bitset_t *s);
void bitset_print(const bitset_t *s);
