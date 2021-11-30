#include "mulprec.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void clear_by_zero(num_t *num) {
  num->is_neg = false;

  for (uint32_t i = 0; i < NUM_LEN; i++) {
    num->n[i] = 0;
  }
}

void display_num(const num_t *num) {
  if (num->is_neg) {
    printf("-");
  }

  // WARNING: 10^n進数でのみ機能する
  for (int32_t i = NUM_LEN - 1; i >= 0; i--) {
    printf("%09d", num->n[i]);
  }
}

void set_rnd(num_t *num, uint32_t k) {
  clear_by_zero(num);

  if (random() % 2 == 0)
    num->is_neg = true;

  for (uint32_t i = 0; i < k; i++) {
    num->n[i] = random() % NUM_BASE;
  }
}

void copy_num(const num_t *src, num_t *dst) {
  dst->is_neg = src->is_neg;

  for (uint32_t i = 0; i < NUM_LEN; i++) {
    dst->n[i] = src->n[i];
  }
}

void abs_num(const num_t *in, num_t *out) {
  copy_num(in, out);

  out->is_neg = false;
}

bool is_zero(const num_t *num) {
  if (num->is_neg)
    return false;

  for (uint32_t i = 0; i < NUM_LEN; i++) {
    if (num->n[i] != 0)
      return false;
  }

  return true;
}

status_t mul_by_base(const num_t *in, num_t *out) {
  out->is_neg = in->is_neg;

  out->n[0] = 0;
  for (uint32_t i = 0; i < NUM_LEN - 1; i++) {
    out->n[i + 1] = in->n[i];
  }

  if (in->n[NUM_LEN - 1] == 0) {
    return STAT_OK;
  } else {
    return STAT_ERR;
  }
}

int64_t div_by_base(const num_t *in, num_t *out) {
  out->is_neg = in->is_neg;

  out->n[0] = in->n[NUM_LEN - 1];
  for (uint32_t i = 1; i < NUM_LEN; i++) {
    out->n[i - 1] = in->n[i];
  }

  if (in->is_neg) {
    return in->n[0];
  } else {
    return -in->n[0];
  }
}

status_t set_int(int64_t in, num_t *out) {
  clear_by_zero(out);

  if (in < 0)
    out->is_neg = true;

  uint64_t tmp = in >= 0 ? in : -in;
  for (uint32_t i = 0; i < NUM_LEN; i++) {
    out->n[i] = tmp % NUM_BASE;
    tmp /= NUM_BASE;
  }

  if (tmp == 0) {
    return STAT_OK;
  } else {
    return STAT_ERR;
  }
}

status_t get_int(const num_t *in, int64_t *out) {
  num_t min, max;
  set_int(INT64_MIN + 1, &min);
  set_int(INT64_MAX, &max);

  if (comp_num(in, &min) < 0 || comp_num(&max, in) < 0)
    return STAT_ERR;

  int64_t sign = in->is_neg ? -1 : 1;

  for (uint32_t i = 0; i < NUM_LEN; i++) {
    int64_t pow = 1;
    for (uint32_t j = 0; j < i; j++)
      pow *= NUM_BASE;
    *out += in->n[i] * pow;
  }

  *out *= sign;

  return STAT_OK;
}

order_t comp_num(const num_t *a, const num_t *b) {
  // a >= 0, b < 0 => a > b
  if (!a->is_neg && b->is_neg)
    return ORDER_GT;
  // a < 0, b >= 0 => a < b
  if (a->is_neg && !b->is_neg)
    return ORDER_LT;
  // a >= 0, b >= 0
  if (!a->is_neg && !b->is_neg)
    for (int32_t i = NUM_LEN - 1; i >= 0; i--) {
      if (a->n[i] > b->n[i])
        return ORDER_GT;
      if (a->n[i] < b->n[i])
        return ORDER_LT;
    }
  // a < 0, b < 0
  if (!a->is_neg && !b->is_neg)
    for (int32_t i = NUM_LEN - 1; i >= 0; i--) {
      if (a->n[i] > b->n[i])
        return ORDER_LT;
      if (a->n[i] < b->n[i])
        return ORDER_GT;
    }

  return ORDER_EQ;
}

status_t add_num(const num_t *a, const num_t *b, num_t *out) {
  // a >= 0, b < 0 => a - |b|
  if (!a->is_neg && b->is_neg) {
    num_t c;
    abs_num(b, &c);
    return sub_num(a, &c, out);
  }
  // a < 0, b >= 0 => b - |a|
  if (a->is_neg && !b->is_neg) {
    num_t c;
    abs_num(a, &c);
    return sub_num(b, &c, out);
  }
  // a < 0, b < 0 => -(|a| + |b|)
  if (a->is_neg && b->is_neg) {
    num_t c, d;
    abs_num(a, &c);
    abs_num(b, &d);
    status_t stat = add_num(&c, &d, out);
    out->is_neg = true;
    return stat;
  }

  uint64_t carry = 0;

  for (uint32_t i = 0; i < NUM_LEN; i++) {
    uint64_t tmp = (uint64_t)a->n[i] + (uint64_t)b->n[i] + carry;
    out->n[i] = tmp % NUM_BASE;
    carry = tmp / NUM_BASE;
  }

  if (carry != 0)
    return STAT_ERR;

  return STAT_OK;
}

status_t sub_num(const num_t *a, const num_t *b, num_t *out) {
  // a >= 0, b < 0 => a + |b|
  if (!a->is_neg && b->is_neg) {
    num_t c;
    abs_num(b, &c);
    return add_num(a, &c, out);
  }
  // a < 0, b >= 0 => |a| + b
  if (a->is_neg && !b->is_neg) {
    num_t c;
    abs_num(a, &c);
    status_t stat = add_num(&c, b, out);
    out->is_neg = true;
    return stat;
  }
  // a < 0, b < 0 => |b| - |a|
  if (a->is_neg && b->is_neg) {
    num_t c, d;
    abs_num(a, &c);
    abs_num(b, &d);
    return sub_num(&d, &c, out);
  }
  // a < b => -(b - a)
  if (comp_num(a, b) == ORDER_LT) {
    status_t stat = sub_num(b, a, out);
    out->is_neg = true;
    return stat;
  }

  uint32_t borrow = 0;

  for (uint32_t i = 0; i < NUM_LEN; i++) {
    int64_t tmp = (int64_t)a->n[i] - (int64_t)b->n[i] - borrow;
    if (tmp < 0) {
      out->n[i] = tmp + NUM_BASE;
      borrow = 1;
    } else {
      out->n[i] = tmp;
      borrow = 0;
    }
  }

  if (borrow != 0)
    return STAT_ERR;

  return STAT_OK;
}

status_t mul_num(const num_t *a, const num_t *b, num_t *out) {
  if (!a->is_neg && b->is_neg) {
    num_t c;
    abs_num(b, &c);
    status_t stat = mul_num(a, &c, out);
    out->is_neg = true;
    return stat;
  }
  if (a->is_neg && !b->is_neg) {
    num_t c;
    abs_num(a, &c);
    status_t stat = mul_num(&c, b, out);
    out->is_neg = true;
    return stat;
  }
  if (a->is_neg && b->is_neg) {
    num_t c, d;
    abs_num(a, &c);
    abs_num(b, &d);
    return mul_num(&c, &d, out);
  }

  for (uint32_t i = 0; i < NUM_LEN; i++) {
    uint64_t carry = 0;
    num_t tmp;
    clear_by_zero(&tmp);

    for (uint32_t j = 0; j < NUM_LEN; j++) {
      uint64_t tmp_n = (uint64_t)a->n[j] * (uint64_t)b->n[i] + carry;

      if (i + j < NUM_LEN)
        tmp.n[i + j] = tmp_n % NUM_BASE;
      else if (tmp_n % NUM_BASE != 0)
        return STAT_ERR;

      carry = tmp_n / NUM_BASE;
    }

    if (carry != 0)
      return STAT_ERR;

    num_t adder;
    clear_by_zero(&adder);

    status_t stat = add_num(&tmp, out, &adder);
    if (stat != STAT_OK)
      return STAT_ERR;

    copy_num(&adder, out);
  }

  return STAT_OK;
}

status_t div_num(const num_t *a, const num_t *b, num_t *div, num_t *mod) {
  if (!a->is_neg && b->is_neg) {
    num_t c;
    abs_num(b, &c);
    status_t stat = div_num(a, &c, div, mod);
    div->is_neg = true;
    return stat;
  }
  if (a->is_neg && !b->is_neg) {
    num_t c;
    abs_num(a, &c);
    status_t stat = div_num(&c, b, div, mod);
    div->is_neg = true;
    mod->is_neg = true;
    return stat;
  }
  if (a->is_neg && b->is_neg) {
    num_t c, d;
    abs_num(a, &c);
    abs_num(b, &d);
    status_t stat = div_num(&c, &d, div, mod);
    mod->is_neg = true;
    return stat;
  }

  if (is_zero(b)) {
    return STAT_ERR;
  }

  num_t n, tmp;
  clear_by_zero(&n);
  clear_by_zero(&tmp);

  copy_num(a, &n);
  while (true) {
    if (comp_num(&n, b) == ORDER_LT)
      break;

    sub_num(&n, b, &tmp);
    copy_num(&tmp, &n);

    increment_num(div, &tmp);
    copy_num(&tmp, div);
  }

  copy_num(&n, mod);

  return 0;
}

status_t increment_num(const num_t *in, num_t *out) {
  num_t one;
  set_int(1, &one);

  return add_num(in, &one, out);
}

status_t decrement_num(const num_t *in, num_t *out) {
  num_t one;
  set_int(1, &one);

  return sub_num(in, &one, out);
}
