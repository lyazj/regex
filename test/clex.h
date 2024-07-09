#pragma once
#include <stddef.h>

#define INT_POSTFIX  "([uU]([lL][lL]?)?|([lL][lL]?)[uU]?)?"  /* We don't accept LUL. */
#define FLT_POSTFIX  "[fFlL]?"

static const char *patterns[] = {  /* Covers only most-used C lexical features. */
  /* white spaces */                "[ \\f\\n\\r\\t\\v]",
  /* keywords */
  "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register", "restrict", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while",
  /* directive */                   "#[^\\n]*",
  /* comment */                     "//[^\\n]*",
  /* comment */                     "/[*]" "([^*]|[*]+[^*/])*" "[*]+/",
  /* identifier */                  "[a-zA-Z_][a-zA-Z0-9_]*",
  /* decimal integer */             "(0|[1-9][0-9]*)" INT_POSTFIX,
  /* octet integer */               "0[0-7]+" INT_POSTFIX,
  /* hexadecimal integer */         "0[xX][0-9a-fA-F]+" INT_POSTFIX,
  /* binary integer */              "0[bB][01]+" INT_POSTFIX,
  /* decimal floating-point */      "(" "[0-9]+[eE][+\\-]?[0-9]+" "|" "(\\.[0-9]+|[0-9]+\\.[0-9]*)([eE][+\\-]?[0-9]+)?" ")" FLT_POSTFIX,
  /* hexadecimal floating-point */  "0[xX](\\.[0-9a-fA-F]+|[0-9a-fA-F]+(\\.[0-9a-fA-F]*)?)[pP][+\\-]?[0-9]+" FLT_POSTFIX,
  /* character integer */           "L?'(" "[^'\\\\\\n]" "|" "\\\\[^\\n]" ")*'",
  /* character array (string) */    "L?\"(" "[^\"\\\\\\n]" "|" "\\\\[^\\n]" ")*\"",
  /* punctuations */
  "!", "%", "&", "\\(", "\\)", "\\*", "\\+", ",", "-", "\\.", "/", ":", ";", "<", "=", ">", "\\?", "\\[", "\\]", "^", "{", "\\|", "}", "~",
  "\\+\\+", "--", "<<", ">>", "==", "&&", "\\|\\|",
  "<=", ">=", "!=", "\\+=", "-=", "\\*=", "/=", "%=", "&=", "^=", "\\|=", "<<=", ">>=",
  "->", "\\.\\.\\.",
  NULL,
};
