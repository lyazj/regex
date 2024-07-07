#include "hashtab.h"
#include "util.h"
#include <string.h>

typedef struct hashtab_node_t {
  struct hashtab_node_t *next;
  char data[0];
} hashtab_node_t;

struct hashtab_t {
  size_t elem_size;
  size_t nbucket;
  hashfunc_t *hashfunc;
  equfunc_t *equfunc;
  hashtab_node_t **bucket;
};

hashtab_t *hashtab_create(size_t s, size_t n, hashfunc_t *h, equfunc_t *e)
{
  hashtab_t *tab = (hashtab_t *)Malloc(sizeof *tab);
  tab->elem_size = s;
  tab->nbucket = n;
  tab->hashfunc = h;
  tab->equfunc = e;
  tab->bucket = (hashtab_node_t **)Calloc(n, sizeof *tab->bucket);
  return tab;
}

void hashtab_destroy(hashtab_t *tab)
{
  for(size_t b = 0; b < tab->nbucket; ++b) {
    for(hashtab_node_t *node = tab->bucket[b], *next; node; node = next) {
      next = node->next; Free(node);
    }
  }
  Free(tab->bucket);
  Free(tab);
}

int hashtab_insert(hashtab_t *tab, const void *data, void **addr)
{
  size_t hash = tab->hashfunc(data) % tab->nbucket;
  hashtab_node_t **p;
  for(p = &tab->bucket[hash]; *p; p = &(*p)->next) {
    if(tab->equfunc((*p)->data, data)) {  /* already exists */
      *addr = (*p)->data;
      return 0;
    }
  }
  /* insert */
  *p = (hashtab_node_t *)Malloc(sizeof(hashtab_node_t) + tab->elem_size);
  (*p)->next = NULL;
  memcpy((*p)->data, data, tab->elem_size);
  *addr = (*p)->data;
  return 1;
}
