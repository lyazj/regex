#include "regex_impl.h"
#include "util.h"

static void regex_build_index_recursion(regex_t *, regex_node_t *);

void regex_build_index(regex_t *regex)
{
  regex->units = (regex_node_t **)Malloc((regex->nunit + 1) * sizeof *regex->units);
  regex->groups = (regex_node_t **)Malloc((regex->ngroup + 1) * sizeof *regex->groups);
  regex->units[0] = NULL;
  regex->groups[0] = regex->node;
  regex_build_index_recursion(regex, regex->node);
}

static void regex_build_index_recursion(regex_t *regex, regex_node_t *node)
{
  if(node == NULL) return;

  if(node->id < 0) {  /* group */
    regex->groups[-node->id] = node;
  } else {
    regex->units[node->id] = node;
  }

  regex_build_index_recursion(regex, node->group);
  regex_build_index_recursion(regex, node->next_node);
  regex_build_index_recursion(regex, node->next_cand);
}

static void regex_compute_node_poses(regex_t *regex, regex_node_t *node);

void regex_compute_poses(regex_t *regex)
{
  size_t id;

  regex_compute_node_poses(regex, regex->node);

  /* Mark accepting states. */
  if(regex->node == NULL) return;
  if(bitset_get_first(&regex->node->lastpos, &id)) do {
    bitset_set(&regex->units[id]->followpos, 0);
  } while(bitset_get_next(&regex->node->lastpos, &id));
}

void regex_compute_node_poses(regex_t *regex, regex_node_t *node)
{
  if(node == NULL) return;

  /* Compute for group/unit. */
  bitset_create(&node->firstpos, regex->nunit + 1);
  bitset_create(&node->lastpos, regex->nunit + 1);
  bitset_create(&node->followpos, regex->nunit + 1);
  bitset_empty(&node->followpos);

  if(node->id < 0) {  /* group */
    if(node->group) {
      regex_compute_node_poses(regex, node->group);
      node->nullable = node->group->nullable;
      bitset_union(&node->group->firstpos, &node->group->firstpos, &node->firstpos);
      bitset_union(&node->group->lastpos, &node->group->lastpos, &node->lastpos);
    } else {  /* empty */
      node->nullable = 1;
      bitset_empty(&node->firstpos);
      bitset_empty(&node->lastpos);
    }
  }

  else {  /* unit */
    node->nullable = 0;
    bitset_empty(&node->firstpos);
    bitset_set(&node->firstpos, node->id);
    bitset_empty(&node->lastpos);
    bitset_set(&node->lastpos, node->id);
  }

  /* Compute for decoration. */
  if(node->type == '*' || node->type == '+') {  /* Update followpos. */
    size_t id;
    if(bitset_get_first(&node->lastpos, &id)) do {
      bitset_union(&regex->units[id]->followpos, &node->firstpos, &regex->units[id]->followpos);
    } while(bitset_get_next(&node->lastpos, &id));
  }
  if(node->type == '*' || node->type == '?') {
    node->nullable = 1;
  }

  /* Consider concatenation. */
  if(node->next_node) {
    regex_compute_node_poses(regex, node->next_node);
    {  /* Update followpos. */
      size_t id;
      if(bitset_get_first(&node->lastpos, &id)) do {
        bitset_union(&regex->units[id]->followpos, &node->next_node->firstpos, &regex->units[id]->followpos);
      } while(bitset_get_next(&node->lastpos, &id));
    }
    if(node->nullable) {
      bitset_union(&node->firstpos, &node->next_node->firstpos, &node->firstpos);
    }
    if(node->next_node->nullable) {
      bitset_union(&node->lastpos, &node->next_node->lastpos, &node->lastpos);
    } else {
      bitset_union(&node->next_node->lastpos, &node->next_node->lastpos, &node->lastpos);
    }
    node->nullable = node->nullable && node->next_node->nullable;
  }

  /* Consider union. */
  if(node->next_cand) {
    regex_compute_node_poses(regex, node->next_cand);
    bitset_union(&node->firstpos, &node->next_cand->firstpos, &node->firstpos);
    bitset_union(&node->lastpos, &node->next_cand->lastpos, &node->lastpos);
    node->nullable = node->nullable || node->next_cand->nullable;
  }
}
