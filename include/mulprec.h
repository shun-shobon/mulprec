#ifndef MULPREC_H
#define MULPREC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define NUM_LEN 3
#define NUM_BASE 1000000000

typedef enum {
  SIGN_POS,
  SIGN_NEG,
} sign_t;

typedef struct {
  uint32_t n[NUM_LEN];
  sign_t sign;
} num_t;

typedef int8_t status_t;
#define STAT_OK 0
#define STAT_ERR (-1)

typedef enum {
  ORDER_LT,
  ORDER_EQ,
  ORDER_GT,
} order_t;

extern void set_sign(num_t *, sign_t);
extern sign_t get_sign(const num_t *);
extern void clear_by_zero(num_t *);
extern void display_num(const num_t *);
extern void set_rnd(num_t *, uint32_t);
extern void copy_num(const num_t *, num_t *);
extern void abs_num(const num_t *, num_t *);
extern status_t mul_by_base(const num_t *, num_t *);
extern int64_t div_by_base(const num_t *, num_t *);
extern status_t set_int(int64_t, num_t *);
extern status_t get_int(const num_t *, int64_t *);
extern order_t comp_num(const num_t *, const num_t *);
extern status_t add_num(const num_t *, const num_t *, num_t *);
extern status_t sub_num(const num_t *, const num_t *, num_t *);
extern status_t mul_num(const num_t *, const num_t *, num_t *);
extern status_t div_num(const num_t *, const num_t *, num_t *, num_t *);
extern status_t increment_num(const num_t *, num_t *);
extern status_t decrement_num(const num_t *, num_t *);

#endif // MULPREC_H
