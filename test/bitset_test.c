#undef NDEBUG
#include "regex/bitset.h"
#include <stdio.h>

int main(void)
{
  bitset_t b0, b1, b2;
  bitset_create(&b0, 5);
  bitset_create(&b1, 5);
  bitset_create(&b2, 5);

  bitset_fill(&b0);
  bitset_unset(&b0, 4);
  bitset_print(&b0);

  bitset_empty(&b1);
  bitset_set(&b1, 0);
  bitset_print(&b1);

  printf("\n");

  bitset_union(&b0, &b1, &b2);
  bitset_print(&b2);

  bitset_intersect(&b0, &b1, &b2);
  bitset_print(&b2);

  bitset_diff(&b0, &b1, &b2);
  bitset_print(&b2);

  b0.data[0] = 0xfe;
  b1.data[0] = 0x1e;
  assert(bitset_equ(&b0, &b1));

  bitset_destroy(&b0);
  bitset_destroy(&b1);
  bitset_destroy(&b2);

  printf("\n");
  return 0;
}

#ifdef ENABLE_VISIBILITY
#include "regex/bitset.c"
#include "regex/util.c"
#endif  /* ENABLE_VISIBILITY */
