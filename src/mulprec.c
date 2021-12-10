#include "mulprec.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fft.h"

void set_sign(num_t *num, sign_t sign) { num->sign = sign; }
sign_t get_sign(const num_t *num) { return num->sign; }

void clear_by_zero(num_t *num) {
  set_sign(num, SIGN_POS);

  for (uint32_t i = 0; i < NUM_LEN; i++) {
    num->n[i] = 0;
  }

  num->len = 1;
}

void print_num(const num_t *num) {
  int64_t base = 10;

  int64_t tmp[num->len];
  for (uint32_t i = 0; i < num->len; i++)
    tmp[i] = num->n[i];

  char buf[(size_t)(log10(NUM_BASE) * num->len + 1)];
  int32_t idx = 0;

  int32_t start = num->len - 1;
  while (start >= 0) {
    int64_t carry = 0;
    for (int32_t i = start; i >= 0; i--) {
      int64_t val = carry * NUM_BASE + tmp[i];
      tmp[i] = val / base;
      carry = val % base;
    }

    buf[idx++] = (char)('0' + carry);

    if (tmp[start] == 0)
      start--;
  }

  if (num->sign == SIGN_NEG)
    putchar('-');
  for (int32_t i = idx - 1; i >= 0; i--)
    putchar(buf[i]);
}

stat_t input_num(const char *src, num_t *dst) {
  int64_t base = 10;

  if (src[0] == '-') {
    set_sign(dst, SIGN_NEG);
    src++;
  } else {
    set_sign(dst, SIGN_POS);
  }

  int32_t len = (int32_t)strlen(src);

  int64_t tmp[len];
  for (uint32_t i = 0; i < len; i++)
    tmp[i] = src[len - i - 1] - '0';

  dst->len = 0;

  int32_t start = len - 1;
  while (start >= 0) {
    int64_t carry = 0;
    for (int32_t i = start; i >= 0; i--) {
      int64_t val = carry * base + tmp[i];
      tmp[i] = val / NUM_BASE;
      carry = val % NUM_BASE;
    }

    dst->n[dst->len++] = carry;
    if (dst->len == NUM_LEN)
      return STAT_ERR;

    while (tmp[start] == 0)
      start--;
  }

  return STAT_OK;
}

void set_rnd(num_t *num, uint32_t k) {
  if (random() % 2 == 0)
    set_sign(num, SIGN_NEG);

  for (uint32_t i = 0; i < k; i++) {
    num->n[i] = random() % NUM_BASE;
  }

  num->len = (int32_t)k;
}

void copy_num(const num_t *src, num_t *dst) {
  set_sign(dst, get_sign(src));

  dst->len = src->len;

  for (uint32_t i = 0; i < src->len; i++) {
    dst->n[i] = src->n[i];
  }
}

void abs_num(const num_t *in, num_t *out) {
  copy_num(in, out);

  set_sign(out, SIGN_POS);
}

bool is_zero(const num_t *num) {
  if (get_sign(num) == SIGN_NEG)
    return false;

  for (uint32_t i = 0; i < num->len; i++) {
    if (num->n[i] != 0)
      return false;
  }

  return true;
}

stat_t mul_by_base(const num_t *in, num_t *out) {
  set_sign(out, get_sign(in));

  out->n[0] = 0;
  for (uint32_t i = 0; i < in->len - 1; i++) {
    out->n[i + 1] = in->n[i];
  }

  out->len = min(in->len + 1, NUM_LEN);

  if (in->n[NUM_LEN - 1] == 0) {
    return STAT_OK;
  } else {
    return STAT_ERR;
  }
}

int64_t div_by_base(const num_t *in, num_t *out) {
  if (in->len <= 1) {
    set_sign(out, SIGN_POS);
    out->len = 1;
    out->n[0] = 0;
    switch (get_sign(in)) {
    case SIGN_POS:
      return in->n[0];
    case SIGN_NEG:
      return -in->n[0];
    }
  }

  set_sign(out, get_sign(in));

  for (uint32_t i = 1; i < in->len; i++) {
    out->n[i - 1] = in->n[i];
  }

  out->len = max(in->len - 1, 0);

  switch (get_sign(in)) {
  case SIGN_POS:
    return in->n[0];
  case SIGN_NEG:
    return -in->n[0];
  }
}

stat_t set_int(int64_t in, num_t *out) {
  *out = ZERO_NUM;

  if (in < 0)
    set_sign(out, SIGN_NEG);

  int64_t tmp = in >= 0 ? in : -in;
  for (int32_t i = 0; i < NUM_LEN; i++) {
    out->n[i] = tmp % NUM_BASE;
    tmp /= NUM_BASE;

    if (tmp == 0) {
      out->len = i + 1;
      break;
    }
  }

  if (tmp == 0) {
    return STAT_OK;
  } else {
    out->len = NUM_LEN;
    return STAT_ERR;
  }
}

stat_t get_int(const num_t *in, int64_t *out) {
  num_t min, max;
  set_int(INT64_MIN + 1, &min);
  set_int(INT64_MAX, &max);

  if (comp_num(in, &min) == ORD_LT || comp_num(&max, in) == ORD_LT)
    return STAT_ERR;

  int64_t sign = get_sign(in) == SIGN_POS ? 1 : -1;

  for (uint32_t i = 0; i < in->len; i++) {
    int64_t pow = 1;
    for (uint32_t j = 0; j < i; j++)
      pow *= NUM_BASE;
    *out += in->n[i] * pow;
  }

  *out *= sign;

  return STAT_OK;
}

static stat_t fix_num(num_t *num) {
  // 通常の繰り上がり/繰り下がり処理
  for (uint32_t i = 0; i < num->len - 1; i++) {
    if (NUM_BASE <= num->n[i]) {
      int64_t carry = num->n[i] / NUM_BASE;
      num->n[i] -= carry * NUM_BASE;
      num->n[i + 1] += carry;
    }
    if (num->n[i] < 0) {
      int64_t borrow = (-num->n[i] - 1) / NUM_BASE + 1;
      num->n[i] += borrow * NUM_BASE;
      num->n[i + 1] -= borrow;
    }
  }

  // 最終桁の繰り上がり/繰り下がり処理
  // XXX: 合ってるか不明
  while (num->len < NUM_LEN &&
         (num->n[num->len - 1] < 0 || NUM_BASE <= num->n[NUM_LEN - 1])) {
    if (NUM_BASE <= num->n[num->len - 1]) {
      int64_t carry = num->n[num->len - 1] / NUM_BASE;
      num->n[num->len - 1] -= carry * NUM_BASE;
      num->n[num->len] = carry;
    }
    if (num->n[num->len - 1] < 0) {
      int64_t borrow = (-num->n[num->len] - 1) / NUM_BASE + 1;
      num->n[num->len - 1] += borrow * NUM_BASE;
      num->n[num->len] = -borrow;
    }
    num->len++;
  }

  // 最終桁の0を消し去る
  while (1 < num->len && num->n[num->len - 1] == 0)
    num->len--;

  // オーバーフロー/アンダーフロー判定
  if (0 <= num->n[NUM_LEN - 1] && num->n[NUM_LEN - 1] < NUM_BASE)
    return STAT_OK;
  else
    return STAT_ERR;
}

static ord_t comp_num_nat(const num_t *a, const num_t *b) {
  if (a->len > b->len)
    return ORD_GT;
  if (a->len < b->len)
    return ORD_LT;

  for (int32_t i = a->len - 1; i >= 0; i--) {
    if (a->n[i] > b->n[i])
      return ORD_GT;
    if (a->n[i] < b->n[i])
      return ORD_LT;
  }

  return ORD_EQ;
}

ord_t comp_num(const num_t *a, const num_t *b) {
  // a >= 0, b < 0 => a > b
  if (get_sign(a) == SIGN_POS && get_sign(b) == SIGN_NEG)
    return ORD_GT;
  // a < 0, b >= 0 => a < b
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_POS)
    return ORD_LT;
  // a >= 0, b >= 0
  if (get_sign(a) == SIGN_POS && get_sign(b) == SIGN_POS)
    return comp_num_nat(a, b);
  // a < 0, b < 0
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_NEG) {
    return comp_num_nat(b, a);
  }

  return ORD_EQ;
}

static stat_t add_num_nat(const num_t *a, const num_t *b, num_t *out) {
  out->len = max(a->len, b->len);

  for (uint32_t i = 0; i < out->len; i++) {
    out->n[i] = index_or_zero(a, i) + index_or_zero(b, i);
  }

  return fix_num(out);
}

static stat_t sub_num_nat(const num_t *a, const num_t *b, num_t *out) {
  out->len = max(a->len, b->len);

  for (uint32_t i = 0; i < out->len; i++) {
    out->n[i] = index_or_zero(a, i) - index_or_zero(b, i);
  }

  return fix_num(out);
}

static stat_t mul_num_nat(const num_t *a, const num_t *b, num_t *out) {
  out->len = a->len + b->len - 1;

#ifdef false
  for (uint32_t i = 0; i < b->len; i++) {
    for (uint32_t j = 0; j < a->len; j++) {
      int64_t tmp = a->n[j] * b->n[i];

      if (NUM_LEN - 1 < i + j && tmp != 0)
        return STAT_ERR;

      // WARNING: オーバーフローの可能性あり
      out->n[i + j] += tmp;
    }
  }
#endif

  int32_t n = get_convolution_size(a->len, b->len);
  convolution(a->n, a->len, b->n, b->len, out->n, n);

  return fix_num(out);
}

static stat_t div_num_nat(const num_t *a, const num_t *b, num_t *div,
                          num_t *mod) {
  *div = ZERO_NUM;
  *mod = ZERO_NUM;

  if (is_zero(b))
    return STAT_ERR;

  num_t tmp;

  copy_num(a, mod);
  while (true) {
    if (comp_num_nat(mod, b) == ORD_LT)
      break;

    sub_num_nat(mod, b, &tmp);
    copy_num(&tmp, mod);

    increment_num(div, &tmp);
    copy_num(&tmp, div);
  }

  return STAT_OK;
}

stat_t add_num(const num_t *a, const num_t *b, num_t *out) {
  // a >= 0, b < 0 => a - |b|
  if (get_sign(a) == SIGN_POS && get_sign(b) == SIGN_NEG) {
    // |a| > |b| => a - |b|
    if (comp_num_nat(a, b) == ORD_GT)
      return sub_num_nat(a, b, out);
    // |a| < |b| => -(a - |b|)
    else {
      stat_t stat = sub_num_nat(b, a, out);
      set_sign(out, SIGN_NEG);
      return stat;
    }
  }
  // a < 0, b >= 0 => b - |a|
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_POS) {
    // b > |a| => b - |a|
    if (comp_num_nat(b, a) == ORD_GT)
      return sub_num_nat(b, a, out);
    // b < |a| => -(|a| - b)
    else {
      stat_t stat = sub_num_nat(a, b, out);
      set_sign(out, SIGN_NEG);
      return stat;
    }
  }
  // a < 0, b < 0 => -(|a| + |b|)
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_NEG) {
    stat_t stat = add_num_nat(a, b, out);
    set_sign(out, SIGN_NEG);
    return stat;
  }

  return add_num_nat(a, b, out);
}

stat_t sub_num(const num_t *a, const num_t *b, num_t *out) {
  // a >= 0, b < 0 => a + |b|
  if (get_sign(a) == SIGN_POS && get_sign(b) == SIGN_NEG) {
    return add_num_nat(a, b, out);
  }
  // a < 0, b >= 0 => -(|a| + b)
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_POS) {
    stat_t stat = add_num_nat(a, b, out);
    set_sign(out, SIGN_NEG);
    return stat;
  }
  // a < 0, b < 0 => |b| - |a|
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_NEG) {
    // |b| > |a| => |b| - |a|
    if (comp_num_nat(b, a) == ORD_GT)
      return sub_num_nat(b, a, out);
    // |b| < |a| => -(|a| - |b|)
    else {
      stat_t stat = sub_num_nat(a, b, out);
      set_sign(out, SIGN_NEG);
      return stat;
    }
  }
  // a < b => -(b - a)
  if (comp_num(a, b) == ORD_LT) {
    stat_t stat = sub_num_nat(b, a, out);
    set_sign(out, SIGN_NEG);
    return stat;
  }

  return sub_num_nat(a, b, out);
}

stat_t mul_num(const num_t *a, const num_t *b, num_t *out) {
  // a >= 0, b < 0 => -(a * |b|)
  if (get_sign(a) == SIGN_POS && get_sign(b) == SIGN_NEG) {
    stat_t stat = mul_num_nat(a, b, out);
    set_sign(out, SIGN_NEG);
    return stat;
  }
  // a < 0, b >= 0 => -(|a| * |b|)
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_POS) {
    stat_t stat = mul_num_nat(a, b, out);
    set_sign(out, SIGN_NEG);
    return stat;
  }
  // a < 0, b < 0 => |a| * |b|
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_NEG) {
    return mul_num_nat(a, b, out);
  }

  return mul_num_nat(a, b, out);
}

stat_t div_num(const num_t *a, const num_t *b, num_t *div, num_t *mod) {
  // a >= 0, b < 0 => -(a / |b|), a % |b|
  if (get_sign(a) == SIGN_POS && get_sign(b) == SIGN_NEG) {
    stat_t stat = div_num_nat(a, b, div, mod);
    set_sign(div, SIGN_NEG);
    return stat;
  }
  // a < 0, b >= 0 => -(|a| / b), -(|a| % b)
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_POS) {
    stat_t stat = div_num_nat(a, b, div, mod);
    set_sign(div, SIGN_NEG);
    set_sign(mod, SIGN_NEG);
    return stat;
  }
  // a < 0, b < 0 => |a| / |b|, -(|a| % |b|)
  if (get_sign(a) == SIGN_NEG && get_sign(b) == SIGN_NEG) {
    stat_t stat = div_num_nat(a, b, div, mod);
    set_sign(mod, SIGN_NEG);
    return stat;
  }

  return div_num_nat(a, b, div, mod);
}

stat_t increment_num(const num_t *in, num_t *out) {
  return add_num(in, &ONE_NUM, out);
}

stat_t decrement_num(const num_t *in, num_t *out) {
  return sub_num(in, &ONE_NUM, out);
}
