#include "regex_impl.h"
#include "util.h"
#include "hashtab.h"
#include <assert.h>

typedef struct state_class_trans_t {
  int n;
  int *to;       /* length=n, 1/2 primary key */
  int from_old;  /* old class id, 2/2 primary key */
  int from;
} state_class_trans_t;

static size_t state_class_trans_hash(state_class_trans_t *trans)
{
  size_t h = (unsigned)trans->from_old;
  for(int i = 0; i < trans->n; ++i) {
    h <<= 1;
    h ^= (unsigned)trans->to[i];  /* [XXX] */
  }
  return h;
}

static int state_class_trans_equ(state_class_trans_t *trans1, state_class_trans_t *trans2)
{
  assert(trans1->n == trans2->n);
  if(trans1->from_old != trans2->from_old) return 0;
  for(int i = 0; i < trans1->n; ++i) {
    if(trans1->to[i] != trans2->to[i]) return 0;
  }
  return 1;
}

void regex_minimize_states(regex_t *regex)
{
  int nclass, nclass_new;
  int *state_class, *state_class_new;
  hashtab_t *tab;
  state_class_trans_t trans, *addr;
  size_t hs;  /* for iteration on hashtab */

  if(regex->node == NULL) return;  /* Empty, 1 state, already minimized. */
  tab = hashtab_create(sizeof(state_class_trans_t), 97,
      (hashfunc_t *)state_class_trans_hash, (equfunc_t *)state_class_trans_equ);

  /*
   * Initially, classify the states by their transitions on
   * terminator($) input.
   */
  trans.to = (int *)Malloc(1 * sizeof(int));
  state_class = (int *)Malloc(regex->nstate * sizeof(int));
  nclass = 0;
  for(int s = 0; s < regex->nstate; ++s) {
    trans.n = 1;
    trans.to[0] = regex->state_trans[s][regex->nclass];
    trans.from_old = 0;
    if(hashtab_insert(tab, &trans, (void**)&addr)) {  /* new class */
      addr->from = nclass++;
      trans.to = (int *)Malloc(1 * sizeof(int));
    }
    state_class[s] = addr->from;
  }
  if(hashtab_get_first(tab, (void**)&addr, &hs)) do {
    Free(addr->to);
  } while(hashtab_get_next(tab, (void**)&addr, &hs));

  /*
   * Then iteratively, sub-divide a class if its members
   * transfer to different classes on the same input.
   */
  Free(trans.to);
  trans.to = (int *)Malloc(regex->nclass * sizeof(int));
  state_class_new = (int *)Malloc(regex->nstate * sizeof(int));
  for(;;) {
    hashtab_clear(tab);
    nclass_new = 0;
    for(int s = 0; s < regex->nstate; ++s) {
      trans.n = regex->nclass;
      for(int c = 0; c < regex->nclass; ++c) {
        int to = regex->state_trans[s][c];
        trans.to[c] = to < 0 /* eliminated dead state */ ? to : state_class[to];
      }
      trans.from_old = state_class[s];
      if(hashtab_insert(tab, &trans, (void**)&addr)) {  /* new class */
        addr->from = nclass_new++;
        trans.to = (int *)Malloc(regex->nclass * sizeof(int));
      }
      state_class_new[s] = addr->from;
    }
    if(hashtab_get_first(tab, (void**)&addr, &hs)) do {
      Free(addr->to);
    } while(hashtab_get_next(tab, (void**)&addr, &hs));

    if(nclass_new == nclass) break;
    nclass = nclass_new;
    {
      int *tmp = state_class;
      state_class = state_class_new;
      state_class_new = tmp;
    }
  }

  /* Release resources not needed anymore. */
  Free(trans.to);
  hashtab_destroy(tab);

  /* Store results. */
  {
    int *class_exist = state_class_new;
    memset(class_exist, 0, nclass * sizeof(int));

    regex->nmstate = nclass;
    regex->mstate_trans = (int **)Malloc(nclass * sizeof(int *));
    for(int s = 0; s < regex->nstate; ++s) {
      int c = state_class[s];
      if(class_exist[c]) continue;
      class_exist[c] = 1;
      regex->mstate_trans[c] = (int *)Malloc((regex->nclass + 1) * sizeof(int));
      for(int ch = 0; ch < regex->nclass; ++ch) {
        int to = regex->state_trans[s][ch];
        regex->mstate_trans[c][ch] = to < 0 /* eliminated dead state */ ? to : state_class[to];
      }
      regex->mstate_trans[c][regex->nclass] = regex->state_trans[s][regex->nclass];
    }
  }
  Free(state_class);
  Free(state_class_new);
}
