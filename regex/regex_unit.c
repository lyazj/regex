#include "regex_impl.h"
#include "charset.h"

static int parse_regex_class(regex_t *, charset_t *);

int parse_regex_unit(regex_t *regex, charset_t *sp)
{
  if(*regex->cur == '[') {
    return parse_regex_class(regex, sp);
  }
  return parse_regex_escaped_unit(regex, sp);
}

int parse_regex_class(regex_t *regex, charset_t *sp)
{
  charset_t s0;
  void (*reduce)(const charset_t *s1, const charset_t *s2, charset_t *s);

  if(*regex->cur != '[') {
    parse_regex_error(regex, "expect \"[\"");
    return 0;
  }
  ++regex->cur;

  if(*regex->cur == '^') {  /* inverse */
    charset_fill(&s0);
    reduce = charset_diff;
    ++regex->cur;
  } else {
    charset_empty(&s0);
    reduce = charset_union;
  }

  while(*regex->cur != ']') {
    charset_t s;
    if(!parse_regex_class_escaped_unit(regex, &s)) return 0;

    if(*regex->cur == '-') {  /* range */
      charset_t s2;
      unsigned char uc, uc2;

      if(!charset_get_unique(&s, &uc)) {
        parse_regex_error(regex, "expect unique charset");
        return 0;
      }
      ++regex->cur;

      if(!parse_regex_class_escaped_unit(regex, &s2)) return 0;
      if(!charset_get_unique(&s2, &uc2)) {
        parse_regex_error(regex, "expect unique charset");
        return 0;
      }
      if(uc > uc2) {
        parse_regex_error(regex, "empty range");
        return 0;
      }

      for(unsigned u = uc + 1; u <= uc2; ++u) {
        charset_set(&s, u);
      }
    }

    reduce(&s0, &s, &s0);
  }
  ++regex->cur;

  *sp = s0;
  return 1;
}
