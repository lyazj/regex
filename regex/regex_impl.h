#pragma once
#include "regex.h"
#include "charset.h"
#include "bitset.h"
#include <stdio.h>

struct regex_config_t {
  charset_t escape[256];
};

typedef struct regex_node_t {
  int id;
  int cand_id;
  char type;  /* "*" | "+" | "?" */
  char nullable;
  char this_cand_nullable;
  charset_t charset;
  struct regex_node_t *next_cand;  /* regex "|" term */
  struct regex_node_t *next_node;  /* term factor */
  struct regex_node_t *group;      /* "(" regex ")" */
  bitset_t firstpos;
  bitset_t lastpos;
  bitset_t followpos;
} regex_node_t;

struct regex_t {
  int nunit;
  int ngroup;
  int ncand;
  int nclass;
  int nstate;
  int nmstate;
  char *str;  /* owned */
  char *cur;
  const regex_config_t *config;
  regex_node_t *node;  /* owned */
  regex_node_t **units;
  regex_node_t **groups;
  unsigned char *char_class;
  unsigned char *class_char;
  bitset_t *states;
  int **state_trans;  /* [state][input] */
  int **mstate_trans;
};

struct regex_lexer_t {
  const regex_t *regex;
  FILE *file;
  size_t line;
  size_t column;
  char *f_buf;       /* forward buffer */
  size_t f_buf_len;
  char *f_cur;
  char *b_buf;       /* backward buffer */
  size_t b_buf_len;
  char *b_cur;
  regex_lexeme_t lexeme;
};

regex_node_t *regex_node_create(int id);
void regex_node_destroy(regex_node_t *);
void regex_node_decorate(regex_node_t *, int type);

void parse_regex_error(regex_t *, const char *msg);
int parse_regex(regex_t *);
int parse_regex_unit(regex_t *, charset_t *);
int parse_regex_escaped_unit(regex_t *, charset_t *);
int parse_regex_class_escaped_unit(regex_t *, charset_t *);

void regex_build_index(regex_t *);
void regex_compute_poses(regex_t *);
void regex_make_char_class(regex_t *);
void regex_make_states(regex_t *);
void regex_minimize_states(regex_t *);
