#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "mulprec.h"

int main(void) {
  int64_t a = 5432109876543210;
  num_t b;
  set_int(a, &b);

  printf("a: %" PRIi64 "\n", a);
  printf("b: ");
  print_num(&b);
  printf("\n");

  printf("Test passed\n");
  return 0;
}