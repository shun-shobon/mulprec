#include "util.h"

#include <stdarg.h>
#include <stdio.h>

#include "mulprec.h"

int lprintf(FILE *fp, const char *file, int line, const char *func,
            const char *fmt, ...) {
  flockfile(fp);

  int n = 0;

  n += fprintf(fp, "%s:%d:%s: ", file, line, func);

  va_list ap;
  va_start(ap, fmt);
  n += vfprintf(fp, fmt, ap);
  va_end(ap);

  n += fprintf(fp, "\n");

  funlockfile(fp);

  return n;
}

void assert_ok(stat_t stat) {
  if (stat != STAT_OK) {
    printf("Error: %s\n", "error");
    exit(1);
  }
}
