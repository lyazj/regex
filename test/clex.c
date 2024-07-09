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

  config = regex_config_create();

  for(const char **p = patterns; *p; ++p) {
    regex = regex_create(config, *p);
    if(regex_compile(regex, 0) == 0) {
      fprintf(stderr, "ERROR: error compiling regex\n");
      return 1;
    }
    regex_print_mstates_mermaid(regex);
    regex_destroy(regex);
  }

  /* for all keywords */
  {
    buf[0] = '\0';
    for(const char **p = patterns + 1; *p; ++p) {
      if(p != patterns + 1) strcat(buf, "|");
      strcat(buf, *p);
      if(strcmp(*p, "while") == 0) break;
    }
    regex = regex_create(config, buf);
    if(regex_compile(regex, 0) == 0) {
      fprintf(stderr, "ERROR: error compiling regex\n");
      return 1;
    }
    regex_print_mstates_mermaid(regex);
    regex_destroy(regex);
  }

  /* for all punctuations */
  {
    const char **q;
    for(q = patterns; *q; ++q) {
      if(strcmp(*q, "!") == 0) break;
    }

    buf[0] = '\0';
    for(const char **p = q; *p; ++p) {
      if(p != q) strcat(buf, "|");
      strcat(buf, *p);
      if(strcmp(*p, "while") == 0) break;
    }
    regex = regex_create(config, buf);
    if(regex_compile(regex, 0) == 0) {
      fprintf(stderr, "ERROR: error compiling regex\n");
      return 1;
    }
    regex_print_mstates_mermaid(regex);
    regex_destroy(regex);
  }

  /* for all integers */
  {
    const char **q;
    for(q = patterns; *q; ++q) {
      if(strcmp(*q, "(0|[1-9][0-9]*)[uUlL]*") == 0) break;
    }

    buf[0] = '\0';
    for(int i = 0; i < 4; ++i) {
      if(i) strcat(buf, "|");
      strcat(buf, q[i]);
    }
    regex = regex_create(config, buf);
    if(regex_compile(regex, 0) == 0) {
      fprintf(stderr, "ERROR: error compiling regex\n");
      return 1;
    }
    regex_print_mstates_mermaid(regex);
    regex_destroy(regex);
  }

  regex_config_destroy(config);
  return 0;
}
