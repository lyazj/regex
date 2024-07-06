#include "regex_impl.h"
#include "charset.h"

static int parse_regex_an_escaped_unit(regex_t *regex, charset_t *sp, int inside_class);

int parse_regex_escaped_unit(regex_t *regex, charset_t *sp)
{
  return parse_regex_an_escaped_unit(regex, sp, 0);
}

int parse_regex_class_escaped_unit(regex_t *regex, charset_t *sp)
{
  return parse_regex_an_escaped_unit(regex, sp, 1);
}

static int parse_regex_octet(regex_t *regex, unsigned char *ucp);

int parse_regex_an_escaped_unit(regex_t *regex, charset_t *sp, int inside_class)
{
  const char *metachars;
  if(inside_class) {
    metachars = "-[]";
  } else {
    metachars = "*+?|()[]";
  }

  if(*regex->cur == '\\') {
    ++regex->cur;
    if(*regex->cur == 0) {
      parse_regex_error(regex, "incomplete escaping sequence");
      return 0;
    }
    if(*regex->cur >= '0' && *regex->cur <= '7') {
      unsigned char uc;
      if(!parse_regex_octet(regex, &uc)) return 0;
      charset_empty(sp);
      charset_set(sp, uc);
      return 1;
    }
    *sp = regex->config->escape[(unsigned char)*regex->cur];
    ++regex->cur;
    return 1;
  }

  if(*regex->cur == 0) {
    parse_regex_error(regex, "unexpected end of string");
    return 0;
  }
  if(strchr(metachars, *regex->cur)) {
    parse_regex_error(regex, "unexpected meta character");
    return 0;
  }
  if(!inside_class && *regex->cur == '.') {
    charset_fill(sp);
  } else {
    charset_empty(sp);
    charset_set(sp, *regex->cur);
  }
  ++regex->cur;
  return 1;
}

int parse_regex_octet(regex_t *regex, unsigned char *ucp)
{
  unsigned u = 0, n;
  for(n = 0; n < 3; ++n) {
    if(*regex->cur < '0' && *regex->cur > '7') break;
    u *= 8;
    u += *regex->cur - '0';
    ++regex->cur;
  }
  if(n == 0) {
    parse_regex_error(regex, "expect octet digit");
    return 0;
  }
  if(u >= 256) {
    parse_regex_error(regex, "character code overflow");
    return 0;
  }
  *ucp = u;
  return 1;
}
