#include <stdarg.h>
#include <stdio.h>

#include "util.h"

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
