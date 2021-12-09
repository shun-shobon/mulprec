#include <stdio.h>

#include "mulprec.h"
#include "util.h"

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <x> <y>\n", argv[0]);
    return 1;
  }

  num_t x, y, z, w;

  assert_ok(input_num(argv[1], &x));
  assert_ok(input_num(argv[2], &y));

  assert_ok(div_num(&x, &y, &z, &w));

  print_num(&z);
  printf("\n");
  print_num(&w);
  printf("\n");

  return 0;
}
