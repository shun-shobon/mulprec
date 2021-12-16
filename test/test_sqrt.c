#include <stdio.h>

#include "fft.h"
#include "mulprec.h"

int main() {
  setup_fft();

  num_t inv;
  sqrt2_inv(100, &inv);

  //  num_t two = ZERO_NUM;
  //  two.n[0] = 2;
  //
  //  num_t out;
  //  mul_num(&inv, &two, &out);

  print_num(&inv);
  printf("\n");

  return 0;
}