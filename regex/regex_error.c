#include "regex_impl.h"
#include <stdio.h>

void parse_regex_error(regex_t *regex, const char *msg)
{
  char buf[16], *p = buf;
  const char *s = regex->cur;

  for(unsigned i = 0; i < 5; ++i) {
    if(s == regex->str) break;
    --s;
  }
  while(s != regex->cur) *p++ = *s++;

  *p++ = '#';

  for(unsigned i = 0; i < 5; ++i) {
    if(*s == 0) break;
    *p++ = *s++;
  }
  *p = 0;

  fprintf(stderr, "ERROR: %s: ...%s...: %s\n", __func__, buf, msg);
}
