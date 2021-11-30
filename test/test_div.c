#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mulprec.h"

int main(void) {
  srandom(time(NULL));

  for (uint32_t i = 0; i < 1000000; i++) {
    int64_t x = (random() - (RAND_MAX / 2)) * 4;
    int64_t y = (random() - (RAND_MAX / 2)) * 4;

    if (y == 0)
      y = 1;

    num_t a, b, c, d;
    set_int(x, &a);
    set_int(y, &b);
    clear_by_zero(&c);
    clear_by_zero(&d);

    status_t stat = div_num(&a, &b, &c, &d);
    int64_t z = 0;
    int64_t w = 0;
    get_int(&c, &z);
    get_int(&d, &w);

    if (x / y == z && x % y == w && stat == STAT_OK)
      continue;

    printf("Test failed\n");
    printf("  x: %lld\n", x);
    printf("  a: ");
      display_num(&a);
    printf("\n");
    printf("  y: %lld\n", y);
    printf("  b: ");
      display_num(&b);
    printf("\n");
    printf("  x / y: %lld\n", x / y);
    printf("  a / b: ");
      display_num(&c);
    printf("\n");
    printf("  x %% y: %lld\n", x % y);
    printf("  a %% b: ");
      display_num(&d);
    printf("\n");
    printf("  stat: %d\n", stat);

    return -1;
  }

  printf("Test passed\n");
  return 0;
}
