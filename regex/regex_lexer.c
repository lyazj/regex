#include "regex_impl.h"
#include "util.h"

static void lexer_lexeme_reset(regex_lexeme_t *lexeme)
{
  lexeme->type = -1;
  lexeme->text = NULL;
  lexeme->len = 0;
  lexeme->f_line = 0;
  lexeme->f_column = 0;
  lexeme->l_line = 0;
  lexeme->l_column = 0;
}

regex_lexer_t *regex_lexer_create(const regex_t *regex, void /* FILE */ *file)
{
  regex_lexer_t *lexer = (regex_lexer_t *)Malloc(sizeof *lexer);
  lexer->regex = regex;
  lexer->file = (FILE *)file;
  lexer->line = 1;
  lexer->column = 0;
  lexer->f_buf = (char *)Malloc(BUFSIZ);
  lexer->f_buf_len = BUFSIZ;
  lexer->f_cur = lexer->f_buf;
  lexer->b_buf = (char *)Malloc(BUFSIZ);
  lexer->b_buf_len = BUFSIZ;
  lexer->b_cur = lexer->b_buf;
  /* lexer->lexeme uninitialized */
  return lexer;
}

void regex_lexer_destroy(regex_lexer_t *lexer)
{
  if(lexer == NULL) return;
  Free(lexer->b_buf);
  Free(lexer->f_buf);
  Free(lexer);
}

static int regex_lexer_getc(regex_lexer_t *lexer);
static int regex_lexer_ungetc(regex_lexer_t *lexer);
static void regex_lexer_record_accepting_state(regex_lexer_t *lexer, int state);
static void regex_lexer_retract(regex_lexer_t *lexer);

int regex_lexer_next(regex_lexer_t *lexer, regex_lexeme_t *lexeme)
{
  const regex_t *regex = lexer->regex;
  int state;

  lexer->f_cur = lexer->f_buf;
  lexer_lexeme_reset(&lexer->lexeme);
  state = 0;
  while(state >= 0) {
    int c;
    if(regex->mstate_trans[state][regex->nclass] >= 0) {
      regex_lexer_record_accepting_state(lexer, state);
    }
    c = regex_lexer_getc(lexer);
    if(c == EOF) {
      state = -1;
    } else {
      if(lexer->lexeme.f_line == 0) {
        lexer->lexeme.f_line = lexer->line;
        lexer->lexeme.f_column = lexer->column;
      }
      c = regex->char_class[(unsigned char)c];
      state = regex->mstate_trans[state][c];
    }
  }

  regex_lexer_retract(lexer);
  if(lexer->lexeme.type >= 0) {
    *lexeme = lexer->lexeme;
    return 1;
  }
  return 0;
}

void regex_lexer_record_accepting_state(regex_lexer_t *lexer, int state)
{
  const regex_t *regex = lexer->regex;
  lexer->lexeme.type = regex->mstate_trans[state][regex->nclass];
  lexer->lexeme.text = lexer->f_buf;
  lexer->lexeme.len = lexer->f_cur - lexer->f_buf;
  lexer->lexeme.l_line = lexer->line;
  lexer->lexeme.l_column = lexer->column;
}

void regex_lexer_retract(regex_lexer_t *lexer)
{
  while(lexer->f_cur > lexer->f_buf + lexer->lexeme.len) {
    regex_lexer_ungetc(lexer);
  }
  *lexer->f_cur = 0;

  lexer->line = lexer->lexeme.l_line;
  lexer->column = lexer->lexeme.l_column;
}

static void regex_lexer_buf_reserve(char **bufp, size_t *lenp, size_t len);

int regex_lexer_getc(regex_lexer_t *lexer)
{
  int c;
  regex_lexer_buf_reserve(&lexer->f_buf, &lexer->f_buf_len, lexer->f_cur - lexer->f_buf + 1);
  if(lexer->b_cur != lexer->b_buf) {
    c = *--lexer->b_cur;
  } else {
    c = fgetc(lexer->file);
  }
  if(c == EOF) return c;
  if(c == '\n') {
    ++lexer->line;
    lexer->column = 0;
  } else {
    ++lexer->column;
  }
  *lexer->f_cur++ = c;
  return c;
}

int regex_lexer_ungetc(regex_lexer_t *lexer)
{
  if(lexer->f_cur == lexer->f_buf) return 0;
  regex_lexer_buf_reserve(&lexer->b_buf, &lexer->b_buf_len, lexer->b_cur - lexer->b_buf + 1);
  *lexer->b_cur++ = *--lexer->f_cur;
  return 1;
}

void regex_lexer_buf_reserve(char **bufp, size_t *lenp, size_t len)
{
  size_t l;
  if(*lenp >= len) return;
  l = *lenp * 2;
  if(l > len) len = l;
  *bufp = (char *)Realloc(*bufp, len);
  *lenp = len;
}
