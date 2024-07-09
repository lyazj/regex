#include "regex_impl.h"
#include "util.h"
#include "charset.h"
#include "bitset.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

regex_t *regex_create(const regex_config_t *config, const char *s)
{
  regex_t *regex = (regex_t *)Malloc(sizeof *regex);
  regex->nunit = 0;
  regex->ngroup = 0;
  regex->ncand = 0;
  regex->nclass = 0;
  regex->nstate = 0;
  regex->nmstate = 0;
  regex->str = Strdup(s);
  regex->cur = regex->str;
  regex->config = config;
  regex->node = NULL;
  regex->units = NULL;
  regex->groups = NULL;
  regex->char_class = NULL;
  regex->class_char = NULL;
  regex->states = NULL;
  regex->state_trans = NULL;
  regex->mstate_trans = NULL;
  return regex;
}

void regex_destroy(regex_t *regex)
{
  if(regex == NULL) return;
  if(regex->mstate_trans) for(int i = 0; i < regex->nmstate; ++i) Free(regex->mstate_trans[i]);
  Free(regex->mstate_trans);
  if(regex->state_trans) for(int i = 0; i < regex->nstate; ++i) Free(regex->state_trans[i]);
  Free(regex->state_trans);
  if(regex->states) for(int i = 0; i < regex->nstate; ++i) bitset_destroy(&regex->states[i]);
  Free(regex->states);
  Free(regex->class_char);
  Free(regex->char_class);
  Free(regex->groups);
  Free(regex->units);
  regex_node_destroy(regex->node);
  Free(regex->str);
  Free(regex);
}

int regex_compile(regex_t *regex, int flags)
{
  if(flags) return 0;  /* [TODO] */

  regex->cur = regex->str;
  if(!parse_regex(regex)) return 0;

  regex_build_index(regex);
  regex_compute_poses(regex);
  regex_make_char_class(regex);
  regex_make_states(regex);
  regex_minimize_states(regex);
  return 1;
}

static void regex_print_node(const regex_node_t *);

void regex_print_compiled_expr(const regex_t *regex)
{
  regex_print_node(regex->node);
}

void regex_print_node(const regex_node_t *node)
{
  if(node == NULL) return;

  if(node->id < 0) {  /* group */
    printf("(");
    regex_print_node(node->group);
    printf(")");
  } else {
    charset_print(&node->charset);
  }

  if(node->type) printf("%c", node->type);
  regex_print_node(node->next_node);
  if(node->next_cand) {
    printf("|");
    regex_print_node(node->next_cand);
  }
}

void regex_print_poses(const regex_t *regex)
{
  if(regex->node) {
    printf("0\tnullable=%d\tfirstpos=", regex->node->nullable);
    bitset_print(&regex->node->firstpos);
    printf("\tlastpos=");
    bitset_print(&regex->node->lastpos);
    printf("\n");
  } else {
    printf("0\tnullable=%d\tfirstpos=", 1);
    printf("[]");
    printf("\tlastpos=");
    printf("[]");
    printf("\n");
  }
  for(int i = 1; i <= regex->nunit; ++i) {
    printf("%d\t", i);
    charset_print(&regex->units[i]->charset);
    printf("\tnullable=%d\tfirstpos=", regex->units[i]->nullable);
    bitset_print(&regex->units[i]->firstpos);
    printf("\tlastpos=");
    bitset_print(&regex->units[i]->lastpos);
    printf("\tfollowpos=");
    bitset_print(&regex->units[i]->followpos);
    printf("\n");
  }
}

static void regex_char_class_print(const regex_t *regex, int i)
{
  charset_t s;
  charset_empty(&s);
  for(unsigned u = 0; u < 256; ++u) {
    if(regex->char_class[u] == i) charset_set(&s, u);
  }
  charset_print(&s);
}

void regex_print_char_class(const regex_t *regex)
{
  for(int i = 0; i < regex->nclass; ++i) {
    printf("%d\t", i);
    regex_char_class_print(regex, i);
    printf("\n");
  }
}

static void regex_print_states_or_mstates(const regex_t *regex, int m)
{
  int nstate = m ? regex->nmstate : regex->nstate;
  int **state_trans = m ? regex->mstate_trans : regex->state_trans;

  for(int j = 0; j <= regex->nclass; ++j) {
    printf("\t%d", j);
  }
  printf("\n");
  for(int i = 0; i < nstate; ++i) {
    printf("%d", i);
    for(int j = 0; j <= regex->nclass; ++j) {
      printf("\t%d", state_trans[i][j]);
    }
    printf("\n");
  }
}

void regex_print_states(const regex_t *regex)  { regex_print_states_or_mstates(regex, 0); }
void regex_print_mstates(const regex_t *regex) { regex_print_states_or_mstates(regex, 1); }

static void regex_print_states_or_mstates_mermaid(const regex_t *regex, int m)
{
  int nstate = m ? regex->nmstate : regex->nstate;
  int **state_trans = m ? regex->mstate_trans : regex->state_trans;
  int dead = -1;

  for(int i = 0; i < nstate; ++i) {
    int isdead = 1;
    for(int j = 0; j < regex->nclass; ++j) {
      if(state_trans[i][j] != i) { isdead = 0; break; }
    }
    if(state_trans[i][regex->nclass] >= 0) isdead = 0;
    if(isdead) { dead = i; break; }
  }


  printf("```mermaid\n");
  printf("graph LR\n");
  for(int i = 0; i < nstate; ++i) {
    if(i == dead) continue;
    for(int j = 0; j < regex->nclass; ++j) {
      if(state_trans[i][j] < 0) continue;
      if(state_trans[i][j] == dead) continue;
      printf("%d -- \"", i);
      regex_char_class_print(regex, j);
      printf("\" --> %d\n", state_trans[i][j]);
    }
    if(state_trans[i][regex->nclass] >= 0) {
      printf("%d(\"%d#%d\")\n", i, i, state_trans[i][regex->nclass]);
    }
  }
  printf("```\n");
}

void regex_print_states_mermaid(const regex_t *regex)  { regex_print_states_or_mstates_mermaid(regex, 0); }
void regex_print_mstates_mermaid(const regex_t *regex) { regex_print_states_or_mstates_mermaid(regex, 1); }

static int parse_regex_term(regex_t *);
static int parse_regex_factor(regex_t *);
static int parse_regex_reusable(regex_t *);
static int parse_regex_group(regex_t *);

static int parse_regex_char(regex_t *regex, char ch)
{
  /* [NODE] requires ch != 0 */
  if(*regex->cur != ch) {
    char msg[] = "expect \"_\"";
               /* 0123456 78 */
    msg[8] = ch;
    parse_regex_error(regex, msg);
    return 0;
  }
  ++regex->cur;
  return 1;
}

int parse_regex(regex_t *regex)
{
  regex_node_t *node = NULL, *first_node;

  if(strchr(")", *regex->cur)) {
    regex->node = NULL;
    return 1;
  }
  if(regex->ncand == 0) regex->ncand = 1;  /* init */

  for(;;) {
    if(!parse_regex_term(regex)) return 0;
    if(node == NULL) {
      node = first_node = regex->node;
    } else {
      node = node->next_cand = regex->node;
    }

    if(strchr(")", *regex->cur)) {
      regex->node = first_node;
      return 1;
    }
    if(!parse_regex_char(regex, '|')) {
      regex_node_destroy(first_node);
      return 0;
    }
    if(regex->ncand > 0 /* unfrozen */) ++regex->ncand;
  }
}

int parse_regex_term(regex_t *regex)
{
  regex_node_t *node, *first_node;

  if(!parse_regex_factor(regex)) return 0;
  node = first_node = regex->node;

  for(;;) {
    if(strchr(")|", *regex->cur)) {
      regex->node = first_node;
      return 1;
    }
    if(!parse_regex_factor(regex)) {
      regex_node_destroy(first_node);
      return 0;
    }
    node = node->next_node = regex->node;
  }
}

int parse_regex_factor(regex_t *regex)
{
  if(!parse_regex_reusable(regex)) return 0;
  while(*regex->cur && strchr("*+?", *regex->cur)) {
    regex_node_decorate(regex->node, *regex->cur);
    ++regex->cur;
  }
  return 1;
}

int parse_regex_reusable(regex_t *regex)
{
  charset_t s;

  if(*regex->cur == '(') {
    return parse_regex_group(regex);
  }
  if(!parse_regex_unit(regex, &s)) return 0;

  regex->node = regex_node_create(++regex->nunit);
  regex->node->cand_id = abs(regex->ncand) - 1;
  regex->node->charset = s;
  return 1;
}

int parse_regex_group(regex_t *regex)
{
  int ncand_frozen;
  regex_node_t *node;

  if(!parse_regex_char(regex, '(')) return 0;
  ncand_frozen = regex->ncand < 0;
  if(!ncand_frozen) regex->ncand = -regex->ncand;  /* freeze candidate number */
  if(!parse_regex(regex)) return 0;
  if(!ncand_frozen) regex->ncand = -regex->ncand;  /* unfreeze candidate number */
  if(!parse_regex_char(regex, ')')) return 0;

  node = regex_node_create(-++regex->ngroup);
  node->cand_id = abs(regex->ncand) - 1;
  node->group = regex->node;
  regex->node = node;
  return 1;
}
