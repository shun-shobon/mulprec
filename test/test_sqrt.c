#include <stdio.h>

#include "fft.h"
#include "mulprec.h"

int main() {
  setup_fft();

  num_t inv;
  sqrt2_inv(240, &inv);

  num_t two;
  set_int(2, &two);

  num_t out;
  mul_num(&inv, &two, &out);

  print_num(&out);
  printf("\n");

  return 0;
}