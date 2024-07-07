#include "regex_impl.h"
#include "bitset.h"
#include "util.h"
#include <stdlib.h>

typedef struct regex_char_info_t {
  bitset_t accepted;
  unsigned char ch;
} regex_char_info_t;

static int regex_char_info_cmp(const regex_char_info_t *i1, const regex_char_info_t *i2)
{
  int r = bitset_cmp(&i1->accepted, &i2->accepted);
  if(r) return r;
  return (int)i1->ch - (int)i2->ch;
}

void regex_make_char_class(regex_t *regex)
{
  regex_char_info_t info[256];
  bitset_t *last = NULL;
  unsigned lastid = 0;

  for(unsigned i = 0; i < 256; ++i) {
    bitset_create(&info[i].accepted, regex->nunit + 1);
    bitset_empty(&info[i].accepted);
    info[i].ch = i;
  }

  for(int i = 1; i <= regex->nunit; ++i) {
    unsigned char uc;
    if(charset_get_first(&regex->units[i]->charset, &uc)) do {
      bitset_set(&info[uc].accepted, i);
    } while(charset_get_next(&regex->units[i]->charset, &uc));
  }

  qsort(info, 256, sizeof(regex_char_info_t), (int(*)(const void*, const void*))regex_char_info_cmp);

  regex->char_class = (unsigned char *)Malloc(256);
  for(unsigned i = 0; i < 256; ++i) {
    bitset_t *cur = &info[i].accepted;
    if(last && !bitset_equ(cur, last)) ++lastid;
    last = cur;
    regex->char_class[info[i].ch] = lastid;
  }
  regex->nclass = lastid + 1;

  for(unsigned i = 0; i < 256; ++i) {
    bitset_destroy(&info[i].accepted);
  }
}
