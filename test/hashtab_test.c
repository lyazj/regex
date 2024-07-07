#undef NDEBUG
#include "regex/hashtab.h"
#include <assert.h>
#include <stdio.h>

typedef struct node_t {
  int key;
  int value;
} node_t;

static size_t hashfunc(const node_t *node)
{
  return (unsigned int)node->key;
}

static int equfunc(const node_t *n1, const node_t *n2)
{
  return n1->key == n2->key;
}

int main(void)
{
  hashtab_t *tab = hashtab_create(sizeof(node_t), 97, (hashfunc_t *)hashfunc, (equfunc_t *)equfunc);
  node_t node;
  node_t *addr;
  size_t state;

  node.key = 1, node.value = 2;
  assert(hashtab_insert(tab, &node, (void **)&addr));
  node.key = 0, node.value = 1;
  assert(hashtab_insert(tab, &node, (void **)&addr));
  node.key = -1, node.value = 0;
  assert(hashtab_insert(tab, &node, (void **)&addr));

  node.value = -1;
  for(int i = -1; i <= 1; ++i) {
    node.key = i;
    assert(hashtab_insert(tab, &node, (void **)&addr) == 0);
    assert(addr->value == i + 1);
  }
  for(int i = -1; i <= 1; ++i) {
    node.key = i;
    assert((addr = (node_t *)hashtab_find(tab, &node)) && addr->value == i + 1);
  }

  if(hashtab_get_first(tab, (void **)&addr, &state)) do {
    printf("%d\t%d\n", addr->key, addr->value);
  } while(hashtab_get_next(tab, (void **)&addr, &state));

  hashtab_destroy(tab);
  return 0;
}

#ifdef ENABLE_VISIBILITY
#include "regex/hashtab.c"
#include "regex/util.c"
#endif  /* ENABLE_VISIBILITY */
