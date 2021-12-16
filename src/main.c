#include <stdint.h>
#include <stdio.h>

#include "fft.h"
#include "mulprec.h"

int main(void) {
  setup_fft();

  int32_t shift = 2;

  num_t four = ZERO_NUM;
  four.n[0] = 4;

  num_t sqrt_inv;
  sqrt2_inv(shift - 1, &sqrt_inv);

  num_t coefficient;
  mul_num(&four, &sqrt_inv, &coefficient);

  num_t numerator = ZERO_NUM;
  numerator.len = shift + 1;
  numerator.n[shift] = 1;

  num_t sum = ZERO_NUM;
  int64_t n = 1000000;
  for (int64_t i = 0; i < n; i++) {
    if (((i + 1) * 100) % n == 0) {
      printf("\rProcessing... %lld%%", ((i + 1) * 100) / n);
      fflush(stdout);
    }
    num_t denominator = ZERO_NUM;
    num_t tmp;

    num_t l;
    denominator.n[0] = 4 * i + 1;
    div_num(&numerator, &denominator, &l, &tmp);

    num_t r;
    denominator.n[0] = 4 * i + 3;
    div_num(&numerator, &denominator, &r, &tmp);

    num_t lr;
    add_num(&l, &r, &lr);

    if (i % 2 == 0) {
      add_num(&sum, &lr, &sum);
    } else {
      sub_num(&sum, &lr, &sum);
    }
  }

  num_t pi;
  mul_num(&coefficient, &sum, &pi);

  shift_right(&pi, &pi, shift);

  printf("\n");
  print_num(&pi);
  printf("\n");

  return 0;
}
