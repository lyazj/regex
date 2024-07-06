#include "regex_impl.h"
#include "util.h"
#include <assert.h>
#include <string.h>

regex_node_t *regex_node_create(int id)
{
  regex_node_t *node = (regex_node_t *)Malloc(sizeof *node);
  node->id = id;
  node->type = 0;
  /* node->charset uninitialized */
  node->next_cand = NULL;
  node->next_node = NULL;
  node->group = NULL;
  return node;
}

void regex_node_destroy(regex_node_t *node)
{
  if(node == NULL) return;
  regex_node_destroy(node->next_cand);
  regex_node_destroy(node->next_node);
  regex_node_destroy(node->group);
  Free(node);
}

void regex_node_decorate(regex_node_t *node, int type)
{
  switch(type) {
  case 0:
    return;
  case '*':
    assert(strchr("*+?", node->type)); node->type = '*';
    return;
  case '+':
    if(strchr("+", node->type)) { node->type = '+'; return; }
    assert(strchr("*?", node->type)); node->type = '*';
    return;
  case '?':
    if(strchr("?", node->type)) { node->type = '?'; return; }
    assert(strchr("*+", node->type)); node->type = '*';
    return;
  case '#':
    assert(strchr("#", node->type)); node->type = '#';
    return;
  default:
    assert(!"unsupported type");
  }
}
