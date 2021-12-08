#include <stdio.h>

#include "mulprec.h"
#include "util.h"

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <x> <y>\n", argv[0]);
    return 1;
  }

  num_t x, y, z;

  assert_ok(input_num(argv[1], &x));
  assert_ok(input_num(argv[2], &y));

  assert_ok(sub_num(&x, &y, &z));

  print_num(&z);
  printf("\n");

  return 0;
}
