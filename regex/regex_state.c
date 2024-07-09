#include "regex_impl.h"
#include "bitset.h"
#include "hashtab.h"
#include "util.h"

typedef struct regex_state_t {
  bitset_t state;
  int cand_id;  /* trans[regex->nclass] */
  int id;
  int *trans;
} regex_state_t;

static size_t regex_state_hash(const regex_state_t *);
static int regex_state_equ(const regex_state_t *, const regex_state_t *);
static void regex_make_states_recursion(regex_t *regex, regex_state_t *state, hashtab_t *tab);

void regex_make_states(regex_t *regex)
{
  regex_state_t state, *addr;
  hashtab_t *tab = hashtab_create(sizeof(regex_state_t), 97, (hashfunc_t *)regex_state_hash, (equfunc_t *)regex_state_equ);
  size_t s;

  /* Build initial state. */
  bitset_create(&state.state, regex->nunit + 1);
  if(regex->node) {
    bitset_union(&regex->node->firstpos, &regex->node->firstpos, &state.state);
    if(regex->node->nullable) bitset_set(&state.state, 0);
  } else {  /* empty */
    bitset_empty(&state.state);
    bitset_set(&state.state, 0);
  }

  /* Avoid involving dead state. */
  if(bitset_get_first(&state.state, &s)) {  /* non-empty */
    state.id = regex->nstate++;
    state.trans = (int *)Malloc((regex->nclass + 1) * sizeof *state.trans);
    state.trans[regex->nclass] = -1;
    if(bitset_test(&state.state, 0)) {  /* accepting */
      if(regex->node) for(regex_node_t *node = regex->node; node; node = node->next_cand) {
        if(node->this_cand_nullable) {
          state.trans[regex->nclass] = node->cand_id;
          break;
        }
      } else {  /* empty */
        state.trans[regex->nclass] = 0;
      }
    }
    state.cand_id = state.trans[regex->nclass];
    hashtab_insert(tab, &state, (void **)&addr);

    /* Derive subsequent states. */
    regex_make_states_recursion(regex, addr, tab);
  }

  /* Store results. */
  regex->states = (bitset_t *)Malloc(regex->nstate * sizeof(*regex->states));
  regex->state_trans = (int **)Malloc(regex->nstate * sizeof(*regex->state_trans));
  if(hashtab_get_first(tab, (void **)&addr, &s)) do {
    int id = addr->id;
    regex->states[id] = addr->state;
    regex->state_trans[id] = addr->trans;
  } while(hashtab_get_next(tab, (void **)&addr, &s));

  hashtab_destroy(tab);
}

size_t regex_state_hash(const regex_state_t *s)
{
  return (bitset_hash(&s->state) << 1) ^ (unsigned)s->cand_id;
}

int regex_state_equ(const regex_state_t *s1, const regex_state_t *s2)
{
  return s1->cand_id == s2->cand_id && bitset_equ(&s1->state, &s2->state);
}

void regex_make_states_recursion(regex_t *regex, regex_state_t *state, hashtab_t *tab)
{
  regex_state_t dest, *addr;
  bitset_create(&dest.state, regex->nunit + 1);

  for(int cls = 0; cls < regex->nclass; ++cls) {
    unsigned char uc = regex->class_char[cls];
    size_t from_node_id;
    int cand_id = -1;

    /* Compute the next state. */
    bitset_empty(&dest.state);
    if(bitset_get_first(&state->state, &from_node_id)) do {
      if(from_node_id == 0) continue;
      if(charset_test(&regex->units[from_node_id]->charset, uc)) {
        bitset_union(&dest.state, &regex->units[from_node_id]->followpos, &dest.state);
        if(bitset_test(&regex->units[from_node_id]->followpos, 0)) {  /* accepting */
          int id = regex->units[from_node_id]->cand_id;
          if(cand_id == -1 || id < cand_id) cand_id = id;  /* prefer smaller */
        }
      }
    } while(bitset_get_next(&state->state, &from_node_id));

    /* Avoid involving dead state. */
    if(!bitset_get_first(&dest.state, &from_node_id)) {  /* empty */
      state->trans[cls] = -1;
      continue;
    }

    /* Fetch/create the state, and recursively, the subsequent states, if any. */
    dest.cand_id = cand_id;
    if(hashtab_insert(tab, &dest, (void **)&addr)) {  /* new state */
      addr->id = regex->nstate++;
      addr->trans = (int *)Malloc((regex->nclass + 1) * sizeof *addr->trans);
      addr->trans[regex->nclass] = cand_id;
      regex_make_states_recursion(regex, addr, tab);  /* depth-first search */
      bitset_create(&dest.state, regex->nunit + 1);
    }

    /* Record the state transition. */
    state->trans[cls] = addr->id;
  }

  bitset_destroy(&dest.state);
}
