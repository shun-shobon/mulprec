#ifndef UTIL_H
#define UTIL_H

#include "mulprec.h"

extern void assert_ok(stat_t);
extern void *malloc_safe_internal(size_t, const char *, int);

#define malloc_safe(size) malloc_safe_internal(size, __FILE__, __LINE__)

#endif // UTIL_H
