#pragma once
#include "regex.h"
#include <stdint.h>

typedef struct charset_t {
  uint64_t data[4];  /* 256-long bitset */
} charset_t;

typedef struct regex_config_t {
  charset_t escape[256];
} regex_config_t;

typedef struct regex_node_t {
  int id;
  int type;  /* "*" | "+" | "?" | "#" */
  charset_t charset;
  struct regex_node_t *next_cand;  /* regex "|" term */
  struct regex_node_t *next_node;  /* term factor */
  struct regex_node_t *group;      /* "(" regex ")" */
} regex_node_t;

typedef struct regex_t {
  char *str;  /* owned */
  char *cur;
  const regex_config_t *config;
  regex_node_t *node;  /* owned */
} regex_t;

regex_node_t *regex_node_create(int id);
void regex_node_destroy(regex_node_t *);
void regex_node_decorate(regex_node_t *, int type);

void parse_regex_error(regex_t *, const char *msg);
int parse_regex(regex_t *);
int parse_regex_unit(regex_t *, charset_t *);
int parse_regex_escaped_unit(regex_t *, charset_t *);
int parse_regex_class_escaped_unit(regex_t *, charset_t *);
