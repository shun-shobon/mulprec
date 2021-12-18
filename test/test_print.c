#include <stdio.h>

#include "mulprec.h"

int main(void) {
  char *in = "-12345678901234567890123456789012345";
  num_t out;
  input_num(in, &out);

  printf("in : %s\n", in);
  printf("out: ");
  print_num(&out);
  printf("\n");

  printf("Test passed\n");
  return 0;
}