#include "util.h"

#include <stdio.h>

#include "mulprec.h"

void assert_ok(stat_t stat) {
  if (stat != STAT_OK) {
    printf("Error: %s\n", "error");
    exit(1);
  }
}
