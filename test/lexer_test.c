#include "clex.h"
#include "regex/regex.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
  char buf[8192];
  regex_config_t *config = NULL;
  regex_t *regex = NULL;
  regex_lexer_t *lexer = NULL;
  FILE *file;
  regex_lexeme_t lexeme;

  config = regex_config_create();

  buf[0] = '\0';
  for(const char **p = patterns; *p; ++p) {
    if(p != patterns) strcat(buf, "|");
    strcat(buf, *p);
  }
  regex = regex_create(config, buf);
  if(regex_compile(regex, 0) == 0) {
    fprintf(stderr, "ERROR: error compiling regex\n");
    return 1;
  }

  file = fopen(__FILE__, "r");
  if(file == NULL) {
    fprintf(stderr, "ERROR: error opening input file\n");
    return 1;
  }
  lexer = regex_lexer_create(regex, file);

  while(regex_lexer_next(lexer, &lexeme)) {
    if(lexeme.type == 0 /* white spaces */) continue;
    printf("%d\t%zd:%zd\t%zd:%zd\t%s\n", lexeme.type, lexeme.f_line, lexeme.f_column,
        lexeme.l_line, lexeme.l_column, lexeme.text);
  }

  regex_lexer_destroy(lexer);
  fclose(file);
  regex_destroy(regex);
  regex_config_destroy(config);
  return 0;
}
