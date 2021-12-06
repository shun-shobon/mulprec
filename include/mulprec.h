#ifndef MULPREC_H
#define MULPREC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define NUM_LEN 3
#define NUM_BASE INT32_MAX

typedef enum {
  SIGN_POS,
  SIGN_NEG,
} sign_t;

typedef struct {
  int64_t n[NUM_LEN];
  sign_t sign;
  int32_t len;
} num_t;

typedef enum {
  STAT_OK,
  STAT_ERR,
} stat_t;

typedef enum {
  ORD_LT,
  ORD_EQ,
  ORD_GT,
} ord_t;

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define index_or_zero(num, i) ((i) < (num)->len ? (num)->n[i] : 0)

extern void set_sign(num_t *, sign_t);
extern sign_t get_sign(const num_t *);
extern void calc_len(num_t *);
extern void clear_by_zero(num_t *);
extern void print_num(const num_t *);
extern void set_rnd(num_t *, uint32_t);
extern void copy_num(const num_t *, num_t *);
extern void abs_num(const num_t *, num_t *);
extern stat_t mul_by_base(const num_t *, num_t *);
extern int64_t div_by_base(const num_t *, num_t *);
extern stat_t set_int(int64_t, num_t *);
extern stat_t get_int(const num_t *, int64_t *);
extern ord_t comp_num(const num_t *, const num_t *);
extern stat_t add_num(const num_t *, const num_t *, num_t *);
extern stat_t sub_num(const num_t *, const num_t *, num_t *);
extern stat_t mul_num(const num_t *, const num_t *, num_t *);
extern stat_t div_num(const num_t *, const num_t *, num_t *, num_t *);
extern stat_t increment_num(const num_t *, num_t *);
extern stat_t decrement_num(const num_t *, num_t *);

#endif // MULPREC_H
