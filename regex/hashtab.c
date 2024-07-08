#include "hashtab.h"
#include "util.h"
#include <string.h>

typedef struct hashtab_node_t {
  struct hashtab_node_t *next;
} hashtab_node_t;

#define NODE_DATA(node)  ((char *)(node) + sizeof(hashtab_node_t))
#define DATA_NODE(data)  ((hashtab_node_t *)(data) - 1)

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
  hashtab_clear(tab);
  Free(tab->bucket);
  Free(tab);
}

void hashtab_clear(hashtab_t *tab)
{
  for(size_t b = 0; b < tab->nbucket; ++b) {
    for(hashtab_node_t *node = tab->bucket[b], *next; node; node = next) {
      next = node->next; Free(node);
    }
    tab->bucket[b] = NULL;
  }
}

int hashtab_insert(hashtab_t *tab, const void *data, void **addr)
{
  size_t hash = tab->hashfunc(data) % tab->nbucket;
  hashtab_node_t **p;
  for(p = &tab->bucket[hash]; *p; p = &(*p)->next) {
    if(tab->equfunc(NODE_DATA(*p), data)) {  /* already exists */
      *addr = NODE_DATA(*p);
      return 0;
    }
  }
  /* insert */
  *p = (hashtab_node_t *)Malloc(sizeof(hashtab_node_t) + tab->elem_size);
  (*p)->next = NULL;
  memcpy(NODE_DATA(*p), data, tab->elem_size);
  *addr = NODE_DATA(*p);
  return 1;
}

void *hashtab_find(const hashtab_t *tab, const void *data)
{
  size_t hash = tab->hashfunc(data) % tab->nbucket;
  for(hashtab_node_t *node = tab->bucket[hash]; node; node = node->next) {
    if(tab->equfunc(NODE_DATA(node), data)) {
      return NODE_DATA(node);
    }
  }
  return NULL;
}

int hashtab_get_first(const hashtab_t *tab, void **addr, size_t *bp)
{
  *bp = (size_t)-1;
  return hashtab_get_next(tab, addr, bp);
}

int hashtab_get_next(const hashtab_t *tab, void **addr, size_t *bp)
{
  size_t b = *bp;
  hashtab_node_t *node;

  if(b == (size_t)-1) {  /* init */
    b = 0;
    node = tab->bucket[b];
  } else {  /* next */
    node = DATA_NODE(*addr)->next;
  }

  while(node == NULL && ++b < tab->nbucket) {
    node = tab->bucket[b];
  }
  if(node == NULL) return 0;
  *addr = NODE_DATA(node);
  *bp = b;
  return 1;
}
