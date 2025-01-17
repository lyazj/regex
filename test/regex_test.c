#undef NDEBUG
#include "regex/regex.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{
  regex_config_t *config = regex_config_create();
  regex_t *regex;

  regex = regex_create(config, "");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, ".");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "[]");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "[.]");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "[^^]");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "\\033");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "a");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "abc");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "a|b|c");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  regex_print_states_mermaid(regex);
  regex_print_mstates_mermaid(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "ab|c");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  regex_print_states_mermaid(regex);
  regex_print_mstates_mermaid(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "a|bc");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  regex_print_states_mermaid(regex);
  regex_print_mstates_mermaid(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "(a|b)|(c|d)");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  regex_print_states_mermaid(regex);
  regex_print_mstates_mermaid(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "()|(a)");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  regex_print_states_mermaid(regex);
  regex_print_mstates_mermaid(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "(a)");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "[a-e3]");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "[asdfghjkl]qwertyuiop[zxcvbnm]");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "[a-zA-Z_][a-zA-Z0-9_]*");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  printf("\n");
  regex_destroy(regex);

  regex = regex_create(config, "[+\\-]?([0-9]+(\\.[0-9]*)?|\\.[0-9]+)([eE][+\\-]?[0-9]+)?");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  printf("\n");
  regex_destroy(regex);

  assert(regex_config_add_escaping_str(config, 'd', "[0-9]"));
  regex = regex_create(config, "[+\\-]?(\\d+(\\.\\d*)?|\\.\\d+)([eE][+\\-]?\\d+)?");
  assert(regex_compile(regex, 0));
  regex_print_compiled_expr(regex);
  printf("\n");
  regex_print_poses(regex);
  regex_print_states_mermaid(regex);
  regex_print_mstates_mermaid(regex);
  printf("\n");
  regex_destroy(regex);

  regex_config_destroy(config);
  return 0;
}
