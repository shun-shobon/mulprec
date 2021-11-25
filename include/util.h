#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

extern int lprintf(FILE *, const char *, int, const char *, const char *, ...);

#define debug(...) lprintf(stderr, __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif // UTIL_H
