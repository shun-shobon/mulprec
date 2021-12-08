#ifndef UTIL_H
#define UTIL_H

#include "mulprec.h"
#include <stdio.h>

extern int lprintf(FILE *, const char *, int, const char *, const char *, ...);
extern void assert_ok(stat_t stat);

#define debug(...) lprintf(stderr, __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif // UTIL_H
