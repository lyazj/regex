#pragma once
#include <stddef.h>

typedef struct hashtab_t hashtab_t;
typedef size_t hashfunc_t(const void *);
typedef int equfunc_t(const void *, const void *);

hashtab_t *hashtab_create(size_t elem_size, size_t nbucket, hashfunc_t *hashfunc, equfunc_t *equfunc);
void hashtab_destroy(hashtab_t *);

/*
 * The functionality is minimized for our use.
 * On success, 1 is returned, and elem_size bytes from data
 * are copied to which *<returned-addr> points to.
 */
int hashtab_insert(hashtab_t *, const void *data, void **addr);
void *hashtab_find(const hashtab_t *, const void *data);
int hashtab_get_first(const hashtab_t *, void **addr, size_t *state);
int hashtab_get_next(const hashtab_t *, void **addr, size_t *state);
