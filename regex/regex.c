#include "regex_impl.h"
#include "util.h"
#include "charset.h"
#include "bitset.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

regex_t *regex_create(const regex_config_t *config, const char *s)
{
  regex_t *regex = (regex_t *)Malloc(sizeof *regex);
  regex->nunit = 0;
  regex->ngroup = 0;
  regex->str = Strdup(s);
  regex->cur = regex->str;
  regex->config = config;
  regex->node = NULL;
  regex->units = NULL;
  regex->groups = NULL;
  return regex;
}

void regex_destroy(regex_t *regex)
{
  if(regex == NULL) return;
  Free(regex->groups);
  Free(regex->units);
  regex_node_destroy(regex->node);
  Free(regex->str);
  Free(regex);
}

int regex_compile(regex_t *regex, int flags)
{
  assert(flags == 0);  /* [TODO] */

  regex->cur = regex->str;
  if(!parse_regex(regex)) return 0;

  regex_build_index(regex);
  regex_compute_poses(regex);
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
    printf("\tfollowpos=");
    bitset_print(&regex->node->followpos);
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
  regex->node->charset = s;
  return 1;
}

int parse_regex_group(regex_t *regex)
{
  regex_node_t *node;

  if(!parse_regex_char(regex, '(')) return 0;
  if(!parse_regex(regex)) return 0;
  if(!parse_regex_char(regex, ')')) return 0;

  node = regex_node_create(-++regex->ngroup);
  node->group = regex->node;
  regex->node = node;
  return 1;
}
