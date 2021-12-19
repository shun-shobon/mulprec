#include <stdio.h>

#include "mulprec.h"

int main() {
  num_t inv;
  calc_sqrt2_inv(100, &inv);

  num_t out;
  bit_shift_left(&inv, &out, 1);

  print_num(&out);
  printf("\n");

  return 0;
}