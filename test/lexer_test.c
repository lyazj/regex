#include "regex/regex.h"
#include <stdio.h>
#include <string.h>

const char *patterns[] = {  /* Covers only most-used C lexical features. */
  /* white spaces */                "[ \\f\\n\\r\\t\\v]",
  /* keywords */
  "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register", "restrict", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while",
  /* directive */                   "#[^\\n]*",
  /* comment */                     "//[^\\n]*",
  /* comment */                     "/[*]" "([^*]|[*]+[^*/])*" "[*]+/",
  /* identifier */                  "[a-zA-Z_][a-zA-Z0-9_]*",
  /* decimal integer */             "(0|[1-9][0-9]*)[uUlL]*",
  /* octet integer */               "0[0-7]+[uUlL]*",
  /* hexadecimal integer */         "0[xX][0-9a-fA-F]+[uUlL]*",
  /* binary integer */              "0[bB][01]+[uUlL]*",
  /* decimal floating-point */      "(" "[0-9]+[eE][+\\-]?[0-9]+" "|" "(\\.[0-9]+|[0-9]+\\.[0-9]*)([eE][+\\-]?[0-9]+)?" ")[fFlL]*",
  /* hexadecimal floating-point */  "0[xX](\\.[0-9a-fA-F]+|[0-9a-fA-F]+(\\.[0-9a-fA-F]*)?)[pP][+\\-]?[0-9]+[fFlL]*",
  /* character integer */           "L?'(" "[^'\\\\\\n]" "|" "\\\\[^\\n]" ")*'",
  /* character array (string) */    "L?\"(" "[^\"\\\\\\n]" "|" "\\\\[^\\n]" ")*\"",
  /* punctuations */
  "!", "%", "&", "\\(", "\\)", "\\*", "\\+", ",", "-", "\\.", "/", ":", ";", "<", "=", ">", "\\?", "\\[", "\\]", "^", "{", "\\|", "}", "~",
  "\\+\\+", "--", "<<", ">>", "==", "&&", "\\|\\|",
  "<=", ">=", "!=", "\\+=", "-=", "\\*=", "/=", "%=", "&=", "^=", "\\|=", "<<=", ">>=",
  "->", "\\.\\.\\.",
  NULL,
};

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
