#include <stdio.h>

#include "mulprec.h"
#include "util.h"

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <x> <y>\n", argv[0]);
    return 1;
  }

  num_t x, y;

  assert_ok(input_num(argv[1], &x));
  assert_ok(input_num(argv[2], &y));

  ord_t ord = comp_num(&x, &y);

  switch (ord) {
  case ORD_GT:
    puts("GT");
    break;
  case ORD_EQ:
    puts("EQ");
    break;
  case ORD_LT:
    puts("LT");
    break;
  }

  return 0;
}
