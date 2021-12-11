#include "util.h"

#include <stdio.h>
#include <stdlib.h>

#include "mulprec.h"

void assert_ok(stat_t stat) {
  if (stat != STAT_OK) {
    printf("Error: %s\n", "error");
    exit(1);
  }
}

void *malloc_safe_internal(size_t size, const char *file, int line) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    fprintf(stderr, "[Fatal]%s:%d: Memory allocation failed(%zu bytes)\n", file,
            line, size);
    abort();
  }

  return ptr;
}
