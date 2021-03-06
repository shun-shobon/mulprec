#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// clang-format off
// 10 ^ NUM_BASE_DIGIT = NUM_DEC_MAX <= NUM_BASE
// 2 ^ NUM_BASE_POW_2 = NUM_BASE
#define NUM_LEN        500
#define NUM_BASE       134217728
#define NUM_DEC_MAX    100000000
#define NUM_BASE_DIGIT 8
#define NUM_BASE_POW_2 27
// clang-format on

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
  STAT_OK = 0,
  STAT_OVERFLOW,
  STAT_ZERO_DIV,
  STAT_UNKNOWN,
} stat_t;

typedef enum {
  ORD_LT,
  ORD_EQ,
  ORD_GT,
} ord_t;

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

#define guard_stat(x)                                                          \
  {                                                                            \
    stat_t stat = x;                                                           \
    if (stat != STAT_OK)                                                       \
      return stat;                                                             \
  }

#define ZERO_NUM ((num_t){.sign = SIGN_POS, .len = 1, .n = {0}})
#define ONE_NUM ((num_t){.sign = SIGN_POS, .len = 1, .n = {1}})

#define max(a, b) ((a) > (b) ? (a) : (b))
#define index_or_zero(num, i) ((i) < (num)->len ? (num)->n[i] : 0)

extern void set_sign(num_t *, sign_t);
extern sign_t get_sign(const num_t *);
extern void print_num(const num_t *);
extern stat_t input_num(const char *, num_t *);
extern void copy_num(const num_t *, num_t *);
extern stat_t shift_left(const num_t *, num_t *, int32_t);
extern stat_t shift_right(const num_t *, num_t *, int32_t);
extern stat_t bit_shift_left(const num_t *, num_t *, int32_t);
extern stat_t bit_shift_right(const num_t *, num_t *, int32_t);
extern stat_t set_int(int64_t, num_t *);
extern ord_t comp_num(const num_t *, const num_t *);
extern stat_t add_num(const num_t *, const num_t *, num_t *);
extern stat_t sub_num(const num_t *, const num_t *, num_t *);
extern stat_t mul_num(const num_t *, const num_t *, num_t *);
extern stat_t div_num(const num_t *, const num_t *, num_t *, num_t *);
extern stat_t increment_num(const num_t *, num_t *);
extern stat_t calc_sqrt2_inv(int32_t, num_t *);
